#include "Animator.h"

#include "../../../External/ImGui/imgui.h"
#include "../../../External/ImGui/imgui_internal.h"
using namespace DirectX;

// void Animator::AddBlendAnimation(
// 	const std::string&                               name,
// 	ModelResource::SceneView*                        sceneView,
// 	std::initializer_list<ModelResource::Animation*> animations
// ) {
// 	BlendTree state;
// 	state.parameter = {};
// 	state.timer     = 0;
// 	_sceneView      = sceneView;
//
// 	int   count      = 0;
// 	int   size       = animations.size() - 1;
// 	float maxSeconds = 0;
// 	for (ModelResource::Animation* animation: animations) {
// 		Motion motion = {animation, count / static_cast<float>(size), 1};
// 		state.motions.push_back(motion);
// 		count++;
// 		if (maxSeconds < animation->secondsLength) maxSeconds = animation->secondsLength;
// 	}
//
// 	state.maxSeconds = maxSeconds;
// 	_state[name]     = state;
// }

ModelResource::KeyFrame Animator::PlayAnimation(const std::string& name, float elapsedTime, float rate) {
	BlendTree& state = _state[name];
	state.blendRate  = rate;

	const Motion* first  = nullptr;
	const Motion* second = nullptr;

	for (auto&& motion: state.motions) {
		if (state.blendRate >= motion.threshold) first = &motion;
		if (!second && state.blendRate < motion.threshold) second = &motion;
	}
	if (!second) second = first;
	if (!first) first = second;

	const float animationRate = state.timer / state.maxSeconds;
	const float leapRate      = second == first ?
		                            1 :
		                            (state.blendRate - first->threshold) / (second->threshold - first->threshold);

	float seq = first->motion->sequence.size() * std::fmodf(animationRate * first->animationSpeed, 1.f);
	//float seq = first->motion->sequence.size() * animationRate;
	int   firstKeyFrameIndex = static_cast<int>(seq);
	float firstLeapRate      = seq - static_cast<float>(firstKeyFrameIndex);

	const ModelResource::KeyFrame* firstKeyFrames[2] = {
		&first->motion->sequence.at(firstKeyFrameIndex),
		&first->motion->sequence.at(
			firstKeyFrameIndex >= first->motion->sequence.size() - 1 ? 0 : firstKeyFrameIndex + 1
		)
	};

	seq = second->motion->sequence.size() * std::fmodf(animationRate * second->animationSpeed, 1.f);
	int   secondKeyFrameIndex = static_cast<int>(seq);
	float secondLeapRate = seq - static_cast<float>(secondKeyFrameIndex);

	const ModelResource::KeyFrame* secondKeyFrames[2] = {
		&second->motion->sequence.at(secondKeyFrameIndex),
		&second->motion->sequence.at(
			secondKeyFrameIndex >= second->motion->sequence.size() - 1 ? 0 : secondKeyFrameIndex + 1
		)
	};

	ModelResource::KeyFrame resultKeyFrame;
	resultKeyFrame.nodes.resize(firstKeyFrames[0]->nodes.size());
	for (int count = 0; count < resultKeyFrame.nodes.size(); ++count) {
		ModelResource::KeyFrame::Node node;
		XMVECTOR                      scaleVec = XMVectorLerp(
			XMVectorLerp(
				XMLoadFloat3(&firstKeyFrames[0]->nodes[count].scaling),
				XMLoadFloat3(&firstKeyFrames[1]->nodes[count].scaling),
				firstLeapRate
			),
			XMVectorLerp(
				XMLoadFloat3(&secondKeyFrames[0]->nodes[count].scaling),
				XMLoadFloat3(&secondKeyFrames[1]->nodes[count].scaling),
				secondLeapRate
			),
			leapRate
		);
		XMVECTOR rotateVec = XMQuaternionSlerp(
			XMQuaternionSlerp(
				XMLoadFloat4(&firstKeyFrames[0]->nodes[count].rotation),
				XMLoadFloat4(&firstKeyFrames[1]->nodes[count].rotation),
				firstLeapRate
			),
			XMQuaternionSlerp(
				XMLoadFloat4(&secondKeyFrames[0]->nodes[count].rotation),
				XMLoadFloat4(&secondKeyFrames[1]->nodes[count].rotation),
				secondLeapRate
			),
			leapRate
		);
		XMVECTOR translationVec = XMVectorLerp(
			XMVectorLerp(
				XMLoadFloat3(&firstKeyFrames[0]->nodes[count].translation),
				XMLoadFloat3(&firstKeyFrames[1]->nodes[count].translation),
				firstLeapRate
			),
			XMVectorLerp(
				XMLoadFloat3(&secondKeyFrames[0]->nodes[count].translation),
				XMLoadFloat3(&secondKeyFrames[1]->nodes[count].translation),
				secondLeapRate
			),
			leapRate
		);

		node.name     = firstKeyFrames[0]->nodes[count].name;
		node.uniqueId = firstKeyFrames[0]->nodes[count].uniqueId;

		XMStoreFloat3(&node.scaling, scaleVec);
		XMStoreFloat4(&node.rotation, rotateVec);
		XMStoreFloat3(&node.translation, translationVec);

		int64_t  parentID = _sceneView->nodes.at(_sceneView->GetIndex(node.uniqueId)).parentIndex;
		XMMATRIX GT       = parentID < 0 ?
			                    XMMatrixIdentity() :
			                    XMMatrixScalingFromVector(scaleVec) *
			                    XMMatrixRotationQuaternion(rotateVec) *
			                    XMMatrixTranslationFromVector(translationVec) * XMLoadFloat4x4(
				                    &resultKeyFrame.nodes[parentID].globalTransform
			                    );

		XMStoreFloat4x4(&node.globalTransform, GT);
		resultKeyFrame.nodes[count] = node;
	}

	state.timer += elapsedTime;
	//if (state.timer >= state.maxSeconds) state.timer = 0;

	return resultKeyFrame;
}

void Animator::AnimationEditor(const std::string& name) {
	if (ImGui::Begin("AnimationEditor")) {
		BlendTree& state = _state[name];

		if (ImGui::CollapsingHeader("State")) {
			ImGui::Text("BlendRate: %f", state.blendRate);
			ImGui::Text("MaxSeconds: %f", state.maxSeconds);
			ImGui::Text("Timer: %f", state.timer);
		}

		if (ImGui::CollapsingHeader("Motion")) {
			bool changed = false;
			for (auto&& motion: state.motions) {
				if (ImGui::TreeNode(motion.motion->name.c_str())) {
					ImGui::DragFloat("AnimationSpeed", &motion.animationSpeed, 0.01f, 0);
					if (motion.animationSpeed < 0) motion.animationSpeed = 0;
					if (ImGui::DragFloat("Threshold", &motion.threshold, 0.01f)) changed = true;

					ImGui::TreePop();
				}
			}
			if (changed) std::sort(
				state.motions.begin(), state.motions.end(), [](const Motion& a, const Motion& b) {
					return a.threshold < b.threshold;
				}
			);
		}
	}
	ImGui::End();
}

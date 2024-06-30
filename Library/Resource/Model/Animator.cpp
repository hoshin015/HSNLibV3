#include "Animator.h"

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

ModelResource::KeyFrame Animator::BlendKeyFrame(const ModelResource::KeyFrame* first,const  ModelResource::KeyFrame* second, const float lerpRate) const {
	const int               maxNodeCount = first->nodes.size();
	ModelResource::KeyFrame keyFrame;
	keyFrame.nodes.resize(maxNodeCount);

	for (int count = 0; count < maxNodeCount; count++) {
		ModelResource::KeyFrame::Node node;
		const XMVECTOR                scaleVec = XMVectorLerp(
			XMLoadFloat3(&first->nodes[count].scaling),
			XMLoadFloat3(&second->nodes[count].scaling),
			lerpRate
		);

		const XMVECTOR rotateVec = XMQuaternionSlerp(
			XMLoadFloat4(&first->nodes[count].rotation),
			XMLoadFloat4(&second->nodes[count].rotation),
			lerpRate
		);

		const XMVECTOR translationVec = XMVectorLerp(
			XMLoadFloat3(&first->nodes[count].translation),
			XMLoadFloat3(&second->nodes[count].translation),
			lerpRate
		);

		node.name = first->nodes[count].name;
		node.uniqueId = first->nodes[count].uniqueId;

		XMStoreFloat3(&node.scaling, scaleVec);
		XMStoreFloat4(&node.rotation, rotateVec);
		XMStoreFloat3(&node.translation, translationVec);

		const int64_t  parentID = _sceneView->nodes.at(_sceneView->GetIndex(node.uniqueId)).parentIndex;
		const XMMATRIX GT = parentID < 0 ?
			XMMatrixIdentity() :
			XMMatrixScalingFromVector(scaleVec) *
			XMMatrixRotationQuaternion(rotateVec) *
			XMMatrixTranslationFromVector(translationVec) * XMLoadFloat4x4(
				&keyFrame.nodes[parentID].globalTransform
			);

		XMStoreFloat4x4(&node.globalTransform, GT);
		keyFrame.nodes[count] = node;
	}

	return keyFrame;
}


ModelResource::KeyFrame Animator::MotionUpdate(Motion* motion, const float rate) const {
	const float seq = motion->motion->sequence.size() * fmodf(rate * motion->animationSpeed, 1);
	const int   index    = static_cast<int>(seq);
	const float lerpRate = seq - static_cast<float>(index);

	const ModelResource::KeyFrame* keyFrames[2] = {
		&motion->motion->sequence.at(index),
		&motion->motion->sequence.at(
			index >= motion->motion->sequence.size() - 1 ? 0 : index + 1
		)
	};
	if(motion->animationSpeed)motion->endMotion = rate * motion->animationSpeed >= 1.f;
	return BlendKeyFrame(keyFrames[0],keyFrames[1],lerpRate);
}

ModelResource::KeyFrame Animator::BlendUpdate(BlendTree* blend, const float time) {
	XMFLOAT2 parameter;
	parameter.x = std::get<float>(_parameters[blend->parameters[0]]);
	parameter.y = blend->parameters[1].empty()?0:std::get<float>(_parameters[blend->parameters[1]]);

	Motion* first = nullptr;
	Motion* second = nullptr;
	std::vector<Motion*> motions;

	// TODO::2ŽŸŒ³‚Å‚à‘I‘ð‚·‚é•û–@‚ðl‚¦‚é
	float maxLen = FLT_MIN;
	float minLen = FLT_MAX;
	for (auto&& motion : blend->motions) {
		float len = XMVectorGetX(XMVector2Length(XMLoadFloat2(&motion.threshold) - XMLoadFloat2(&parameter)));
		if(len>=1.f) continue;

		if (len > maxLen) {
			motions.insert(motions.begin(), &motion);
			maxLen = len;
		}
		else if (len < minLen) {
			motions.emplace_back(&motion);
			minLen = len;
		}
	}
	if (!second) second = first;
	if (!first) first = second;

	const float animationRate = time / blend->maxSeconds;
	const float lerpRate = second == first ?
		1 :
		(parameterX - first->threshold.x) / (second->threshold.x - first->threshold.x);

	const ModelResource::KeyFrame firstKeyFrame  = MotionUpdate(first,animationRate);
	const ModelResource::KeyFrame secondKeyFrame = MotionUpdate(second, animationRate);

	return BlendKeyFrame(&firstKeyFrame, &secondKeyFrame, lerpRate);
}

ModelResource::KeyFrame Animator::StateUpdate() {
	State& state = *_currentState;

	ModelResource::KeyFrame currentKeyFrame;
	switch (state.type) {
		case State::MOTION: {
			std::shared_ptr<Motion> motion = std::static_pointer_cast<Motion>(state.object);
			currentKeyFrame = MotionUpdate(motion.get(), _timer / motion->motion->secondsLength);
			break;
		}
		case State::BLEND_TREE: {
			currentKeyFrame = BlendUpdate(std::static_pointer_cast<BlendTree>(state.object).get(), _timer);
			break;
		}
	}

	for (auto&& function: state.transitions) {
		if(State* state = function(*this)) {
			_currentState = state;
			_timer = 0;
			break;
		}
	}

	if (_nextState) {
		// ModelResource::KeyFrame nextKeyFrame;
		// switch (_nextState->type) {
		// case State::MOTION:
		// {
		// 	std::shared_ptr<Motion> motion = std::static_pointer_cast<Motion>(_nextState->object);
		// 	nextKeyFrame = MotionUpdate(motion.get(), fmodf(_nextState->timer / motion->motion->secondsLength, 1));
		// 	break;
		// }
		// case State::BLEND_TREE:
		// {
		// 	nextKeyFrame = BlendUpdate(std::static_pointer_cast<BlendTree>(_nextState->object).get(), _nextState->timer);
		// 	break;
		// }
		// }
		//
		// const int               maxNodeCount = currentkeyFrame.nodes.size();
		// ModelResource::KeyFrame keyFrame;
		// keyFrame.nodes.resize(maxNodeCount);
	}

	return currentKeyFrame;
}

ModelResource::KeyFrame Animator::PlayAnimation(float elapsedTime) {
	_timer += elapsedTime;
	return StateUpdate();
	// BlendTree& state = _state[name];
	// state.blendRate  = rate;
	//
	// const Motion* first  = nullptr;
	// const Motion* second = nullptr;
	//
	// for (auto&& motion: state.motions) {
	// 	if (state.blendRate >= motion.threshold) first = &motion;
	// 	if (!second && state.blendRate < motion.threshold) second = &motion;
	// }
	// if (!second) second = first;
	// if (!first) first = second;
	//
	// const float animationRate = state.timer / state.maxSeconds;
	// const float leapRate      = second == first ?
	// 	                            1 :
	// 	                            (state.blendRate - first->threshold) / (second->threshold - first->threshold);
	//
	// float seq = first->motion->sequence.size() * std::fmodf(animationRate * first->animationSpeed, 1.f);
	// //float seq = first->motion->sequence.size() * animationRate;
	// int   firstKeyFrameIndex = static_cast<int>(seq);
	// float firstLeapRate      = seq - static_cast<float>(firstKeyFrameIndex);
	//
	// const ModelResource::KeyFrame* firstKeyFrames[2] = {
	// 	&first->motion->sequence.at(firstKeyFrameIndex),
	// 	&first->motion->sequence.at(
	// 		firstKeyFrameIndex >= first->motion->sequence.size() - 1 ? 0 : firstKeyFrameIndex + 1
	// 	)
	// };
	//
	// seq = second->motion->sequence.size() * std::fmodf(animationRate * second->animationSpeed, 1.f);
	// int   secondKeyFrameIndex = static_cast<int>(seq);
	// float secondLeapRate = seq - static_cast<float>(secondKeyFrameIndex);
	//
	// const ModelResource::KeyFrame* secondKeyFrames[2] = {
	// 	&second->motion->sequence.at(secondKeyFrameIndex),
	// 	&second->motion->sequence.at(
	// 		secondKeyFrameIndex >= second->motion->sequence.size() - 1 ? 0 : secondKeyFrameIndex + 1
	// 	)
	// };
	//
	// ModelResource::KeyFrame resultKeyFrame;
	// resultKeyFrame.nodes.resize(firstKeyFrames[0]->nodes.size());
	// for (int count = 0; count < resultKeyFrame.nodes.size(); ++count) {
	// 	ModelResource::KeyFrame::Node node;
	// 	XMVECTOR                      scaleVec = XMVectorLerp(
	// 		XMVectorLerp(
	// 			XMLoadFloat3(&firstKeyFrames[0]->nodes[count].scaling),
	// 			XMLoadFloat3(&firstKeyFrames[1]->nodes[count].scaling),
	// 			firstLeapRate
	// 		),
	// 		XMVectorLerp(
	// 			XMLoadFloat3(&secondKeyFrames[0]->nodes[count].scaling),
	// 			XMLoadFloat3(&secondKeyFrames[1]->nodes[count].scaling),
	// 			secondLeapRate
	// 		),
	// 		leapRate
	// 	);
	// 	XMVECTOR rotateVec = XMQuaternionSlerp(
	// 		XMQuaternionSlerp(
	// 			XMLoadFloat4(&firstKeyFrames[0]->nodes[count].rotation),
	// 			XMLoadFloat4(&firstKeyFrames[1]->nodes[count].rotation),
	// 			firstLeapRate
	// 		),
	// 		XMQuaternionSlerp(
	// 			XMLoadFloat4(&secondKeyFrames[0]->nodes[count].rotation),
	// 			XMLoadFloat4(&secondKeyFrames[1]->nodes[count].rotation),
	// 			secondLeapRate
	// 		),
	// 		leapRate
	// 	);
	// 	XMVECTOR translationVec = XMVectorLerp(
	// 		XMVectorLerp(
	// 			XMLoadFloat3(&firstKeyFrames[0]->nodes[count].translation),
	// 			XMLoadFloat3(&firstKeyFrames[1]->nodes[count].translation),
	// 			firstLeapRate
	// 		),
	// 		XMVectorLerp(
	// 			XMLoadFloat3(&secondKeyFrames[0]->nodes[count].translation),
	// 			XMLoadFloat3(&secondKeyFrames[1]->nodes[count].translation),
	// 			secondLeapRate
	// 		),
	// 		leapRate
	// 	);
	//
	// 	node.name     = firstKeyFrames[0]->nodes[count].name;
	// 	node.uniqueId = firstKeyFrames[0]->nodes[count].uniqueId;
	//
	// 	XMStoreFloat3(&node.scaling, scaleVec);
	// 	XMStoreFloat4(&node.rotation, rotateVec);
	// 	XMStoreFloat3(&node.translation, translationVec);
	//
	// 	int64_t  parentID = _sceneView->nodes.at(_sceneView->GetIndex(node.uniqueId)).parentIndex;
	// 	XMMATRIX GT       = parentID < 0 ?
	// 		                    XMMatrixIdentity() :
	// 		                    XMMatrixScalingFromVector(scaleVec) *
	// 		                    XMMatrixRotationQuaternion(rotateVec) *
	// 		                    XMMatrixTranslationFromVector(translationVec) * XMLoadFloat4x4(
	// 			                    &resultKeyFrame.nodes[parentID].globalTransform
	// 		                    );
	//
	// 	XMStoreFloat4x4(&node.globalTransform, GT);
	// 	resultKeyFrame.nodes[count] = node;
	// }
	//
	// state.timer += elapsedTime;
	// //if (state.timer >= state.maxSeconds) state.timer = 0;
	//
	// return resultKeyFrame;
	// return ModelResource::KeyFrame();
}

void Animator::AnimationEditor() {
	if (ImGui::Begin("AnimationEditor")) {
		if (ImGui::CollapsingHeader("Parameters")) {
			for (auto& [name, var]: _parameters) {
				if (int* v = std::get_if<int>(&var)) {
					ImGui::DragInt(name.c_str(), v);
				}
				if (float* v = std::get_if<float>(&var)) {
					ImGui::DragFloat(name.c_str(), v,0.01f);
				}
				if (bool* v = std::get_if<bool> (&var)) {
					ImGui::Checkbox(name.c_str(), v);
				}
			}
		}
	
		if (ImGui::CollapsingHeader("Motion")) {
			for (auto& [name, state]: _states) {
				if (ImGui::TreeNode(name.c_str())) {
					if(state.type==State::BLEND_TREE) {
						BlendTree* blendTree = std::static_pointer_cast<BlendTree>(state.object).get();


						for (auto&& motion: blendTree->motions) {
							if (ImGui::TreeNode(motion.motion->name.c_str())) {
								ImGui::DragFloat("animationSpeed", &motion.animationSpeed);
								ImGui::DragFloat2("threshold", &motion.threshold.x,0.01f);

								ImGui::TreePop();
							}
						}
					}

					if(state.type == State::MOTION) {
						Motion* motion = std::static_pointer_cast<Motion>(state.object).get();

						if (ImGui::TreeNode(motion->motion->name.c_str())) {
							ImGui::DragFloat("animationSpeed", &motion->animationSpeed,0.01f);
							if (motion->animationSpeed < 0)motion->animationSpeed = 0;

							ImGui::TreePop();
						}
					}

					ImGui::TreePop();
				}
			}
		}
	}
	ImGui::End();
}

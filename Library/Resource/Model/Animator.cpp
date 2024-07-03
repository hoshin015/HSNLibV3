#include "Animator.h"

#include "../../../External/ImGui/imgui_internal.h"
using namespace DirectX;

inline XMFLOAT3 operator-(XMFLOAT3& a, XMFLOAT3& b) {
	XMFLOAT3 result;
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	result.z = a.z - b.z;
	return result;
}

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

ModelResource::KeyFrame Animator::BlendKeyFrame(
	const ModelResource::KeyFrame* first, const ModelResource::KeyFrame* second, const float lerpRate
) const {
	const size_t            maxNodeCount = first->nodes.size();
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

		node.name     = first->nodes[count].name;
		node.uniqueId = first->nodes[count].uniqueId;

		XMStoreFloat3(&node.scaling, scaleVec);
		XMStoreFloat4(&node.rotation, rotateVec);
		XMStoreFloat3(&node.translation, translationVec);

		const int64_t  parentID = _sceneView->nodes.at(_sceneView->GetIndex(node.uniqueId)).parentIndex;
		const XMMATRIX GT       = parentID < 0 ?
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
	const float seq      = motion->motion->sequence.size() * fmodf(rate * motion->animationSpeed, 1);
	const int   index    = static_cast<int>(seq);
	const float lerpRate = seq - static_cast<float>(index);

	const ModelResource::KeyFrame* keyFrames[2] = {
		&motion->motion->sequence.at(index),
		&motion->motion->sequence.at(
			index >= motion->motion->sequence.size() - 1 ? 0 : index + 1
		)
	};

	// TODO::�Ō�̃t���[����ʉ߂������̔��������������Ɖ��Ƃ�����
	// ���̂܂܂��ƃ^�C�~���O�ɂ���Ă͍Ō�̃t���[���S���Ă����m�ł��Ȃ�
	motion->endMotion = fmodf(rate * motion->animationSpeed,1) >= .9f;
	return BlendKeyFrame(keyFrames[0], keyFrames[1], lerpRate);
}

ModelResource::KeyFrame Animator::BlendUpdate(BlendTree* blend, const float time) {
	XMFLOAT2 parameter;
	parameter.x = std::get<float>(_parameters[blend->parameters[0]]);
	parameter.y = blend->parameters[1].empty() ? 0 : std::get<float>(_parameters[blend->parameters[1]]);

	std::vector<Motion*> motions;
	std::vector<float>   weights;

	// TODO::2�����ł��I��������@���l����
	float totalWeight = 0;
	for (auto&& motion: blend->motions) {
		const float len = XMVectorGetX(XMVector2Length(XMLoadFloat2(&motion.threshold) - XMLoadFloat2(&parameter)));

		if (float weight = fmaxf(1 - len, 0.f); weight > 0) {
			motions.emplace_back(&motion);
			weights.emplace_back(weight);
			totalWeight += weight;
		}
	}
	if (totalWeight > 0) for (float& weight: weights) { weight /= totalWeight; }

	const float                          animationRate = time / blend->maxSeconds;
	std::vector<ModelResource::KeyFrame> keyFrames;

	for (auto&& motion: motions) { keyFrames.emplace_back(MotionUpdate(motion, animationRate)); }

	// �d�������߂Ă���������`��Ԃ���
	ModelResource::KeyFrame& result = keyFrames[0];

	const size_t weightSize = weights.size();
	for (int i = 1; i < weightSize; i++) { result = BlendKeyFrame(&result, &keyFrames[i], weights[i]); }

	return result;
}

ModelResource::KeyFrame Animator::StateUpdate(float elapsedTime) {
	State& state = *_currentState;

	ModelResource::KeyFrame currentKeyFrame;
	ModelResource::KeyFrame nextKeyFrame;
	switch (state.type) {
		case State::MOTION: {
			const std::shared_ptr<Motion> motion = std::static_pointer_cast<Motion>(state.object);

			currentKeyFrame = MotionUpdate(motion.get(), _timer / motion->motion->secondsLength);
			nextKeyFrame    = MotionUpdate(motion.get(), (_timer + elapsedTime) / motion->motion->secondsLength);
			if (motion->endMotion)
				nextKeyFrame = ModelResource::KeyFrame();
			break;
		}
		case State::BLEND_TREE: {
			const std::shared_ptr<BlendTree> blendTree = std::static_pointer_cast<BlendTree>(state.object);

			currentKeyFrame = BlendUpdate(blendTree.get(), _timer);
			nextKeyFrame = BlendUpdate(blendTree.get(), _timer + elapsedTime);

			i
			break;
		}
	}

	for (auto&&    function: state.transitions) {
		if (State* state = function(*this)) {
			_currentState = state;
			_timer        = 0;
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

	{
		const auto currentRootNode = std::find_if(
			currentKeyFrame.nodes.begin(), currentKeyFrame.nodes.end(), [&](const ModelResource::KeyFrame::Node& a) {
				return a.name == _rootMotionName;
			}
		);
		const auto nextRootNode = std::find_if(
			nextKeyFrame.nodes.begin(), nextKeyFrame.nodes.end(), [&](const ModelResource::KeyFrame::Node& a) {
				return a.name == _rootMotionName;
			}
		);

		_velocity = nextRootNode == nextKeyFrame.nodes.end() ?
						XMFLOAT3() :
			            nextRootNode->translation - currentRootNode->translation;

		currentRootNode->translation = {};
		for (auto&& node: currentKeyFrame.nodes) {
			const int64_t  parentID = _sceneView->nodes.at(_sceneView->GetIndex(node.uniqueId)).parentIndex;
			const XMMATRIX GT       = parentID < 0 ?
				                          XMMatrixIdentity() :
				                          XMMatrixScaling(node.scaling.x, node.scaling.y, node.scaling.z) *
				                          XMMatrixRotationQuaternion(XMLoadFloat4(&node.rotation)) *
				                          XMMatrixTranslation(
					                          node.translation.x, node.translation.y, node.translation.z
				                          ) * XMLoadFloat4x4(
					                          &currentKeyFrame.nodes[parentID].globalTransform
				                          );

			XMStoreFloat4x4(&node.globalTransform, GT);
		}
	}

	//_nextKeyFrame = currentKeyFrame;
	return currentKeyFrame;
}

ModelResource::KeyFrame Animator::PlayAnimation(float elapsedTime) {
	_timer += elapsedTime;
	return StateUpdate(elapsedTime);
}

void Animator::AnimationEditor() {
	if (ImGui::Begin("AnimationEditor")) {
		if (ImGui::CollapsingHeader("Parameters")) {
			for (auto&     [name, var]: _parameters) {
				if (int*   v = std::get_if<int>(&var)) { ImGui::DragInt(name.c_str(), v); }
				if (float* v = std::get_if<float>(&var)) { ImGui::DragFloat(name.c_str(), v, 0.01f); }
				if (bool*  v = std::get_if<bool>(&var)) { ImGui::Checkbox(name.c_str(), v); }
			}
		}

		if (ImGui::CollapsingHeader("Motion")) {
			for (auto& [name, state]: _states) {
				if (ImGui::TreeNode(name.c_str())) {
					if (state.type == State::BLEND_TREE) {
						BlendTree* blendTree = std::static_pointer_cast<BlendTree>(state.object).get();

						for (auto&& motion: blendTree->motions) {
							if (ImGui::TreeNode(motion.motion->name.c_str())) {
								ImGui::DragFloat("animationSpeed", &motion.animationSpeed);
								ImGui::DragFloat2("threshold", &motion.threshold.x, 0.01f);

								ImGui::TreePop();
							}
						}
					}

					if (state.type == State::MOTION) {
						Motion* motion = std::static_pointer_cast<Motion>(state.object).get();

						if (ImGui::TreeNode(motion->motion->name.c_str())) {
							ImGui::DragFloat("animationSpeed", &motion->animationSpeed, 0.01f);
							if (motion->animationSpeed < 0) motion->animationSpeed = 0;

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

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

		const int64_t  index = _sceneView->GetIndex(node.uniqueId);
		const int64_t  parentID = index >= 0 ? _sceneView->nodes.at(index).parentIndex : 0;
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
	motion->endMotion = fmodf(rate * motion->animationSpeed, 1) >= .9f;
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
	if (motions.empty()) motions.emplace_back(&blend->motions[0]);

	const float                          animationRate = time / blend->maxSeconds;
	std::vector<ModelResource::KeyFrame> keyFrames;

	for (auto&& motion: motions) { keyFrames.emplace_back(MotionUpdate(motion, animationRate)); }

	// �d�������߂Ă���������`��Ԃ���
	ModelResource::KeyFrame& result = keyFrames[0];

	const size_t weightSize = weights.size();
	for (int i = 1; i < weightSize; i++) { result = BlendKeyFrame(&result, &keyFrames[i], weights[i]); }

	blend->endMotion = animationRate >= .9f;
	return result;
}

ModelResource::KeyFrame Animator::StateUpdate(State* state, float elapsedTime) {
	state->timer += elapsedTime;

	bool                    endMotion = false;
	ModelResource::KeyFrame currentKeyFrame;
	ModelResource::KeyFrame nextKeyFrame;
	switch (state->type) {
		case State::MOTION: {
			const std::shared_ptr<Motion> motion = std::static_pointer_cast<Motion>(state->object);

			currentKeyFrame = MotionUpdate(motion.get(), state->timer / motion->motion->secondsLength);
			if (_rootMotionEnabled) {
				nextKeyFrame = MotionUpdate(motion.get(), (state->timer + elapsedTime) / motion->motion->secondsLength);
				if (endMotion = motion->endMotion; endMotion) nextKeyFrame.nodes.clear();
			}
			break;
		}
		case State::BLEND_TREE: {
			const std::shared_ptr<BlendTree> blendTree = std::static_pointer_cast<BlendTree>(state->object);

			currentKeyFrame = BlendUpdate(blendTree.get(), state->timer);
			if (_rootMotionEnabled) {
				nextKeyFrame = BlendUpdate(blendTree.get(), state->timer + elapsedTime);
				if (endMotion = blendTree->endMotion; endMotion) nextKeyFrame.nodes.clear();
			}

			break;
		}
	}

	if (!_nextState) {
		// �֐����o�^����Ă��邩
		// std::function�ɂ�operator bool�Ŋ֐����o�^����Ă��邩���擾�ł���
		if (state->transitions) {
			if (State* stateFromFunc = state->transitions(*this)) {
				_nextState = stateFromFunc;
				//state->timer = 0;
			}
		}
	}

	if (_rootMotionEnabled) {
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
	ModelResource::KeyFrame current = StateUpdate(_currentState, elapsedTime);
	if (_nextState) {
		_timer += elapsedTime;
		ModelResource::KeyFrame next = StateUpdate(_nextState, elapsedTime);

		float rate = _nextState->transitionTime > 0 ? _timer / _nextState->transitionTime : 1;
		current    = BlendKeyFrame(&current, &next, rate);
		if (rate >= 1.f) {
			_timer               = 0;
			_currentState->timer = 0;
			_currentState        = _nextState;
			_nextState           = nullptr;
		}
	}

	return current;
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
					if(ImGui::TreeNode(u8"�X�e�[�^�X")) {
						if (state.type == State::MOTION) ImGui::Text(u8"�I�u�W�F�N�g�^�C�v: MOTION");
						if (state.type == State::BLEND_TREE) ImGui::Text(u8"�I�u�W�F�N�g�^�C�v: BLEND_TREE");
						ImGui::Text(u8"����:%f", state.speed);
						ImGui::Text(u8"����:%f", state.timer);
						ImGui::DragFloat(u8"�J�ڎ���", &state.transitionTime, 0.001f);

						ImGui::TreePop();
					}

					if (state.type == State::BLEND_TREE) {
						BlendTree* blendTree = std::static_pointer_cast<BlendTree>(state.object).get();

						for (auto&& motion: blendTree->motions) {
							if (ImGui::TreeNode(motion.motion->name.c_str())) {
								ImGui::DragFloat(u8"�A�j���[�V�����̑���", &motion.animationSpeed);
								if (motion.animationSpeed < 0) motion.animationSpeed = 0;

								ImGui::DragFloat2(u8"�������l", &motion.threshold.x, 0.001f);

								ImGui::TreePop();
							}
						}
					}

					if (state.type == State::MOTION) {
						Motion* motion = std::static_pointer_cast<Motion>(state.object).get();

						if (ImGui::TreeNode(motion->motion->name.c_str())) {
							ImGui::DragFloat(u8"�A�j���[�V�����̑���", &motion->animationSpeed, 0.001f);
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

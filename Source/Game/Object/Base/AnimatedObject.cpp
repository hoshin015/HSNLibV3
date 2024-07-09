// --- Library ---
#include "../../Library/Timer.h"
// --- Game ---
#include "AnimatedObject.h"
#include "../../../../Library/3D/DebugPrimitive.h"

AnimatedObject::AnimatedObject(const char* filePath)
{
	model = std::make_unique<AnimatedModel>(filePath);
}

void AnimatedObject::UpdateTransform()
{
	// �X�P�[���ύX
	DirectX::XMMATRIX MS = DirectX::XMMatrixScaling(model->GetModelResource()->GetScale(),
	                                                model->GetModelResource()->GetScale(),
	                                                model->GetModelResource()->GetScale());

	// ���ϊ��s��
	DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&model->GetModelResource()->GetCoordinateSystemTransform());

	// �X�P�[���s����쐬
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(GetScaleX(), GetScaleY(), GetScaleZ());
	// ��]�s����쐬
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(GetAngleX()),
	                                                            DirectX::XMConvertToRadians(GetAngleY()),
	                                                            DirectX::XMConvertToRadians(GetAngleZ()));
	// �ʒu�s����쐬
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(GetPosX(), GetPosY(), GetPosZ());

	// �S�̍s���g�ݍ��킹�A���[���h�s����쐬
	DirectX::XMMATRIX W = MS * S * R * T * C;
	DirectX::XMStoreFloat4x4(&transform, W);
}

void AnimatedObject::DrawDebugPrimitive()
{
	if (!model->GetModelResource()->GetAnimationClips().empty())
	{
		for (auto& sphere : model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex).animSphereCollisions)
		{
			if (sphere.startFrame <= currentKeyFrame && sphere.endFrame >= currentKeyFrame)
			{
				DirectX::XMFLOAT3 pos = GetBonePosition(sphere.bindBoneName);
				DebugPrimitive::Instance().AddSphere(pos, sphere.radius, sphere.color);
			}
		}
	}

	for (auto& bone : model->GetModelResource()->GetSkeletonSphereCollisions())
	{
		DirectX::XMFLOAT3 pos;
		if (bone.name != "")
		{
			pos = GetBonePosition(bone.name);
		}
		else
		{
			DirectX::XMVECTOR BONE_POS = DirectX::XMLoadFloat3(&bone.position);
			DirectX::XMVECTOR POS = DirectX::XMLoadFloat3(&position);
			DirectX::XMStoreFloat3(&pos, DirectX::XMVectorAdd(BONE_POS, POS));
		}

		DirectX::XMFLOAT4 color = bone.color[static_cast<int>(bone.skeletonType)];
		DebugPrimitive::Instance().AddSphere(pos, bone.radius, color);
	}
}

void AnimatedObject::PlayAnimation(int index, bool loop)
{
	if (GetCurrentAnimationIndex() == index && animationLoopFlag == loop) return;

	currentAnimationIndex   = index;
	currentAnimationSeconds = 0.0f;

	animationLoopFlag = loop;
	animationEndFlag  = false;

	ClearAnimSphereCollisionDamagedFlag();
	ClearSeFlag();
}

// �A�j���[�V�����X�V
void AnimatedObject::UpdateAnimation()
{
	// �Đ����łȂ��Ȃ�X�V���Ȃ�
	if (!isPlayAnimation) return;

	// �A�j���[�V�����N���b�v�������Ȃ��ꍇ�������Ȃ�
	if (model->GetModelResource()->GetAnimationClips().empty()) return;

	// �ŏI�t���[���Ȃ�X�V���Ȃ�
	if (animationEndFlag) return;

	// �A�j���[�V�����f�[�^�擾
	ModelResource::Animation& animation = model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex);

	currentKeyFrame = static_cast<int>(currentAnimationSeconds * animation.samplingRate);

	// �o�ߎ���
	currentAnimationSeconds += Timer::Instance().DeltaTime() * animationTimeScale;
	// ���݂̃t���[�����ő�t���[���𒴂��Ă�����
	if (currentAnimationSeconds >= animation.secondsLength)
	{
		if (animationLoopFlag)
		{
			// �擪�b����
			currentKeyFrame = 0;
			currentAnimationSeconds -= animation.secondsLength;

			ClearAnimSphereCollisionDamagedFlag();
			ClearSeFlag();
		}
		else
		{
			// �ŏI�b���Œ�~
			currentKeyFrame         = animation.sequence.size() - 1;
			currentAnimationSeconds = animation.secondsLength;
			animationEndFlag        = true;
		}
	}

	// keyFrame�ݒ�
	keyFrame = animation.sequence.at(currentKeyFrame);


	// �_�u���A�j���[�V�������L���Ȃ�㔼�g�̂ݕʃL�[�ɂ���
	if (isDoubleAnimations)
	{
		// �_�u���A�j���[�V�����f�[�^�擾
		ModelResource::Animation& doubleAnimation = model->GetModelResource()->GetAnimationClips().at(
			doubleCurrentAnimationIndex);

		doubleCurrentKeyFrame = static_cast<int>(doubleCurrentAnimationSeconds * animation.samplingRate);

		// �o�ߎ���
		doubleCurrentAnimationSeconds += Timer::Instance().DeltaTime();

		// ���݂̃t���[�����ő�t���[���𒴂��Ă�����
		if (doubleCurrentAnimationSeconds >= doubleAnimation.secondsLength)
		{
			// �_�u���A�j���[�V������OFF
			isDoubleAnimations            = false;
			doubleCurrentKeyFrame         = 0;
			doubleCurrentAnimationSeconds = 0.0f;

			return;
		}

		doubleKeyFrame = doubleAnimation.sequence.at(doubleCurrentKeyFrame);

		// �㔼�g�A�j���[�V�������g�p����m�[�h�� index ���i�[����x�N�^�[ (�e�s�񂪏㔼�g�A�j���[�V����)
		std::vector<uint32_t> doubleAnimationNodeIndex;

		//node �̐������J��Ԃ�
		size_t nodeCount = doubleKeyFrame.nodes.size();
		for (size_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
		{
			// ���݂� index �� node ���擾
			ModelResource::KeyFrame::Node& keyData = doubleKeyFrame.nodes.at(nodeIndex);

			// ���ꂼ��̒l�ɑΉ�����s��̍쐬
			DirectX::XMMATRIX S = DirectX::XMMatrixScaling(keyData.scaling.x, keyData.scaling.y, keyData.scaling.z);
			DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(XMLoadFloat4(&keyData.rotation));
			DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(keyData.translation.x, keyData.translation.y,
			                                                   keyData.translation.z);

			// �e�s�񂪂���ΐe�̍s���������
			uint32_t uniqueIndex = model->GetModelResource()->GetSceneView().GetIndex(keyData.uniqueId);
			// ���̃m�[�h��uniqueId �擾
			int64_t parentIndex = model->GetModelResource()->GetSceneView().nodes.at(uniqueIndex).parentIndex;
			// ����uniqueId �� parent �̐e�� index ���擾

			// ��Z���ׂ��e�s�񂪂ǂ���̃A�j���[�V�������𒲂ׂ�
			bool doubleAnimationParent = false;
			for (auto checkIndex : doubleAnimationNodeIndex)
			{
				if (parentIndex == checkIndex)
				{
					doubleAnimationParent = true;                    // �e�̓_�u���A�j���[�V�����̍s����g�p
					doubleAnimationNodeIndex.push_back(uniqueIndex); // ���̃m�[�h�̎q�̂��߂ɒǉ�
					break;
				}
			}

			DirectX::XMMATRIX P;

			// �����̖��O�͕ς����悤�ɂ����ق����悳����
			if (keyData.name == "mixamorig:Spine")
			{
				doubleAnimationNodeIndex.push_back(uniqueIndex);
				doubleAnimationParent = true; // �e�̓_�u���A�j���[�V�����̍s����g�p(�������͏ꍇ�ɂ���Ă͂�߂��ق�����������)
			}

			if (doubleAnimationParent)
			{
				P = parentIndex < 0
					    ? DirectX::XMMatrixIdentity()
					    : DirectX::XMLoadFloat4x4(&keyFrame.nodes.at(parentIndex).globalTransform);
				XMStoreFloat4x4(&keyData.globalTransform, S * R * T * P);
				keyFrame.nodes.at(nodeIndex).globalTransform = keyData.globalTransform;
			}
		}
	}


	// �A�j���[�V�����u�����h(���̃A�j���[�V�����ւ̎��R�ȑJ��)
	// ���݂̃A�j���[�V���������[�v�A�j���[�V�����Ȃ獡����J��
	// ���݂̃A�j���[�V�������񃋁[�v�A�j���[�V�����Ȃ�w�肵�����Ԃ���w�肵�����Ԃ��|���đJ��
	if (blendAnimationIndex != -1)
	{
		float blendRate = 1.0f;
		if (animationBlendTransitionDurations > animationBlendTimer)
		{
			animationBlendTimer += Timer::Instance().DeltaTime();
			if (animationBlendTimer > animationBlendTransitionDurations)
			{
				animationBlendTimer = animationBlendTransitionDurations;
			}
			blendRate = animationBlendTimer / animationBlendTransitionDurations;
			blendRate *= blendRate;

			ModelResource::Animation& blendAnimation = model->GetModelResource()->GetAnimationClips().at(
				blendAnimationIndex);
			int blendAnimationCurrentKeyFrame = static_cast<int>(animationBlendTimer * blendAnimation.samplingRate);

			ModelResource::KeyFrame key = keyFrame;

			const ModelResource::KeyFrame* keyFrames[2] =
			{
				&blendAnimation.sequence.at(blendAnimationCurrentKeyFrame), // ���̃A�j���[�V�����̃L�[�t���[��
				&keyFrame,                                                  // ���ݍĐ����̃L�[�t���[��
			};
			BlendAnimation(keyFrames, blendRate, keyFrame);


			//node �̐������J��Ԃ�
			size_t nodeCount = keyFrame.nodes.size();
			for (size_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
			{
				// ���݂� index �� node ���擾
				ModelResource::KeyFrame::Node& keyData = keyFrame.nodes.at(nodeIndex);

				// ���ꂼ��̒l�ɑΉ�����s��̍쐬
				DirectX::XMMATRIX S = DirectX::XMMatrixScaling(keyData.scaling.x, keyData.scaling.y, keyData.scaling.z);
				DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(XMLoadFloat4(&keyData.rotation));
				DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(keyData.translation.x, keyData.translation.y,
				                                                   keyData.translation.z);

				// �e�s�񂪂���ΐe�̍s���������
				uint32_t uniqueIndex = model->GetModelResource()->GetSceneView().GetIndex(keyData.uniqueId);
				// ���̃m�[�h��uniqueId �擾
				int64_t parentIndex = model->GetModelResource()->GetSceneView().nodes.at(uniqueIndex).parentIndex;
				// ����uniqueId �� parent �̐e�� index ���擾

				DirectX::XMMATRIX P = parentIndex < 0
					                      ? DirectX::XMMatrixIdentity()
					                      : DirectX::XMLoadFloat4x4(&keyFrame.nodes.at(parentIndex).globalTransform);
				XMStoreFloat4x4(&keyData.globalTransform, S * R * T * P);
			}
		}
		else
		{
			blendAnimationIndex = -1;
		}
	}
}

void AnimatedObject::UpdateBlendAnimation()
{
	// �Đ����łȂ��Ȃ�X�V���Ȃ�
	if (!isPlayAnimation) return;

	// �A�j���[�V�����N���b�v�������Ȃ��ꍇ�������Ȃ�
	if (model->GetModelResource()->GetAnimationClips().empty()) return;

	// �ŏI�t���[���Ȃ�X�V���Ȃ�
	if (animationEndFlag) return;

	// �A�j���[�V�����f�[�^�擾
	ModelResource::Animation& animation = model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex);

	currentKeyFrame = static_cast<int>(currentAnimationSeconds * animation.samplingRate);

	// �o�ߎ���
	currentAnimationSeconds += Timer::Instance().DeltaTime() * animationTimeScale;
	// ���݂̃t���[�����ő�t���[���𒴂��Ă�����
	if (currentAnimationSeconds >= animation.secondsLength)
	{
		if (animationLoopFlag)
		{
			// �擪�b����
			currentKeyFrame = 0;
			//currentAnimationSeconds -= animation.secondsLength;
			currentAnimationSeconds = 0;
		}
		else
		{
			// �ŏI�b���Œ�~
			currentKeyFrame = animation.sequence.size() - 1;
			currentAnimationSeconds = animation.secondsLength;
			animationEndFlag = true;
		}
	}
}

// �_���[�W�t���O�̃N���A
void AnimatedObject::ClearAnimSphereCollisionDamagedFlag()
{
	for (auto& playerAnimSphereCollision : model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex).animSphereCollisions)
	{
		playerAnimSphereCollision.isDamaged = false;
	}
}

// ���ʉ��X�V
void AnimatedObject::UpdateSe()
{
	int seCount = model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex).animSes.size();

	for (int i = 0; i < seCount; i++)
	{
		// ���ɍĐ�����Ă���Ȃ�Đ����Ȃ�
		if (model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex).animSes.at(i).isPlay) continue;

		int startFrame = model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex).animSes.at(i).startFrame;
		int endFrame = model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex).animSes.at(i).endFrame;

		//if (currentFrameInt < startFrame || currentFrameInt > endFrame) continue;
		if (currentKeyFrame != startFrame) continue;

		model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex).animSes.at(i).isPlay = true;

		MUSIC_LABEL type = model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex).animSes.at(i).musicType;

		AudioManager::Instance().PlayMusic(static_cast<int>(type));
	}
}

// ���ʉ��t���O�̃N���A
void AnimatedObject::ClearSeFlag()
{
	int seCount = model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex).animSes.size();

	for (int i = 0; i < seCount; i++)
	{
		model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex).animSes.at(i).isPlay = false;
	}
}


DirectX::XMFLOAT3 AnimatedObject::GetBonePosition(std::string boneName)
{
	for(auto& mesh : model->GetModelResource()->GetMeshes())
	{
		for(auto& bone : mesh.skeleton.bones)
		{
			if(bone.name == boneName)
			{
				ModelResource::Animation& animationClip = model->GetModelResource()->GetAnimationClips().
					at(currentAnimationIndex);

				ModelResource::KeyFrame& keyframe = animationClip.sequence.at(currentKeyFrame);

				int nodeCount = keyframe.nodes.size();
				for (int nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
				{
					ModelResource::KeyFrame::Node& node = keyframe.nodes.at(nodeIndex);

					if (bone.name == node.name)
					{
						DirectX::XMMATRIX T = DirectX::XMLoadFloat4x4(&transform);
						DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&node.globalTransform);

						DirectX::XMMATRIX W = M * T;
						DirectX::XMFLOAT4X4 w;
						DirectX::XMStoreFloat4x4(&w, W);

						return { w._41, w._42, w._43 };
					}
				}
			}
		}
	}

	// ��v����{�[�����Ȃ��ꍇ 0,0,0 ��Ԃ�
	return { 0,0,0 };
}

// �A�j���[�V�����u�����h
void AnimatedObject::BlendAnimation(const ModelResource::KeyFrame* keyFrames[2], float factor,
                                    ModelResource::KeyFrame&       keyFrame)
{
	// �o�͑Ώۂ� keyFrame �� node ���������� keyFrame �� node �̃T�C�Y�ɂ���
	size_t nodeCount = keyFrames[0]->nodes.size();
	keyFrame.nodes.resize(nodeCount);
	// �S�Ă� node ����������
	for (size_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
	{
		// scale �̍���
		DirectX::XMVECTOR S[2] =
		{
			XMLoadFloat3(&keyFrames[0]->nodes.at(nodeIndex).scaling),
			XMLoadFloat3(&keyFrames[1]->nodes.at(nodeIndex).scaling)
		};
		XMStoreFloat3(&keyFrame.nodes.at(nodeIndex).scaling, DirectX::XMVectorLerp(S[0], S[1], factor));

		// rotation �̍���
		DirectX::XMVECTOR R[2] =
		{
			XMLoadFloat4(&keyFrames[0]->nodes.at(nodeIndex).rotation),
			XMLoadFloat4(&keyFrames[1]->nodes.at(nodeIndex).rotation)
		};
		XMStoreFloat4(&keyFrame.nodes.at(nodeIndex).rotation, DirectX::XMQuaternionSlerp(R[0], R[1], factor));

		// translation �̍���
		DirectX::XMVECTOR T[2] =
		{
			XMLoadFloat3(&keyFrames[0]->nodes.at(nodeIndex).translation),
			XMLoadFloat3(&keyFrames[1]->nodes.at(nodeIndex).translation)
		};
		XMStoreFloat3(&keyFrame.nodes.at(nodeIndex).translation, DirectX::XMVectorLerp(T[0], T[1], factor));
	}
}

// --- Library ---
#include "../../Library/Timer.h"
// --- Game ---
#include "AnimatedObject.h"

AnimatedObject::AnimatedObject(const char* filePath)
{
	model = std::make_unique<AnimatedModel>(filePath);
}

void AnimatedObject::UpdateTransform()
{
	// �X�P�[���ύX
	DirectX::XMMATRIX MS = DirectX::XMMatrixScaling(model->GetModelResource()->GetScale(), model->GetModelResource()->GetScale(), model->GetModelResource()->GetScale());

	// ���ϊ��s��
	DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&model->GetModelResource()->GetCoordinateSystemTransform());

	// �X�P�[���s����쐬
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(GetScaleX(), GetScaleY(), GetScaleZ());
	// ��]�s����쐬
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(GetAngleX()), DirectX::XMConvertToRadians(GetAngleY()), DirectX::XMConvertToRadians(GetAngleZ()));
	// �ʒu�s����쐬
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(GetPosX(), GetPosY(), GetPosZ());

	// �S�̍s���g�ݍ��킹�A���[���h�s����쐬
	DirectX::XMMATRIX W = MS * S * R * T * C;
	DirectX::XMStoreFloat4x4(&transform, W);
}

void AnimatedObject::PlayAnimation(int index, bool loop)
{
	currentAnimationIndex = index;
	currentAnimationSeconds = 0.0f;

	animationLoopFlag = loop;
	animationEndFlag = false;
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
	currentAnimationSeconds += Timer::Instance().DeltaTime();
	// ���݂̃t���[�����ő�t���[���𒴂��Ă�����
	if (currentAnimationSeconds >= animation.secondsLength)
	{
		if (animationLoopFlag)
		{
			// �擪�b����
			currentKeyFrame = 0;
			currentAnimationSeconds -= animation.secondsLength;
		}
		else
		{
			// �ŏI�b���Œ�~
			currentKeyFrame = animation.sequence.size() - 1;
			currentAnimationSeconds = animation.secondsLength;
			animationEndFlag = true;
		}
	}

	// keyFrame�ݒ�
	keyFrame = animation.sequence.at(currentKeyFrame);


	// �_�u���A�j���[�V�������L���Ȃ�㔼�g�̂ݕʃL�[�ɂ���
	if(isDoubleAnimations)
	{
		// �_�u���A�j���[�V�����f�[�^�擾
		ModelResource::Animation& doubleAnimation = model->GetModelResource()->GetAnimationClips().at(doubleCurrentAnimationIndex);

		doubleCurrentKeyFrame = static_cast<int>(doubleCurrentAnimationSeconds * animation.samplingRate);

		// �o�ߎ���
		doubleCurrentAnimationSeconds += Timer::Instance().DeltaTime();

		// ���݂̃t���[�����ő�t���[���𒴂��Ă�����
		if (doubleCurrentAnimationSeconds >= doubleAnimation.secondsLength)
		{
			// �_�u���A�j���[�V������OFF
			isDoubleAnimations = false;
			doubleCurrentKeyFrame = 0;
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
			DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(keyData.translation.x, keyData.translation.y, keyData.translation.z);

			// �e�s�񂪂���ΐe�̍s���������
			uint32_t uniqueIndex = model->GetModelResource()->GetSceneView().GetIndex(keyData.uniqueId);			// ���̃m�[�h��uniqueId �擾
			int64_t parentIndex = model->GetModelResource()->GetSceneView().nodes.at(uniqueIndex).parentIndex;		// ����uniqueId �� parent �̐e�� index ���擾

			// ��Z���ׂ��e�s�񂪂ǂ���̃A�j���[�V�������𒲂ׂ�
			bool doubleAnimationParent = false;
			for (auto checkIndex : doubleAnimationNodeIndex)
			{
				if(parentIndex == checkIndex)
				{
					doubleAnimationParent = true;						// �e�̓_�u���A�j���[�V�����̍s����g�p
					doubleAnimationNodeIndex.push_back(uniqueIndex);	// ���̃m�[�h�̎q�̂��߂ɒǉ�
					break;
				}
			}

			DirectX::XMMATRIX P;

			// �����̖��O�͕ς����悤�ɂ����ق����悳����
			if (keyData.name == "mixamorig:Spine")
			{
				doubleAnimationNodeIndex.push_back(uniqueIndex);
				doubleAnimationParent = true;						// �e�̓_�u���A�j���[�V�����̍s����g�p(�������͏ꍇ�ɂ���Ă͂�߂��ق�����������)
			}

			if(doubleAnimationParent)
			{
				P = parentIndex < 0 ? DirectX::XMMatrixIdentity() : DirectX::XMLoadFloat4x4(&keyFrame.nodes.at(parentIndex).globalTransform);
				XMStoreFloat4x4(&keyData.globalTransform, S * R * T * P);
				keyFrame.nodes.at(nodeIndex).globalTransform = keyData.globalTransform;
			}
		}
	}
}

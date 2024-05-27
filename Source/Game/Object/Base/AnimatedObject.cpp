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


	//int keyCount = animation.sequence.size();
	//for (int keyIndex = 0; keyIndex < keyCount - 1; keyIndex++)
	//{
	//	const ModelResource::KeyFrame& keyFrame0 = animation.sequence.at(keyIndex);
	//	const ModelResource::KeyFrame& keyFrame1 = animation.sequence.at(keyIndex+1);
	//	if(currentAnimationSeconds >= keyFrame0.)
	//}

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

	//node �̐������J��Ԃ�
	//size_t nodeCount = keyFrame.animKeyData.size();
	//for (size_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
	//{
	//	// ���݂� index �� node ���擾
	//	ModelResource::AnimKeyData& keyData = keyFrame.animKeyData.at(nodeIndex);
	//
	//	// ���ꂼ��̒l�ɑΉ�����s��̍쐬
	//	XMMATRIX S = XMMatrixScaling(keyData.scaling.x, keyData.scaling.y, keyData.scaling.z);
	//	XMMATRIX R = XMMatrixRotationQuaternion(XMLoadFloat4(&keyData.rotation));
	//	XMMATRIX T = XMMatrixTranslation(keyData.translation.x, keyData.translation.y, keyData.translation.z);
	//
	//	// �e�s�񂪂���ΐe�̍s���������
	//	int64_t parentIndex = modelResource->GetNodeGroup().nodeData.at(nodeIndex).parentIndex;
	//	XMMATRIX P = parentIndex < 0 ? XMMatrixIdentity() : XMLoadFloat4x4(&keyFrame.animKeyData.at(parentIndex).globalTransform);
	//
	//	// globalTransform �ɒl��ݒ�
	//	XMStoreFloat4x4(&keyData.globalTransform, S * R * T * P);
	//}
}

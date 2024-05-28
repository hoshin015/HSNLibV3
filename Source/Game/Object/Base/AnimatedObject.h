#pragma once
// --- Game ---
#include "BaseObject.h"
#include "AnimatedModel.h"

// �A�j���[�V��������3D�I�u�W�F�N�g�̃N���X
class AnimatedObject : public BaseObject
{
public:
	AnimatedObject() = default;
	AnimatedObject(const char* filePath);
	virtual ~AnimatedObject() = default;

	virtual void Update() = 0;
	virtual void Render(bool isShadow) = 0;

	AnimatedModel* GetModel() { return model.get(); }

	// �A�j���[�V�����Đ�
	void PlayAnimation(int index, bool loop);
protected:
	// �p���s��X�V
	void UpdateTransform() override;

	// �A�j���[�V�����X�V
	void UpdateAnimation();


protected:
	std::unique_ptr<AnimatedModel> model;

	// --- �A�j���[�V�����t���O ---
	ModelResource::KeyFrame keyFrame;				// �g�p�����L�[�t���[��
	int currentKeyFrame = 0;						// ���݂̃t���[��
	int currentAnimationIndex = 0;					// ���ݍĐ����̃A�j���[�V�����ԍ�
	float currentAnimationSeconds = 0.0f;			// ���ݍĐ����̕b��

	bool animationLoopFlag = true;					// ���[�v�Đ����邩�ǂ���
	bool animationEndFlag = false;					// �A�j���[�V�������Ō�̃L�[�t���[���ɓ��B�������ǂ���
	bool isPlayAnimation = true;					// �A�j���[�V�������Đ������ǂ���

	// --- �_�u���A�j���[�V�����p�t���O ---
	bool isDoubleAnimations = false;				// �㔼�g�ʃA�j���[�V�����g�����̃t���O
	ModelResource::KeyFrame doubleKeyFrame;			// �㔼�g�Ɏg�p�����L�[�t���[��
	int doubleCurrentKeyFrame = 0;					// �㔼�g�̌��݂̃t���[��
	int doubleCurrentAnimationIndex = 0;			// �㔼�g�̃A�j���[�V�����ԍ�
	float doubleCurrentAnimationSeconds = 0.0f;		// �㔼�g�̌��ݍĐ����̕b��

public:
	int GetCurrentKeyFrame() { return currentKeyFrame; }
	int GetCurrentAnimationIndex() { return currentAnimationIndex; }
	bool GetAnimationLoopFlag() { return animationLoopFlag; }
	bool GetIsPlayAnimation() { return isPlayAnimation; }

	void SetKeyFrame(ModelResource::KeyFrame kf) { keyFrame = kf; }
	void SetCurrentKeyFrame(int kf) { currentKeyFrame = kf; }
	void SetCurrentAnimationIndex(int index) { currentAnimationIndex = index; }
	void SetCurrentAnimationSeconds(float s) { currentAnimationSeconds = s; }
	void SetAnimationLoopFlag(bool b) { animationLoopFlag = b; }
	void SetAnimationEndFlag(bool b) { animationEndFlag = b; }
	void SetIsPlayAnimation(bool b) { isPlayAnimation = b; }

	void SetIsDoubleAnimation(bool b) { isDoubleAnimations = b; }
	void SetDoubleCurrentAnimationIndex(int index) { doubleCurrentAnimationIndex = index; }
};
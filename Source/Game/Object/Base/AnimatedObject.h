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

	// �f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive();

	AnimatedModel* GetModel() { return model.get(); }

	// �A�j���[�V�����Đ�
	void PlayAnimation(int index, bool loop);

	// �A�j���[�V�����u�����h
	void BlendAnimation(const ModelResource::KeyFrame* keyFrames[2], float factor, ModelResource::KeyFrame& keyFrame);
protected:
	// �p���s��X�V
	void UpdateTransform() override;

	// �A�j���[�V�����X�V
	void UpdateAnimation();

	//
	void UpdateBlendAnimation();

public:
	DirectX::XMFLOAT3 GetBonePosition(std::string boneName);

protected:
	std::unique_ptr<AnimatedModel> model;

	// --- �A�j���[�V�����t���O ---
	ModelResource::KeyFrame keyFrame;                       // �g�p�����L�[�t���[��
	int                     currentKeyFrame         = 0;    // ���݂̃t���[��
	int                     currentAnimationIndex   = 0;    // ���ݍĐ����̃A�j���[�V�����ԍ�
	float                   currentAnimationSeconds = 0.0f; // ���ݍĐ����̕b��

	float					animationTimeScale = 1.0f;	// �A�j���[�V�����̍Đ����x

	bool animationLoopFlag = true;  // ���[�v�Đ����邩�ǂ���
	bool animationEndFlag  = false; // �A�j���[�V�������Ō�̃L�[�t���[���ɓ��B�������ǂ���
	bool isPlayAnimation   = true;  // �A�j���[�V�������Đ������ǂ���

	// --- �A�j���[�V�����u�����h�p�t���O ---
	int   blendAnimationIndex              = 0;    // �J�ڑO�̃A�j���[�V�����ԍ�
	float animationBlendTimer               = 0.0f; // �A�j���[�V�����u�����h�p�^�C�}�[
	float animationBlendTransitionDurations = 0.0f; // ���̎��Ԃ��|���ăA�j���[�V������J�ڂ�����
	float exitTime                          = 0.0f; // (�񃋁[�v�A�j���[�V�������g�p)���݂ɃA�j���[�V�����̎��Ԃ������܂œ��B�����玟�̃A�j���[�V�����ɑJ�ڊJ�n

	// --- �_�u���A�j���[�V�����p�t���O ---
	bool                    isDoubleAnimations = false;           // �㔼�g�ʃA�j���[�V�����g�����̃t���O
	ModelResource::KeyFrame doubleKeyFrame;                       // �㔼�g�Ɏg�p�����L�[�t���[��
	int                     doubleCurrentKeyFrame         = 0;    // �㔼�g�̌��݂̃t���[��
	int                     doubleCurrentAnimationIndex   = 0;    // �㔼�g�̃A�j���[�V�����ԍ�
	float                   doubleCurrentAnimationSeconds = 0.0f; // �㔼�g�̌��ݍĐ����̕b��

public:
	// --- �A�j���[�V�����t���O ---
	int GetCurrentKeyFrame() { return currentKeyFrame; }
	int GetCurrentAnimationIndex() { return currentAnimationIndex; }
	float GetCurrentAnimationSeconds() { return currentAnimationSeconds; }

	bool GetAnimationLoopFlag() { return animationLoopFlag; }
	bool GetIsPlayAnimation() { return isPlayAnimation; }
	bool GetAnimationEndFlag() { return animationEndFlag; }

	void SetKeyFrame(ModelResource::KeyFrame kf) { keyFrame = kf; }
	void SetCurrentKeyFrame(int kf) { currentKeyFrame = kf; }
	void SetCurrentAnimationIndex(int index) { currentAnimationIndex = index; }
	void SetCurrentAnimationSeconds(float s) { currentAnimationSeconds = s; }

	void SetAnimationLoopFlag(bool b) { animationLoopFlag = b; }
	void SetAnimationEndFlag(bool b) { animationEndFlag = b; }
	void SetIsPlayAnimation(bool b) { isPlayAnimation = b; }

	// --- �A�j���[�V�����u�����h�p�t���O ---
	int   GetBeforeAnimationIndex() { return blendAnimationIndex; }
	float GetAnimationBlendTimer() { return animationBlendTimer; }
	float GetAnimationBlendTransitionDurations() { return animationBlendTransitionDurations; }
	float GetExitTime() { return exitTime; }

	void SetBeforeAnimationIndex(int index) { blendAnimationIndex = index; }
	void SetAnimationBlendTimer(float time) { animationBlendTimer = time; }
	void SetAnimationBlendTransitionDurations(float time) { animationBlendTransitionDurations = time; }
	void SetExitTime(float time) { exitTime = time; }

	// --- �_�u���A�j���[�V�����p�t���O ---
	bool GetIsDoubleAnimation() { return isDoubleAnimations; }

	void SetIsDoubleAnimation(bool b) { isDoubleAnimations = b; }
	void SetDoubleCurrentAnimationIndex(int index) { doubleCurrentAnimationIndex = index; }
};

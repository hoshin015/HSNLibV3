#pragma once
#include <memory>
#include "../../Library/2D/Sprite.h"
#include "../../Library/Easing.h"

class UiPause
{
private:
	UiPause(){};
	~UiPause(){};

public:
	static UiPause& Instance()
	{
		static UiPause instance;
		return instance;
	}

	// ������
	void Initialize();

	// �X�V
	bool Update();

	// �`��
	void Render();

	// isPause �ݒ�
	void SetPause(bool isPause) { this->isPause = isPause; }
	bool GetPause() const { return isPause; }

	

private:
	bool isPause = false;

	enum class UiPauseState
	{
		Hidden,
		Showing,
		Visible,
		Hide,
	};
	UiPauseState state;

	float pauseTimer = 0.0f;			// �|�[�Y��ԊǗ��p�^�C�}�[
	float pauseTotalTime = 1.0;		// �|�[�YUI�̑J�ڊ����܂ł̎���

	// UI �̃C�[�W���O�P�P�ɂ�����������ĊǗ�
	struct UiEasingValue
	{
		float startTime;
		float endTime;
		float startValue;
		float endValue;
	};
	struct UiEasingValueVec
	{
		float startTime;
		float endTime;
		DirectX::XMFLOAT2 startValueVec;
		DirectX::XMFLOAT2 endValueVec;
	};


	std::unique_ptr<Sprite> pauseText;
	UiEasingValueVec pauseTextPos = 
	{
		0.0f,0.4f,
		{-100,100},
		{200, 100}
	};
	UiEasingValue pauseTextAlpha =
	{
		0.0,0.4,
		0, 1
	};

	std::unique_ptr<Sprite> pauseText1;
	UiEasingValueVec pauseTextPos1 =
	{
		0.2f,0.6f,
		{-100,250},
		{200, 250}
	};
	UiEasingValue pauseTextAlpha1 =
	{
		0.2,0.6,
		0, 1
	};

	std::unique_ptr<Sprite> pauseText2;
	UiEasingValueVec pauseTextPos2 =
	{
		0.4f,0.8f,
		{-100,400},
		{200, 400}
	};
	UiEasingValue pauseTextAlpha2 =
	{
		0.4,0.8,
		0, 1
	};


	std::unique_ptr<Sprite> pauseImg1;
	UiEasingValueVec pauseImg1Pos =
	{
		0, 1,
		{1300,200},{800,200}
	};

	std::unique_ptr<Sprite> pauseImg2;
	DirectX::XMFLOAT2 pauseImg2HiddenPos = { -100,350 };
	DirectX::XMFLOAT2 pauseImg2VisiblePos = { 640, 350 };

public:
	// �C�[�W���O�ɂ�錻�݂̃p�����[�^�擾
	float GetNowParam(float time, float startTime, float endTime, float startValue, float endValue);

	template<typename Ty>
	using EasingFunc = Ty(*)(Ty, Ty, Ty, Ty);

	template<typename Ty>
	// �C�[�W���O�ɂ�錻�݂̃p�����[�^�擾
	float GetNowParam(EasingFunc<Ty> func, float time, UiEasingValue uiEasingValue);
	// �C�[�W���O�ɂ�錻�݂̃p�����[�^�擾
	template<typename Ty>
	DirectX::XMFLOAT2 GetNowParamVec(EasingFunc<Ty> func, float time, UiEasingValueVec uiEasingValueVec);
};
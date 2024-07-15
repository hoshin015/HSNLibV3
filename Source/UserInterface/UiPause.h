#pragma once
#include <memory>
#include "../../Library/2D/Sprite.h"
#include "../../Library/Easing.h"

class UiPause
{
private:
	UiPause()
	{
	};

	~UiPause()
	{
	};

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

	float pauseTimer     = 0.0f; // �|�[�Y��ԊǗ��p�^�C�}�[
	float pauseTotalTime = 1.0;  // �|�[�YUI�̑J�ڊ����܂ł̎���

	
};

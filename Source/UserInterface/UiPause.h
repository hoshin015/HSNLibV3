#pragma once
#include <memory>
#include "../../Library/2D/Sprite.h"
#include "../../Library/Easing.h"

class UiPause
{
private:
	UiPause()
	{
		imgBlack = std::make_unique<Sprite>("Data/Texture/Black.png");
		sprites.emplace_back(imgBlack.get());
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
	void SetAllOffRender();

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

	std::vector<Sprite*> sprites;

	float pauseTimer     = 0.0f; // �|�[�Y��ԊǗ��p�^�C�}�[
	float pauseTotalTime = 0.5;  // �|�[�YUI�̑J�ڊ����܂ł̎���

	std::unique_ptr<Sprite> imgBlack;
	Easing::EasingValue imgBlackAlpha =
	{
	0.0f, 0.5f,
	0.0f, 0.7f
	};
};

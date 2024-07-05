#pragma once
#include <memory>
#include "../../Library/2D/Sprite.h"
#include "../../Library/Easing.h"

class UiTitle
{
private:
	UiTitle()
	{
	};

	~UiTitle()
	{
	};

public:
	static UiTitle& Instance()
	{
		static UiTitle instance;
		return instance;
	}

	// ������
	void Initialize();

	// �X�V
	void Update();

	// �`��
	void Render();

private:
	// �^�C�g��UI��ԊǗ��p
	enum class UiTitleState
	{
		Title,
		TitleToSelectMenu,
		SelectMenuToTitle,
		SelectMenu,
		SelectMenuToLevel,
		Level,
		LevelToTitle,
	};
	UiTitleState state;

	float titleTimer = 0.0f;


	// --- titleToSelectMenu ---
	float titleToSelectMenuTime = 1.0f;


	std::unique_ptr<Sprite> imgPressAnyButton;
	Easing::EasingValue imgPressAnyButtonScale =
	{
		0.0f, 0.25f,
		1.0f, 1.1f
	};
	Easing::EasingValue imgPressAnyButtonAlpha =
	{
		0.0f, 0.25f,
		1.0f, 0.0f
	};

	std::unique_ptr<Sprite> imgBgCover;
	Easing::EasingValue imgBgCoverAlpha =
	{
		0.5f, 0.7f,
		0.0f, 0.8f
	};

	std::unique_ptr<Sprite> imgGameStart;
	Easing::EasingValue imgGameStartAlpha =
	{
		0.5f, 0.7f,
		0.0f, 1.0f
	};
	Easing::EasingValueVec2 imgGameStartPos =
	{
		0.5f, 0.7f,
		{50.0f, 100.0f}, {100.0f,100.0}
	};

	std::unique_ptr<Sprite> imgOptions;
	Easing::EasingValue imgOptionsAlpha =
	{
		0.6f, 0.8f,
		0.0f, 1.0f
	};
	Easing::EasingValueVec2 imgOptionsPos =
	{
		0.6f, 0.8f,
		{50.0f, 200.0f}, {100.0f,200.0}
	};

	std::unique_ptr<Sprite> imgQuit;
	Easing::EasingValue imgQuitAlpha =
	{
		0.7f, 0.9f,
		0.0f, 1.0f
	};
	Easing::EasingValueVec2 imgQuitPos =
	{
		0.7f, 0.9f,
		{50.0f, 300.0f}, {100.0f,300.0}
	};

	// --- selectMenu ---
	enum class SelectMenu
	{
		GamePlay,
		Options,
		Quit,
		END,
	};
	int selectMenu;

	std::unique_ptr<Sprite> imgSelectBar;

	// --- level ---
	float selectMenuToSelectLevelTime = 1.0f;
	enum class SelectLevel
	{
		Easy,
		Normal,
		Hard,
		End,
	};
	int selectLevel;

	std::unique_ptr<Sprite> imgSelectLevel;
	DirectX::XMFLOAT2 imgSelectLevelPos = { 200.0f, 100.0f };
	Easing::EasingValue imgSelectLevelDissolveThread =
	{
		0.0f, 0.5f,
		0.0f, 1.5f
	};

	std::unique_ptr<Sprite> imgEasy;
	DirectX::XMFLOAT2 imgEasyPos = { 200.0f, 200.0f };

	std::unique_ptr<Sprite> imgNormal;
	DirectX::XMFLOAT2 imgNormalPos = { 200.0f, 300.0f };

	std::unique_ptr<Sprite> imgHard;
	DirectX::XMFLOAT2 imgHardPos = { 200.0f, 400.0f };
};

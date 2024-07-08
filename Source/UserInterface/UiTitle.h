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

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Render();

	// debugGui
	void DrawDebugImGui();


private:
	bool isStageRender = true;			// タイトルステージ描画フラグ
	bool isCharacterRender = false;		// タイトルキャラクター描画フラグ

public:
	bool GetIsStageRender() { return isStageRender; }
	bool GetIsCharacterRender() { return isCharacterRender; }

private:
	void SetAllOffRender();

private:
	std::vector<Sprite*> sprites;

	// タイトルUI状態管理用
	enum class UiTitleState
	{
		Init,
		ToTitle1,
		ToTitle2,
		Title1,
		Title2,
		TitleToSelectMenu1,
		TitleToSelectMenu2,
		SelectMenu1,
		SelectMenu2,
		SelectMenuToLevel1,
		SelectMenuToLevel2,
		Level1,
		Level2,
	};
	UiTitleState state;

	float titleTimer = 0.0f;

	// --- toTitleMenu ---
	float toTitleTime = 2.0f;
	Easing::EasingValue imgTitleLogDissolveThread =
	{
		0.0f, 1.5f,
		0.0f, 1.0f
	};
	Easing::EasingValue imgTitleTextDissolveThread =
	{
		0.5f, 1.7f,
		0.0f, 1.0f
	};

	// --- enter back text ---
	std::unique_ptr<Sprite> imgEnterText;
	DirectX::XMFLOAT2 imgEnterTextPos = { 100, 650 };
	std::unique_ptr<Sprite> imgBackText;
	DirectX::XMFLOAT2 imgBackTextPos = { 200, 650 };

	Easing::EasingValue imgEnterBackTextTitleToSelectMenuAlpha =
	{
		0.7f, 0.9f,
		0.0f, 1.0f
	};
	Easing::EasingValue imgEnterBackTextSelectMenuToLevelAlpha =
	{
		0.3f, 0.5f,
		0.0f, 1.0f
	};

	// --- titleToSelectMenu ---
	float titleToSelectMenuTime = 1.0f;

	std::unique_ptr<Sprite> imgTitleLogo;
	Easing::EasingValue imgTitleLogoAlpha =
	{
		0.0f, 0.25f,
		1.0f, 0.0f
	};
	std::unique_ptr<Sprite> imgTitleLogoSmall;
	Easing::EasingValue imgTitleLogoSmallAlpha =
	{
		0.7f, 0.9f,
		0.0f, 1.0f
	};

	std::unique_ptr<Sprite> imgTitleText;
	DirectX::XMFLOAT2 imgTitleTextPos = {640, 360};
	Easing::EasingValue imgTitleTextAlpha =
	{
		0.0f, 0.25f,
		1.0f, 0.0f
	};

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
		END,
	};
	int selectLevel;

	std::unique_ptr<Sprite> imgSelectLevelBgCover;
	Easing::EasingValue imgSelectLevelBgCoverDissolveThread =
	{
		0.0f, 0.6f,
		0.0f, 1.0f
	};

	std::unique_ptr<Sprite> imgSelectLevel;
	DirectX::XMFLOAT2 imgSelectLevelPos = { 200.0f, 100.0f };

	std::unique_ptr<Sprite> imgEasy;
	DirectX::XMFLOAT2 imgEasyPos = { 200.0f, 250.0f };

	std::unique_ptr<Sprite> imgNormal;
	DirectX::XMFLOAT2 imgNormalPos = { 200.0f, 400.0f };

	std::unique_ptr<Sprite> imgHard;
	DirectX::XMFLOAT2 imgHardPos = { 200.0f, 550.0f };
};

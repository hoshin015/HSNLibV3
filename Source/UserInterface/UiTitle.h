#pragma once
#include <memory>
#include "../../Library/2D/Sprite.h"
#include "../../Library/Easing.h"

class UiTitle
{
private:
	UiTitle()
	{
		imgBlack = std::make_unique<Sprite>("Data/Texture/Black.png");
		sprites.emplace_back(imgBlack.get());

		imgEnterText = std::make_unique<Sprite>("Data/Texture/Text/enterText.png");
		imgEnterText->SetIsRender(false);
		imgEnterText->SetColorA(0.0f);
		imgEnterText->SetPos(imgEnterTextPos);
		sprites.emplace_back(imgEnterText.get());
		imgBackText = std::make_unique<Sprite>("Data/Texture/Text/backText.png");
		imgBackText->SetIsRender(false);
		imgBackText->SetColorA(0.0f);
		imgBackText->SetPos(imgBackTextPos);
		sprites.emplace_back(imgBackText.get());


		imgTitleLogo = std::make_unique<Sprite>("Data/Texture/UserInterface/Title/TitleLogo.png");
		imgTitleLogo->SetDissolveTexture(L"Data/Texture/Noise/DirtGradient.png");
		sprites.emplace_back(imgTitleLogo.get());
		imgTitleLogoSmall = std::make_unique<Sprite>("Data/Texture/UserInterface/Title/TitleLogoSmall.png");
		imgTitleLogoSmall->SetIsRender(false);
		imgTitleLogoSmall->SetColorA(0.0f);
		sprites.emplace_back(imgTitleLogoSmall.get());

		imgPressAnyButton = std::make_unique<Sprite>("Data/Texture/Text/PressAnyButton.sprite");
		imgPressAnyButton->UpdateAnimation();
		imgPressAnyButton->SetPos({ 640, 550 });
		sprites.emplace_back(imgPressAnyButton.get());

		imgTitleText = std::make_unique<Sprite>("Data/Texture/UserInterface/Title/titleText.sprite");
		imgTitleText->SetDissolveTexture(L"Data/Texture/Noise/DirtGradient2.png");
		imgTitleText->UpdateAnimation();
		imgTitleText->SetPos(imgTitleTextPos);
		sprites.emplace_back(imgTitleText.get());

		imgBgCover = std::make_unique<Sprite>("Data/Texture/UserInterface/Title/BgCover.png");
		imgBgCover->SetPos({ 0, 0 });
		imgBgCover->SetIsRender(false);
		imgBgCover->SetColorA(0.0f);
		sprites.emplace_back(imgBgCover.get());

		imgGameStart = std::make_unique<Sprite>("Data/Texture/Text/GameStart.sprite");
		imgGameStart->UpdateAnimation();
		imgGameStart->SetIsRender(false);
		sprites.emplace_back(imgGameStart.get());

		imgOptions = std::make_unique<Sprite>("Data/Texture/Text/Options.sprite");
		imgOptions->UpdateAnimation();
		imgOptions->SetIsRender(false);
		sprites.emplace_back(imgOptions.get());

		imgQuit = std::make_unique<Sprite>("Data/Texture/Text/Quit.sprite");
		imgQuit->UpdateAnimation();
		imgQuit->SetIsRender(false);
		sprites.emplace_back(imgQuit.get());

		imgSelectBar = std::make_unique<Sprite>("Data/Texture/UserInterface/Title/selectBar.sprite");
		imgSelectBar->UpdateAnimation();
		imgSelectBar->SetIsRender(false);
		imgSelectBar->SetPos({ 100.0f, 100.0 });
		sprites.emplace_back(imgSelectBar.get());


		imgSelectLevelBgCover = std::make_unique<Sprite>("Data/Texture/UserInterface/Title/levelSelectBgCover.png");
		imgSelectLevelBgCover->SetIsRender(false);
		imgSelectLevelBgCover->SetDissolveTexture(L"Data/Texture/Noise/GrungeMap.png");
		imgSelectLevelBgCover->spriteDissolveConstant.edgeThreshold = 0.0f;
		imgSelectLevelBgCover->spriteDissolveConstant.edgeColor = { 0.2f, 0.2f, 1.0f, 1.0f };
		sprites.emplace_back(imgSelectLevelBgCover.get());

		imgSelectLevel = std::make_unique<Sprite>("Data/Texture/Text/LevelSelect.sprite");
		imgSelectLevel->UpdateAnimation();
		imgSelectLevel->SetIsRender(false);
		imgSelectLevel->SetPos(imgSelectLevelPos);
		sprites.emplace_back(imgSelectLevel.get());

		imgEasy = std::make_unique<Sprite>("Data/Texture/Text/Easy.sprite");
		imgEasy->UpdateAnimation();
		imgEasy->SetIsRender(false);
		imgEasy->SetPos(imgEasyPos);
		sprites.emplace_back(imgEasy.get());

		imgNormal = std::make_unique<Sprite>("Data/Texture/Text/normal.sprite");
		imgNormal->UpdateAnimation();
		imgNormal->SetIsRender(false);
		imgNormal->SetPos(imgNormalPos);
		sprites.emplace_back(imgNormal.get());

		imgHard = std::make_unique<Sprite>("Data/Texture/Text/hard.sprite");
		imgHard->UpdateAnimation();
		imgHard->SetIsRender(false);
		imgHard->SetPos(imgHardPos);
		sprites.emplace_back(imgHard.get());


		// eitter
		imgEmitterTop = std::make_unique<Sprite>("Data/Texture/Effect/particle0.sprite");
		imgEmitterTop->UpdateAnimation();
		imgEmitterTop->SetScale({ 0.07,0.07 });
		imgEmitterTop->SetColor({ 1.0,2.0,2.0,0.5 });
		imgEmitterTop->SetIsRender(false);
		sprites.emplace_back(imgEmitterTop.get());
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

	// --- black ---
	std::unique_ptr<Sprite> imgBlack;
	Easing::EasingValue imgBlackToTitleAlpha =
	{
		0.0f, 0.3f,
		1.0f, 0.0f
	};
	Easing::EasingValue imgBlackSelectMenuToLevelAlpha =
	{
		0.0f, 0.3f,
		1.0f, 0.0f
	};

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


private:
	std::unique_ptr<Sprite> imgEmitterTop;

	float uiEmitterTimer = 0.0f;
	float uiEmitterTime = 0.03f;
	DirectX::XMFLOAT2 emitterPos = { 0,0 };
	DirectX::XMFLOAT2 emitterTargetPos = { 0,0 };
	float emitterMoveTimer = 0.0f;

	void EmitUpdate();
public:
	bool isEmitterRender = false;
};

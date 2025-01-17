#pragma once
#include <memory>
#include "../../Library/2D/Sprite.h"
#include "../../Library/Easing.h"
#include "../../Library/Video/Video.h"
#include "../../Library/Graphics/Graphics.h"

class UiTitle
{
private:
	UiTitle()
	{
		imgBlack = std::make_unique<Sprite>("Data/Texture/Black.png");
		sprites.emplace_back(imgBlack.get());

		imgBackBlack = std::make_unique<Sprite>("Data/Texture/Black.png");
		sprites.emplace_back(imgBackBlack.get());

		imgEnterKeyText = std::make_unique<Sprite>("Data/Texture/Text/Key/enter.png");
		imgEnterKeyText->SetIsRender(false);
		imgEnterKeyText->SetColorA(0.0f);
		imgEnterKeyText->SetPos(imgEnterTextPos);
		sprites.emplace_back(imgEnterKeyText.get());

		imgEnterPadText = std::make_unique<Sprite>("Data/Texture/Text/Pad/enter.png");
		imgEnterPadText->SetIsRender(false);
		imgEnterPadText->SetColorA(0.0f);
		imgEnterPadText->SetPos(imgEnterTextPos);
		sprites.emplace_back(imgEnterPadText.get());

		imgBackKeyText = std::make_unique<Sprite>("Data/Texture/Text/Key/back.png");
		imgBackKeyText->SetIsRender(false);
		imgBackKeyText->SetColorA(0.0f);
		imgBackKeyText->SetPos(imgBackTextPos);
		sprites.emplace_back(imgBackKeyText.get());

		imgBackPadText = std::make_unique<Sprite>("Data/Texture/Text/Pad/back.png");
		imgBackPadText->SetIsRender(false);
		imgBackPadText->SetColorA(0.0f);
		imgBackPadText->SetPos(imgBackTextPos);
		sprites.emplace_back(imgBackPadText.get());

		imgTitleLogo = std::make_unique<Sprite>("Data/Texture/UserInterface/Title/TitleLogo.png");
		imgTitleLogo->SetDissolveTexture(L"Data/Texture/Noise/DirtGradient.png");
		sprites.emplace_back(imgTitleLogo.get());
		imgTitleLogoSmall = std::make_unique<Sprite>("Data/Texture/UserInterface/Title/TitleLogoSmall.png");
		imgTitleLogoSmall->SetIsRender(false);
		imgTitleLogoSmall->SetColorA(0.0f);
		sprites.emplace_back(imgTitleLogoSmall.get());

		imgPressAnyKeyButton = std::make_unique<Sprite>("Data/Texture/Text/Key/PressAnyButton.sprite");
		imgPressAnyKeyButton->UpdateAnimation();
		imgPressAnyKeyButton->SetPos({ 640, 550 });
		sprites.emplace_back(imgPressAnyKeyButton.get());

		imgPressAnyPadButton = std::make_unique<Sprite>("Data/Texture/Text/Pad/PressAnyButton.sprite");
		imgPressAnyPadButton->UpdateAnimation();
		imgPressAnyPadButton->SetPos({ 640, 550 });
		sprites.emplace_back(imgPressAnyPadButton.get());

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

		imgEasySelect = std::make_unique<Sprite>("Data/Texture/Text/easySelect.sprite");
		imgEasySelect->UpdateAnimation();
		imgEasySelect->SetIsRender(false);
		imgEasySelect->SetPos(imgEasyPos);
		sprites.emplace_back(imgEasySelect.get());

		imgNormal = std::make_unique<Sprite>("Data/Texture/Text/normal.sprite");
		imgNormal->UpdateAnimation();
		imgNormal->SetIsRender(false);
		imgNormal->SetPos(imgNormalPos);
		sprites.emplace_back(imgNormal.get());

		imgNormalSelect = std::make_unique<Sprite>("Data/Texture/Text/normalSelect.sprite");
		imgNormalSelect->UpdateAnimation();
		imgNormalSelect->SetIsRender(false);
		imgNormalSelect->SetPos(imgNormalPos);
		sprites.emplace_back(imgNormalSelect.get());

		imgHard = std::make_unique<Sprite>("Data/Texture/Text/hard.sprite");
		imgHard->UpdateAnimation();
		imgHard->SetIsRender(false);
		imgHard->SetPos(imgHardPos);
		sprites.emplace_back(imgHard.get());

		imgHardSelect = std::make_unique<Sprite>("Data/Texture/Text/hardSelect.sprite");
		imgHardSelect->UpdateAnimation();
		imgHardSelect->SetIsRender(false);
		imgHardSelect->SetPos(imgHardPos);
		sprites.emplace_back(imgHardSelect.get());

		// eitter
		imgEmitterTop = std::make_unique<Sprite>("Data/Texture/Effect/particle0.sprite");
		imgEmitterTop->UpdateAnimation();
		imgEmitterTop->SetScale({ 0.07,0.07 });
		imgEmitterTop->SetColor({ 1.0,2.0,2.0,0.7 });
		imgEmitterTop->SetIsRender(false);
		sprites.emplace_back(imgEmitterTop.get());

		tutorialVideo.LoadFile(Graphics::Instance().GetDevice(), L"Data/Video/tutorial.mp4");
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
	int GetLevel() const { return selectLevel; }

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
		SelectMenuToOption1,
		SelectMenuToOption2,
		Option1,
		Option2,
	};
	UiTitleState state;

	float titleTimer = 0.0f;

	// --- black ---
	std::unique_ptr<Sprite> imgBlack;
	std::unique_ptr<Sprite> imgBackBlack;
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
	Easing::EasingValue imgBlackSelectMenuToOptionAlpha =
	{
		0.0f, 0.5f,
		0.0f, 0.75f
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
	std::unique_ptr<Sprite> imgEnterKeyText;
	std::unique_ptr<Sprite> imgEnterPadText;
	DirectX::XMFLOAT2 imgEnterTextPos = { 200, 650 };
	std::unique_ptr<Sprite> imgBackKeyText;
	std::unique_ptr<Sprite> imgBackPadText;
	DirectX::XMFLOAT2 imgBackTextPos = { 300, 650 };
	DirectX::XMFLOAT2 imgBackTextPos2 = { 605, 650 };

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
	DirectX::XMFLOAT2 imgTitleTextPos = {0, 0};
	Easing::EasingValue imgTitleTextAlpha =
	{
		0.0f, 0.25f,
		1.0f, 0.0f
	};

	std::unique_ptr<Sprite> imgPressAnyKeyButton;
	std::unique_ptr<Sprite> imgPressAnyPadButton;
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
	DirectX::XMFLOAT2 imgSelectLevelPos = { 300.0f, 100.0f };

	std::unique_ptr<Sprite> imgEasy;
	std::unique_ptr<Sprite> imgEasySelect;
	DirectX::XMFLOAT2 imgEasyPos = { 300.0f, 250.0f };

	std::unique_ptr<Sprite> imgNormal;
	std::unique_ptr<Sprite> imgNormalSelect;
	DirectX::XMFLOAT2 imgNormalPos = { 300.0f, 400.0f };

	std::unique_ptr<Sprite> imgHard;
	std::unique_ptr<Sprite> imgHardSelect;
	DirectX::XMFLOAT2 imgHardPos = { 300.0f, 550.0f };

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

private:
	Video tutorialVideo;
	bool isVideoRender = false;
};

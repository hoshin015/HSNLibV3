#pragma once
#include <memory>
#include "../../Library/2D/Sprite.h"
#include "../../Library/Easing.h"

class UiClearAfter
{
private:
	UiClearAfter()
	{
		imgText = std::make_unique<Sprite>("Data/Texture/Font/damageFont.sprite");
		imgText->UpdateAnimation();
		imgText->SetIsRender(false);
		sprites.emplace_back(imgText.get());

		imgClearAfterTimer = std::make_unique<Sprite>("Data/Texture/UserInterface/ClearAfter/clearAfterTimer.sprite");
		imgClearAfterTimer->UpdateAnimation();
		imgClearAfterTimer->SetPos({ 640, 100 });
		imgClearAfterTimer->SetIsRender(false);
		sprites.emplace_back(imgClearAfterTimer.get());

		imgQuestClearText = std::make_unique<Sprite>("Data/Texture/UserInterface/ClearAfter/questClear.sprite");
		imgQuestClearText->UpdateAnimation();
		imgQuestClearText->SetPos({ 640, 360 });
		imgQuestClearText->SetIsRender(false);
		sprites.emplace_back(imgQuestClearText.get());

		imgClearBackBar = std::make_unique<Sprite>("Data/Texture/UserInterface/ClearAfter/clearBackBar.sprite");
		imgClearBackBar->SetDissolveTexture(L"Data/Texture/Noise/GrungeMap.png");
		imgClearBackBar->UpdateAnimation();
		imgClearBackBar->SetPos({ 640, 360 });
		imgClearBackBar->SetIsRender(false);

		sprites.emplace_back(imgClearBackBar.get());

		imgClearText = std::make_unique<Sprite>("Data/Texture/UserInterface/ClearAfter/clear.sprite");
		imgClearText->UpdateAnimation();
		imgClearText->SetPos({ 640, 360 });
		imgClearText->SetIsRender(false);
		sprites.emplace_back(imgClearText.get());

		imgBlack = std::make_unique<Sprite>("Data/Texture/Black.png");
		imgBlack->SetIsRender(false);
		sprites.emplace_back(imgBlack.get());
	}
	~UiClearAfter() {}

public:
	static UiClearAfter& Instance()
	{
		static UiClearAfter instance;
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

	// クリアフラグON
	void OnClear();

	bool IsClear() const { return clearFlag; }

	void SetAllOffRender();

private:
	bool clearFlag = false;	// このフラグをONにするとリザルト画面まで流れる
	std::vector<Sprite*> sprites;

	enum class State
	{
		NO,
		ClearWait,
		ClearAnim
	};
	int state = static_cast<int>(State::NO);

	float clearAfterTime = 40.0f;
	float clearAfterTimer = 0.0f;

	std::unique_ptr<Sprite> imgText;
	std::string text;

	// clearWait
	float clearWaitTimer = 0.0f;
	Easing::EasingValue questClearTextInAlpha =
	{
		0.0f, 0.5f,
		0.0f, 1.0f
	};
	Easing::EasingValue questClearTextInPosY =
	{
		0.0f, 0.5f,
		-100.0f, 360.0f
	};
	Easing::EasingValue questClearTextOutAlpha =
	{
		2.0f, 2.5f,
		1.0f, 0.0f
	};
	Easing::EasingValue questClearTextOutPosY =
	{
		2.0f, 2.5f,
		360.0f, 820.0f
	};

	std::unique_ptr<Sprite> imgQuestClearText;
	std::unique_ptr<Sprite> imgClearAfterTimer;


	// clearAnim
	float clearAnimTimer = 0.0f;
	float clearAnimTime = 6.5f;

	std::unique_ptr<Sprite> imgClearBackBar;
	Easing::EasingValue imgClearBackBarDissolveThread =
	{
		3.3, 3.8,
		0.0f, 1.0f,
	};


	std::unique_ptr<Sprite> imgClearText;
	Easing::EasingValue imgClearTextAlpha =
	{
		3.0f, 3.5f,
		0.0f,1.0f
	};
	Easing::EasingValue imgClearTextScale =
	{
		3.0f, 3.5f,
		2.0f,1.0f
	};
	Easing::EasingValue imgClearTextAngle =
	{
		3.0f, 3.5f,
		120.0f, 0.0f
	};


	std::unique_ptr<Sprite> imgBlack;
	Easing::EasingValue imgBlackAlpha =
	{
		5.0f, 6.0f,
		0.0f, 1.0f
	};
};

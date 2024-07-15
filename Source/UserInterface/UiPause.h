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
		imgPauseText = std::make_unique<Sprite>("Data/Texture/UserInterface/Pause/pauseText.sprite");
		imgPauseText->UpdateAnimation();
		imgPauseText->SetPos({ 640, 360 });
		sprites.emplace_back(imgPauseText.get());
		imgResume = std::make_unique<Sprite>("Data/Texture/UserInterface/Pause/resume.png");
		imgResume->SetPos({ 400, 550 });
		sprites.emplace_back(imgResume.get());
		imgGoTitle = std::make_unique<Sprite>("Data/Texture/UserInterface/Pause/goTitle.png");
		imgGoTitle->SetPos({ 700, 550 });
		sprites.emplace_back(imgGoTitle.get());
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

	// 初期化
	void Initialize();

	// 更新
	bool Update();

	// 描画
	void Render();

	// isPause 設定
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

	float pauseTimer     = 0.0f; // ポーズ状態管理用タイマー
	float pauseTotalTime = 0.2;  // ポーズUIの遷移完了までの時間

	std::unique_ptr<Sprite> imgBlack;
	Easing::EasingValue imgBlackAlpha =
	{
	0.0f, 0.2f,
	0.0f, 0.7f
	};

	std::unique_ptr<Sprite> imgPauseText;
	Easing::EasingValue imgPauseTextAlpha =
	{
		0.1f, 0.2f,
		0.0f,1.0f
	};

	std::unique_ptr<Sprite> imgResume;
	std::unique_ptr<Sprite> imgGoTitle;
};

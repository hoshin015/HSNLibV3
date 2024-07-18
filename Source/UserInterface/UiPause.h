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
		imgResume = std::make_unique<Sprite>("Data/Texture/UserInterface/Pause/resume.sprite");
		imgResume->UpdateAnimation();
		imgResume->SetPos({ 500, 550 });
		sprites.emplace_back(imgResume.get());
		imgGoTitle = std::make_unique<Sprite>("Data/Texture/UserInterface/Pause/goTitle.sprite");
		imgGoTitle->UpdateAnimation();
		imgGoTitle->SetPos({ 800, 550 });
		sprites.emplace_back(imgGoTitle.get());
		imgResumeGoTitleSelect = std::make_unique<Sprite>("Data/Texture/UserInterface/Pause/resumeGoTitleSelect.sprite");
		imgResumeGoTitleSelect->UpdateAnimation();
		imgResumeGoTitleSelect->SetPos({ 500, 575 });
		sprites.emplace_back(imgResumeGoTitleSelect.get());


		imgReallyBox = std::make_unique<Sprite>("Data/Texture/UserInterface/Pause/reallyBox.sprite");
		imgReallyBox->UpdateAnimation();
		imgReallyBox->SetPos({ 640, 300 });
		sprites.emplace_back(imgReallyBox.get());
		imgYesText = std::make_unique<Sprite>("Data/Texture/UserInterface/Pause/yesText.sprite");
		imgYesText->UpdateAnimation();
		imgYesText->SetPos({ 440, 400 });
		sprites.emplace_back(imgYesText.get());
		imgNoText = std::make_unique<Sprite>("Data/Texture/UserInterface/Pause/noText.sprite");
		imgNoText->UpdateAnimation();
		imgNoText->SetPos({ 840, 400 });
		sprites.emplace_back(imgNoText.get());
		imgYesNoSelect = std::make_unique<Sprite>("Data/Texture/UserInterface/Pause/yesNoSelect.sprite");
		imgYesNoSelect->UpdateAnimation();
		imgYesNoSelect->SetPos({ 840, 425 });
		sprites.emplace_back(imgYesNoSelect.get());
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
		Check,
	};
	UiPauseState state;

	std::vector<Sprite*> sprites;

	float pauseTimer = 0.0f; // ポーズ状態管理用タイマー
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
	std::unique_ptr<Sprite> imgResumeGoTitleSelect;

	enum class CheckResumeGoTitle
	{
		Resume,
		GoTitle,
		END,
	};
	int checkResumeGoTitleState;


	// check
	enum class CheckYesNo
	{
		Yes,
		No,
		END,
	};
	int checkYesNoState;

	std::unique_ptr<Sprite> imgReallyBox;
	std::unique_ptr<Sprite> imgYesText;
	std::unique_ptr<Sprite> imgNoText;
	std::unique_ptr<Sprite> imgYesNoSelect;
};

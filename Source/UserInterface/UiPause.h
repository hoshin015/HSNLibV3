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
	bool isPause = false;

	enum class UiPauseState
	{
		Hidden,
		Showing,
		Visible,
		Hide,
	};

	UiPauseState state;

	float pauseTimer     = 0.0f; // ポーズ状態管理用タイマー
	float pauseTotalTime = 1.0;  // ポーズUIの遷移完了までの時間

	
};

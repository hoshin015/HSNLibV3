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

private:
	// タイトルUI状態管理用
	enum class UiTitleState
	{
		Title,
		TitleToSelectMenu,
		SelectMenuToTitle,
		SelectMenu,
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

};

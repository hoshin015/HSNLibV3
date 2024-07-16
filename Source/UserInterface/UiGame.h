#pragma once
#include <memory>
#include "../../Library/2D/Sprite.h"
#include "../../Library/Easing.h"

class UiGame
{
private:
	UiGame()
	{
	};

	~UiGame()
	{
	};

public:
	static UiGame& Instance()
	{
		static UiGame instance;
		return instance;
	}

	// èâä˙âª
	void Initialize();

	// çXêV
	void Update();

	// ï`âÊ
	void Render();

	// debugGui
	void DrawDebugImGui();

private:
	// ëÃä≤ÉQÅ[ÉW
	std::unique_ptr<Sprite> somaGauge;
	std::unique_ptr<Sprite> somaGaugeBg;

	int somaMax = 100;
	int soma = 50;

	// hpÉQÅ[ÉW
	std::unique_ptr<Sprite> hpGauge;
	std::unique_ptr<Sprite> hpGaugeBg;
	std::unique_ptr<Sprite> injuryGauge;

	int hpMax = 100;
	int hp = 80;
	int injury = 90;

	// skillÉQÅ[ÉW
	std::unique_ptr<Sprite> skillGauge;
	std::unique_ptr<Sprite> skillGaugeBg;

	int skillMax = 100;
	int skill = 50;

	// HPÇO
	bool isDownOverFlag = false;
	float downTimer = 0.0f;
	float downTime = 4.0f;
	std::unique_ptr<Sprite> imgDownOver;
	Easing::EasingValue imgDownOverAlpha =
	{
		0.0f, 0.5f,
		0.0f,1.0f
	};
	Easing::EasingValue imgDownOverPosY =
	{
		0.0f, 0.5f,
		-100.0f, 360.0f
	};
	std::unique_ptr<Sprite> imgBlack;
	Easing::EasingValue imgBlackAlpha =
	{
		2.0f, 3.0f,
		0.0f,1.0f
	};
public:
	void OnDown();
};
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

	// ú»
	void Initialize();

	// XV
	void Update();

	// `æ
	void Render();

	// debugGui
	void DrawDebugImGui();

private:
	// Ì²Q[W
	std::unique_ptr<Sprite> somaGauge;
	std::unique_ptr<Sprite> somaGaugeBg;

	int somaMax = 100;
	int soma = 50;

	// hpQ[W
	std::unique_ptr<Sprite> hpGauge;
	std::unique_ptr<Sprite> hpGaugeBg;
	std::unique_ptr<Sprite> injuryGauge;
	std::unique_ptr<Sprite> hpGaugeLogo;

	int hpMax = 100;
	int hp = 80;
	int injury = 90;

	// skillQ[W
	std::unique_ptr<Sprite> skillGauge;
	std::unique_ptr<Sprite> skillGaugeBg;
	std::unique_ptr<Sprite> skillGaugeLogo;

	int skillMax = 100;
	int skill = 50;


	enum class State
	{
		BlackUp,
		BlackDown,
	};
	int state = static_cast<int>(State::BlackUp);

	// HPO
	bool isDownOverFlag = false;
	float downTimer = 0.0f;
	float downTime = 10.0f;
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
	Easing::EasingValue imgBlackAlphaUp =
	{
		9.0f, 10.0f,
		0.0f,1.0f
	};

	// 
	float blackDownTime = 5.0f;
	Easing::EasingValue imgBlackAlphaDown =
	{
		1.0f, 3.0f,
		1.0f,0.0f
	};



	// key
	std::unique_ptr<Sprite> imgKeyOption;
	std::unique_ptr<Sprite> imgKeyCameraMove;
	std::unique_ptr<Sprite> imgKeyLockOn;
	std::unique_ptr<Sprite> imgKeyMove;
	std::unique_ptr<Sprite> imgKeyAvoid;
	std::unique_ptr<Sprite> imgKeyAttack;

	// controller
	std::unique_ptr<Sprite> imgConOption;
	std::unique_ptr<Sprite> imgConCameraMove;
	std::unique_ptr<Sprite> imgConLockOn;
	std::unique_ptr<Sprite> imgConMove;
	std::unique_ptr<Sprite> imgConAvoid;
	std::unique_ptr<Sprite> imgConAttack;

public:
	void OnDown();

	// Q[ÌÔªè
	float gameTimer = 0.0f;
};
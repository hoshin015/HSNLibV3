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

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Render();

	// debugGui
	void DrawDebugImGui();

private:
	// 体幹ゲージ
	std::unique_ptr<Sprite> somaGauge;
	std::unique_ptr<Sprite> somaGaugeBg;

	int somaMax = 100;
	int soma = 50;

	// hpゲージ
	std::unique_ptr<Sprite> hpGauge;
	std::unique_ptr<Sprite> hpGaugeBg;
	std::unique_ptr<Sprite> injuryGauge;

	int hpMax = 100;
	int hp = 80;
	int injury = 90;

	// skillゲージ
	std::unique_ptr<Sprite> skillGauge;
	std::unique_ptr<Sprite> skillGaugeBg;

	int skillMax = 100;
	int skill = 50;
};
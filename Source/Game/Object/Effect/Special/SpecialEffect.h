#pragma once
#include "../../../../../Library/Easing.h"
#include "../../../../../Library/Graphics/RadialBlur.h"

class SpecialEffect
{
private:
	SpecialEffect()
	{
	}

	~SpecialEffect()
	{
	}

public:
	static SpecialEffect& Instance()
	{
		static SpecialEffect instance;
		return instance;
	}

	// 更新
	void Update(RadialBlur* radialBlur);

	// 描画
	void Render();

	// 実行
	void Emit();

private:
	bool isSpecialEffect = false; // 起動中かどうか

	enum class SpecialState
	{
		Init,       // 初期化
		firstNova,  // 最初の衝撃
		chargeNova, // 溜め
		End,        // 終了
	};

	SpecialState specialState;


	float lifeTimer = 0.0f;

	// firstNova
	float               firstNovaTime          = 2.0f;
	float               firstNovaRockStartTime = 1.0f;
	Easing::EasingValue firstNovaSamplingUp    =
	{
		0.0f, 1.0f,
		1.0f, 12.0f
	};
	Easing::EasingValue firstNovaSamplingDown =
	{
		1.0f, 2.0f,
		12.0f, 3.0f
	};
	Easing::EasingValue firstNovaColorUp =
	{
		0.5f, 1.5f,
		0.1f, 0.3f,
	};

	// chargeNova
	float               chargeNovaTime         = 5.0f;
	float               rockIntervalTimer      = 0.0f;
	float               rockIntervalTime       = 0.1f;
	Easing::EasingValue chargeNovaSamplingDown =
	{
		4.0f, 5.0f,
		3.0f, 1.0f
	};
};

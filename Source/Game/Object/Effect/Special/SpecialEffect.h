#pragma once
#include "../../../../../Library/Easing.h"
#include "../../../../../Library/Graphics/RadialBlur.h"
#include "../../../../../Library/Graphics/HeatHaze.h"
#include "../../../../Camera/CameraDerived.h"

#define SPECIAL_AUDIO_DELAY 0

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
	void Update(RadialBlur* radialBlur, HeatHaze* heatHaze);

	// 描画
	void Render();

	// 実行
	void Emit();

	// 岩生成
	void GenerateRock();
	// lightning 生成
	void GenerateLightning();

public:
	bool drawAfterStage = false;

private:
	bool isSpecialEffect = false; // 起動中かどうか

	// damage
	bool isDamaged = false;
	float damageRadius = 50.0f;
	float damage = 1000.0f;

	bool isPlayChargeSound = false;
	float playChargeSoundTime = 8.5f;
	
	enum class SpecialState
	{
		Init,       // 初期化
		firstNova,  // 最初の衝撃
		chargeNova, // 溜め
		Nova,		// 爆発
		UpMusic,	// サウンドアップ
		End,        // 終了
	};

	SpecialState specialState;


	float lifeTimer = 0.0f;

	// music
	Easing::EasingValue battle1DownSound =
	{
		0.0f, 1.0f,
		0.5f, 0.0f
	};

	// firstNova
	float               firstNovaTime          = 2.0f;
	float               firstNovaRockStartTime = 1.0f;
	Easing::EasingValue firstNovaSamplingUp    =
	{
		0.0f, 1.0f,
		0.0f, 0.1f
	};
	Easing::EasingValue firstNovaSamplingDown =
	{
		1.0f, 2.0f,
		0.1f, 0.0f
	};
	Easing::EasingValue firstNovaHeatHazeMaxShiftUp =
	{
		1.0f, 2.0f,
		0.0f, 0.02f
	};
	Easing::EasingValue firstNovaColorUp =
	{
		0.5f, 1.5f,
		0.2f, 0.4f,
	};

	// chargeNova
	float               chargeNovaTime         = 10.0f;
	float               rockIntervalTimer      = 0.0f;
	float               rockIntervalTime       = 0.1f;
	float               lightningIntervalTimer = 0.0f;
	float               lightningIntervalTime  = 0.05f;

	bool				isEndParticle = false;
	float				endParticleTime = 8.6f;

	bool				isEndRock = false;
	float				endRockStartTime = 9.0f;
	float				endRockEndTime = 9.3f;
	float				endRockGenTimer = 0.0f;
	float				endRockGenTime = 0.05f;

	bool				isEndNova = false;
	float				endNovaTime = 9.3f;

	float				afterStageTime = 9.4f;

	Easing::EasingValue chargeNovaHeatHazeMaxShiftDown =
	{
		8.5f, 10.0f,
		0.02f, 0.0f
	};
	Easing::EasingValue firstNovaColorDown =
	{
		8.5f, 10.0f,
		0.4f, 0.2f,
	};

	Easing::EasingValue endNovaBlurPowerUp =
	{
		8.5f, 9.5f,
		0.0f, 0.1f,
	};
	Easing::EasingValue endNovaBlurPowerDown =
	{
		9.5f, 10.0f,
		0.1f, 0.0f,
	};




	// soundUp
	float soundUpTime = 0.0f;
	Easing::EasingValue soundUpValue =
	{
		0.0f, 1.0f,
		0.0f, 0.5f,
	};
};

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

	// �X�V
	void Update(RadialBlur* radialBlur, HeatHaze* heatHaze, PlayerCamera* playerCamera);

	// �`��
	void Render();

	// ���s
	void Emit();

	// �␶��
	void GenerateRock();
	// lightning ����
	void GenerateLightning();

private:
	bool isSpecialEffect = false; // �N�������ǂ���

	bool isPlayChargeSound = false;
	float playChargeSoundTime = 8.5f;
	
	enum class SpecialState
	{
		Init,       // ������
		firstNova,  // �ŏ��̏Ռ�
		chargeNova, // ����
		Nova,		// ����
		UpMusic,	// �T�E���h�A�b�v
		End,        // �I��
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
		12.0f, 1.0f
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
	float               lightningIntervalTime  = 0.1f;

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







	// soundUp
	float soundUpTime = 0.0f;
	Easing::EasingValue soundUpValue =
	{
		0.0f, 1.0f,
		0.0f, 0.5f,
	};
};

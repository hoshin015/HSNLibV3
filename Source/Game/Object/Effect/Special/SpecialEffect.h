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

	// �X�V
	void Update(RadialBlur* radialBlur);

	// �`��
	void Render();

	// ���s
	void Emit();

private:
	bool isSpecialEffect = false; // �N�������ǂ���

	enum class SpecialState
	{
		Init,       // ������
		firstNova,  // �ŏ��̏Ռ�
		chargeNova, // ����
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

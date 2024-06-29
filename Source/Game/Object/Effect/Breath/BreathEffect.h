#pragma once

#include "../../Library/Easing.h"
#include "BreathMesh.h"

// BreathEffect �܂Ƃ߂�N���X
class BreathEffect
{
	BreathEffect()
	{
	}

	~BreathEffect()
	{
	}
public:
	static BreathEffect& Instance()
	{
		static BreathEffect instance;
		return instance;
	}

	// BreathData �𐶐�
	void Initialize();

	// BreathData �̒��g���܂Ƃ߂čX�V
	void Update();
	// BreathData �̒��g���܂Ƃ߂ĕ`��
	void Render(bool isShadow = false);

	// lightning ����
	void LightningGenerateUpdate();

private:
	// area �I�u�W�F�N�g
	std::unique_ptr<BreathArea> areaObject;
	// crossPlane �I�u�W�F�N�g
	std::unique_ptr<BreathCrossPlane> crossPlaneObject;
	// cylinder �I�u�W�F�N�g
	std::unique_ptr<BreathCylinder> cylinderObject;

	float lifeTimer = 0.0f;
	float lifeTime = 4.0f;

	float endTime = 2.8f;
};


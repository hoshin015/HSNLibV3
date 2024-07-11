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

	void SetPosition(const DirectX::XMFLOAT3& position);
	void SetAngle(const float angle) { this->angle.y = angle; }

	// BreathData �𐶐�
	void Initialize();

	// BreathData �̒��g���܂Ƃ߂čX�V
	void Update();
	// BreathData �̒��g���܂Ƃ߂ĕ`��
	void Render(bool isShadow = false);

	// lightning ����
	void LightningGenerateUpdate();



	// �u���X�Ăяo��
	void Emit();

private:
	// area �I�u�W�F�N�g
	std::unique_ptr<BreathArea> areaObject;
	// crossPlane �I�u�W�F�N�g
	std::unique_ptr<BreathCrossPlane> crossPlaneObject;
	// cylinder �I�u�W�F�N�g
	std::unique_ptr<BreathCylinder> cylinderObject;

	bool isBreath = false;

	float lifeTimer = 0.0f;
	float lifeTime  = 3.0f;

	float endTime = 2.8f;

	float lightningTimer = 0.0f;
	float lightningTime  = 0.01f;

	DirectX::XMFLOAT3 position = {0, 0, 0};
	DirectX::XMFLOAT3 angle    = {0, 90, 90};
	DirectX::XMFLOAT4	orientation = {};
};

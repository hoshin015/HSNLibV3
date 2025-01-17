#pragma once

#include "../../Library/Easing.h"
#include "BreathMesh.h"

// BreathEffect まとめるクラス
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

	// BreathData を生成
	void Initialize();

	// BreathData の中身をまとめて更新
	void Update();
	// BreathData の中身をまとめて描画
	void Render(bool isShadow = false);

	// lightning 生成
	void LightningGenerateUpdate();



	// ブレス呼び出し
	void Emit();

private:
	// area オブジェクト
	std::unique_ptr<BreathArea> areaObject;
	// crossPlane オブジェクト
	std::unique_ptr<BreathCrossPlane> crossPlaneObject;
	// cylinder オブジェクト
	std::unique_ptr<BreathCylinder> cylinderObject;

	bool isBreath = false;

	float lifeTimer = 0.0f;
	float lifeTime  = 4.0f;

	float endTime = 1.7f;

	float lightningTimer = 0.0f;
	float lightningTime  = 0.0025f;

	Easing::EasingValue lightningNum =
	{
		1.4, 1.7,
		2.0f, 0.0f,
	};

	DirectX::XMFLOAT3 position = {0, 0, 0};
	DirectX::XMFLOAT3 angle    = {0, 90, 90};
	DirectX::XMFLOAT4	orientation = {};

	// --- ダメージ判定 ---
	float damageTimeStart;
	float damageTimeEnd;
	float damageRadius;
	float damage;
	bool isDamaged = false;
};

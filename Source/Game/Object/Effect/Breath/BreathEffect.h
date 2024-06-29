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

	// BreathData を生成
	void Initialize();

	// BreathData の中身をまとめて更新
	void Update();
	// BreathData の中身をまとめて描画
	void Render(bool isShadow = false);

	// lightning 生成
	void LightningGenerateUpdate();

private:
	// area オブジェクト
	std::unique_ptr<BreathArea> areaObject;
	// crossPlane オブジェクト
	std::unique_ptr<BreathCrossPlane> crossPlaneObject;
	// cylinder オブジェクト
	std::unique_ptr<BreathCylinder> cylinderObject;

	float lifeTimer = 0.0f;
	float lifeTime = 4.0f;

	float endTime = 2.8f;
};


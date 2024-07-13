#pragma once
#include <DirectXMath.h>
#include <string>
#include <vector>

class EffectDamageManager
{
private:
	EffectDamageManager(){}
	~EffectDamageManager(){}

public:
	static EffectDamageManager& Instance()
	{
		static EffectDamageManager instance;
		return instance;
	}

	// エフェクト用　Collision データ
	struct EffectCollision
	{
		struct SphereData
		{
			DirectX::XMFLOAT3 position;
			float radius;
			float damage;
		};
		std::vector<SphereData> spheres;
	};
	

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Render();

	// 全削除
	void Clear();

	// 登録
	void Register(EffectCollision effectCollision);

private:
	std::vector<EffectCollision> effectCollisions;
};
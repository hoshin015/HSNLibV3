#pragma once

#include "../../Base/StaticObject.h"
#include "../../Library/Easing.h"


class RockMainMesh;

// 1つ1つの情報
class RockData : public BaseObject
{
public:
	RockData()
	{
		funcs.push_back(&RockData::RockDefaultUpdate);
		funcs.push_back(&RockData::RockUpUpdate);
	}

	~RockData() override
	{
	}

	void Update() override;

	void Render(bool isShadow = false) override
	{
	};

	void UpdateTransform();

	// 関数ポインタを使った関数
	enum class RockFuncEnum
	{
		Default,
		Up,
	};

	typedef void (RockData::*UpdateFunc)();
	std::vector<UpdateFunc>       funcs;
	void                          RockDefaultUpdate();
	void                          RockUpUpdate();

private:
	DirectX::XMMATRIX MSC;

	float lifeTime;
	float lifeTimer = 0.0f;

	float             emissivePower = 1.0f;
	DirectX::XMFLOAT4 color         = {1, 1, 1, 1};

	DirectX::XMFLOAT3 velocity = { 0,0,0 };
	float gravity = 15.0f;

	RockMainMesh* owner;

	RockFuncEnum updateType = RockData::RockFuncEnum::Default;

	Easing::EasingValue val1;

public:
	// lifeTime
	void SetLifeTime(float lt) { lifeTime = lt; }

	// velocity
	void SetVeloicty(DirectX::XMFLOAT3 v) { velocity = v; }
	DirectX::XMFLOAT3 GetVelocity() { return velocity; }

	// onwer
	void          SetOwner(RockMainMesh* o) { owner = o; }
	RockMainMesh* GetOwner() { return owner; }

	// emissivePower
	float GetEmissivePower() { return emissivePower; }
	void  SetEmissivePower(float ep) { this->emissivePower = ep; }

	// color
	DirectX::XMFLOAT4 GetColor() { return color; }
	void              SetColor(DirectX::XMFLOAT4 c) { color = c; }

	// msc
	void SetMSC(DirectX::XMMATRIX MSC){ this->MSC = MSC; };

	// updateType
	void SetUpdateType(RockFuncEnum t) { updateType = t; }

	// val1
	void SetVal1(Easing::EasingValue v) { val1 = v; }

	// gravity
	void SetGravity(float g) { gravity = g; }

public:
	bool isDamaged = false;
	float radius = 2.0f;
	float damage = 10.0f;
};

// Rock まとめるクラス
class RockMainMesh : public StaticObject
{
public:
	RockMainMesh(const char* filePath) : StaticObject(filePath)
	{
	}

	~RockMainMesh() override
	{
	}

	// RockData を生成
	void Initialize();

	// RockData の中身をまとめて更新
	void Update() override;
	// RockData の中身をまとめて描画
	void Render(bool isShadow = false) override;

	// 登録
	void Register(RockData* rockData);
	// 解除
	void Remove(RockData* rockData);
	// 全削除
	void Clear();

protected:
	std::vector<RockData*> rockInfo;
	std::set<RockData*>    removes;

	DirectX::XMFLOAT4X4 rockTransforms[MAX_INSTANCE] = {}; // 姿勢行列配列
	float               rockEmissives[MAX_INSTANCE]  = {}; // エミッシブ力配列
	DirectX::XMFLOAT4   rockColors[MAX_INSTANCE]     = {}; // カラー配列
	DirectX::XMFLOAT4   rockUvScrolls[MAX_INSTANCE]     = {}; // uvScroll配列
};

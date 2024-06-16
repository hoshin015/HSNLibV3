#pragma once

#include "../Base/StaticObject.h"
#include "../../Library/Easing.h"


class LightningMainMesh;

// 1つ1つの情報
class LightningData : public BaseObject
{
public:
	LightningData()
	{
		funcs.push_back(&LightningData::LightningNoUpdate);
		funcs.push_back(&LightningData::LightningAreaUpdate);
		funcs.push_back(&LightningData::LightningMainUpdate);
	}

	~LightningData() override
	{
	}

	void Update() override;

	void Render(bool isShadow = false) override
	{
	};

	void UpdateTransform();

	// 関数ポインタを使った関数
	enum class LightningFuncEnum
	{
		NO,
		Area,
		Main,
		END,
	};

	typedef void (LightningData::*UpdateFunc)();
	std::vector<UpdateFunc>       funcs;
	void                          LightningNoUpdate();
	void                          LightningAreaUpdate();
	void                          LightningMainUpdate();

private:
	DirectX::XMMATRIX MSC;

	float lifeTime;
	float lifeTimer = 0.0f;

	float             emissivePower = 1.0f;
	DirectX::XMFLOAT4 color         = {1, 1, 1, 1};

	LightningMainMesh* owner;

	LightningFuncEnum updateType = LightningFuncEnum::NO;

public:
	// lifeTime
	void SetLifeTime(float lt) { lifeTime = lt; }

	// onwer
	void               SetOwner(LightningMainMesh* o) { owner = o; }
	LightningMainMesh* GetOwner() { return owner; }

	// emissivePower
	float GetEmissivePower() { return emissivePower; }
	void  SetEmissivePower(float ep) { this->emissivePower = ep; }

	// color
	DirectX::XMFLOAT4 GetColor() { return color; }
	void              SetColor(DirectX::XMFLOAT4 c) { color = c; }

	// msc
	void SetMSC(DirectX::XMMATRIX MSC);

	// updateType
	void SetUpdateType(LightningFuncEnum t) { updateType = t; }
};

// lightning まとめるクラス
class LightningMainMesh : public StaticObject
{
public:
	LightningMainMesh(const char* filePath) : StaticObject(filePath)
	{
	}

	~LightningMainMesh() override
	{
	}

	// lightningData を生成
	void Initialize();

	// lightningData の中身をまとめて更新
	void Update() override;
	// lightningData の中身をまとめて描画
	void Render(bool isShadow = false) override;

	// 登録
	void Register(LightningData* lightningData);
	// 解除
	void Remove(LightningData* lightningData);
	// 全削除
	void Clear();

protected:
	std::vector<LightningData*> lightningInfo;
	std::set<LightningData*>    removes;

	DirectX::XMFLOAT4X4 lightningTransforms[MAX_INSTANCE] = {}; // 姿勢行列配列
	float               lightningEmissives[MAX_INSTANCE]  = {}; // エミッシブ力配列
	DirectX::XMFLOAT4   lightningColors[MAX_INSTANCE]     = {}; // カラー配列
};


class LightningMesh1 : public LightningMainMesh
{
public:
	LightningMesh1(const char* filePath) : LightningMainMesh(filePath)
	{
	}

	~LightningMesh1() override
	{
	}
};

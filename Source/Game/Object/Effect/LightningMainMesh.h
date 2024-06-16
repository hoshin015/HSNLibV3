#pragma once

#include "../Base/StaticObject.h"
#include "../../Library/Easing.h"

class LightningMainMeshChildParent;

// lightning をまとめて扱うためのクラス
class LightningMainMesh : public StaticObject
{
public:
	LightningMainMesh(const char* filePath) : StaticObject(filePath), path(filePath){}
	~LightningMainMesh() override {}

	// lightningData を生成
	void Initialize();

	// lightningData の中身をまとめて更新
	void Update() override;
	// lightningData の中身をまとめて描画
	void Render(bool isShadow = false) override;

private:
	std::string path = "";

	DirectX::XMFLOAT4X4 lightning1Transforms[MAX_INSTANCE] = {};	// 姿勢行列配列
	float lightning1Emissives[MAX_INSTANCE] = {};					// エミッシブ力配列

	std::vector<LightningMainMeshChildParent*> lightningData;



	float lightningMainTimer = 0.0f;	// lightning の全体を管理するタイマー
	float lightningMainTime = 5.0f;		// lightning の生存時間

	// 各エミッタ生成時間
	float lightningMainMeshChild1GenerateTime = 0.6f;
	bool IsLightningMainMeshChild1Generate = true;

	float lightningMainMeshChild2GenerateTime = 0.0f;
	bool IsLightningMainMeshChild2Generate = true;

	float lightningMainMeshChild3GenerateTime = 0.3f;
	bool IsLightningMainMeshChild3Generate = true;
};


// lightning １つ１つの情報
class LightningMainMeshChildParent : public StaticObject
{
public:
	LightningMainMeshChildParent(const char* filePath) : StaticObject(filePath) {}
	virtual ~LightningMainMeshChildParent() = default;

	// 更新
	void Update() = 0;

	// 描画(override の必要があるからしているが使用しない)
	void Render(bool isShadow = false) override{};

protected:
	float lifeTimer = 0.0f;
	float lifeTime = 2.0f;
	float emissivePower = 1.0f;

public:
	float GetlifeTimer() { return lifeTimer; }
	void SetLifeTimer(float t) { lifeTimer = t; }

	float GetlifeTime() { return lifeTime; }
	void SetLifeTime(float t) { lifeTime = t; }

	float GetEmissivePower() { return emissivePower; }
	void SetEmissivePower(float e) { emissivePower = e; }
};

class LightningMainMeshChild1 : public LightningMainMeshChildParent
{
public:
	LightningMainMeshChild1(const char* filePath) : LightningMainMeshChildParent(filePath){}
	~LightningMainMeshChild1()override {}

	// 更新
	void Update() override;

private:
	Easing::EasingValue lightning1Scale =
	{
		0.0f, 0.5f,
		4.0f, 0.5f,
	};
};

class LightningMainMeshChild2 : public LightningMainMeshChildParent
{
public:
	LightningMainMeshChild2(const char* filePath) : LightningMainMeshChildParent(filePath) { lifeTime = 0.2f; }
	~LightningMainMeshChild2()override {}

	// 更新
	void Update() override;
};
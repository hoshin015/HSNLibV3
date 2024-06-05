#pragma once

#include "../Base/StaticObject.h"

class LightningMainMeshChild;

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
	DirectX::XMFLOAT4X4 m[999] = {};

	std::vector<LightningMainMeshChild*> lightningData;
};


// lightning １つ１つの情報
class LightningMainMeshChild : public StaticObject
{
public:
	LightningMainMeshChild(const char* filePath) : StaticObject(filePath) {}
	~LightningMainMeshChild()override {}

	// 更新
	void Update() override;

	// 描画(override の必要があるからしているが使用しない)
	void Render(bool isShadow = false) override{};

private:
	float lifeTimer = 2.0f;

public:
	float GetlifeTimer() { return lifeTimer; }
	void SetLifeTimer(float t) { lifeTimer = t; }
};
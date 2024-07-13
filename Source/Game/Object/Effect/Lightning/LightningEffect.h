#pragma once
#include <memory>
#include "LightningMainMesh.h"

class LightningEffect
{
private:
	LightningEffect(){}
	~LightningEffect(){}

public:
	static LightningEffect& Instance()
	{
		static LightningEffect instance;
		return instance;
	}

	// 初期化
	void Initialize();

	// 更新
	void Update();
	// 描画
	void Render(bool isShadow = false);

	// ライトニング追加
	void Emit(DirectX::XMFLOAT3 pos);

	void PlayLightningSound();

	struct LightningEmitter
	{
		DirectX::XMFLOAT3 position;
		float timer = 0.0f;
		float destoryTime = 10.0f;
		bool addLightning0 = false;
		bool addLightning1 = false;
		bool addLightning2 = false;
		bool addLightning3 = false;
	};


	std::unique_ptr<LightningMesh1> lightningMesh1;
	std::unique_ptr<LightningMesh1> lightningMesh2;
	std::unique_ptr<LightningMesh1> lightningMesh3;
	std::unique_ptr<LightningMesh1> lightningMesh4;
	std::unique_ptr<LightningMesh1> lightningMesh5;
	std::unique_ptr<LightningMesh1> lightningMesh6;
	std::unique_ptr<LightningMesh1> lightningMesh7;
	std::unique_ptr<LightningMesh1> lightningMesh8;
	std::unique_ptr<LightningMesh1> lightningMesh9;
private:
	float lifeTime = 10.0f;

	float lightning0GenerateTime = 1.15f;
	float lightning1GenerateTime = 1.2f;
	float lightning2GenerateTime = 1.25f;
	float lightning3GenerateTime = 1.3f;

	float bottomTime = 0.01f;
	float bottomTimer = 0.0f;

	bool isSound = false;

	std::vector<LightningEmitter> lightningEmitters;

	// ダメージ判定を出す時間
	float damageTimeStart;
	float damageTimeEnd;
	float damageRadius;
	float damage;
};
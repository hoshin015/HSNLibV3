#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>


class Emitter
{
public:
	Emitter();

	~Emitter()
	{
	}

	// 更新
	void Update();

public:
	// エミッターの情報
	struct EmitterData
	{
		float duration;    // パーティクルの生成時間(この時間だけエミッターが機能する)
		bool  looping;     // duration を繰り返すかどうか
		float burstsTime;  // パーティクルの生成間隔(s)
		float burstsCount; // 一度に生成するパーティクル数

		// 生成するパーティクルの情報(パーティクル初期化時に必要な情報)
		float             particleLifeTimeMin;                // 生成するパーティクルの寿命(最小値)
		float             particleLifeTimeMax;                // 生成するパーティクルの寿命(最大値)
		float             particleSpeedMin;                   // 生成するパーティクルの初速(最小値)
		float             particleSpeedMax;                   // 生成するパーティクルの初速(最大値)
		DirectX::XMFLOAT2 particleSizeMin;                    // 生成するパーティクルのサイズ(最小値)
		DirectX::XMFLOAT2 particleSizeMax;                    // 生成するパーティクルのサイズ(最大値)
		DirectX::XMFLOAT4 particleColorMin    = {1, 1, 1, 1}; // 生成するパーティクルのカラー(最小値)
		DirectX::XMFLOAT4 particleColorMax    = {1, 1, 1, 1}; // 生成するパーティクルのカラー(最大値)
		float             particleFrictionMin = 0.0f;         // 抵抗(最小値)
		float             particleFrictionMax = 0.0f;         // 抵抗(最大値)
		float             particleGravity     = 0.0f;         // 重力(下方向の力の大きさ)
		float             particleAddAngle    = 0.0f;         // １秒間に回転する角度

		float particleKind;              // 生成するパーティクルのタイプ
		float particleBillboardType = 0; // billboardのタイプ
	};

	EmitterData emitterData;


	DirectX::XMFLOAT3 position = {0, 0, 0}; // エミッターのポジション
	DirectX::XMFLOAT3 velocity = {0, 0, 0}; // 生成するパーティクルの velocity (正規化して speed を乗算する)

	float emitRateTimer; // パーティクルの生成間隔を管理する用のタイマー
	float emitLifeTimer; // 生成された経過時間を持つ

	float emitCount;	// 生成した回数

	// 定数バッファ用
	struct EmitterConstants
	{
		DirectX::XMFLOAT2 particleSizeMin;
		DirectX::XMFLOAT2 particleSizeMax;
		DirectX::XMFLOAT4 particleColorMin;
		DirectX::XMFLOAT4 particleColorMax;
		float             particleSpeedMin;
		float             particleSpeedMax;
		float             particleLifeTimeMin;
		float             particleLifeTimeMax;
		float             particleKind;
		float             particleFrictionMin;
		float             particleFrictionMax;
		float             particleAddAngle;
		float             particleGravity;
		float             particleBillboardType;
		DirectX::XMFLOAT2 emitterPad1;
		DirectX::XMFLOAT3 emitterPosition;
		float             emitCount;
	};

	EmitterConstants emitterConstant;

	Microsoft::WRL::ComPtr<ID3D11Buffer> emitterConstantBuffer;
};

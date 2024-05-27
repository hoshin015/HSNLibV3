#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>


class Emitter
{
public:
	Emitter();
	~Emitter() {}

	// 更新
	void Update();

public:
	float				duration;					// パーティクルの生成時間(この時間だけエミッターが機能する)
	bool				looping;					// duration を繰り返すかどうか
	float				startLifeTime;				// 生成するパーティクルの寿命
	float				startSpeed;					// 生成するパーティクルの初速	
	float				startSize;					// 生成するパーティクルのサイズ(後々 x,y,z 独立させる)
	DirectX::XMFLOAT4	startColor = { 1,1,1,1 };	// 生成するパーティクルのカラー
	float				startKind;					// 生成するパーティクルのタイプ
	
	float rateOverTime;	// パーティクルの生成間隔(s)
	float rate;			// 一度に生成するパーティクル数
	

	DirectX::XMFLOAT3	position = { 0,0,0 };	// エミッターのポジション
	DirectX::XMFLOAT3	velocity = { 0,0,0 };

	float emitRateTimer;	// パーティクルの生成間隔を管理する用のタイマー
	float emitLifeTimer;	// 生成された経過時間を持つ

	// 定数バッファ用
	struct EmitterConstants
	{
		float particleSize;
		float particleLifeTime;
		float particleKind;
		float pad1;
		DirectX::XMFLOAT4 particleColor;
		DirectX::XMFLOAT3 emitterPosition;
		float pad2;
	};
	EmitterConstants emitterConstant;

	Microsoft::WRL::ComPtr<ID3D11Buffer> emitterConstantBuffer;
};
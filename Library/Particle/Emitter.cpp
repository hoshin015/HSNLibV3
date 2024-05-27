#include "Emitter.h"
#include "Particle.h"
#include "../Timer.h"
#include "../RegisterNum.h"
#include "../ErrorLogger.h"
#include "../Graphics/Graphics.h"
#include "EmitterManager.h"

// コンストラクタ
Emitter::Emitter()
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11Device* device = gfx->GetDevice();

	// 定数バッファ作成
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(EmitterConstants);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	HRESULT hr = device->CreateBuffer(&bufferDesc, nullptr, emitterConstantBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));


	rate = 16;
	duration = 5;
	looping = true;
	rateOverTime = 0.01f;

	// 生成パーティクル設定
	startColor = { 0.5,2,2,1 };
	startLifeTime = 0.2;
	startSize = 0.05f;
	startKind = 0;
	emitterConstant.particleColor = startColor;
	emitterConstant.particleLifeTime = startLifeTime;
	emitterConstant.particleSize = startSize;
	emitterConstant.particleKind = startKind;
}

// 更新
void Emitter::Update()
{
	if (emitLifeTimer > duration)
	{
		if (looping)
		{
			// ループ処理
			emitLifeTimer = 0;
		}
		else
		{
			// エミッター破棄処理
			EmitterManager::Instance().Remove(this);
			
			// 一旦return
			return;
		}
	}
	
	while (emitRateTimer > rateOverTime)
	{
		// エミッター定数バッファ更新
		Graphics* gfx = &Graphics::Instance();
		ID3D11DeviceContext* dc = gfx->GetDeviceContext();

		// このエミッターの値をいれてあげる
		emitterConstant.emitterPosition = position;
		emitterConstant.particleColor = startColor;
		emitterConstant.particleLifeTime = startLifeTime;
		emitterConstant.particleSize = startSize;
		emitterConstant.particleKind = startKind;
		dc->UpdateSubresource(emitterConstantBuffer.Get(), 0, 0, &emitterConstant, 0, 0);
		dc->CSSetConstantBuffers(_emitterConstant, 1, emitterConstantBuffer.GetAddressOf());
		

		// パーティクルの生成
		Particle::Instance().Instance().Emit(rate);

		emitRateTimer -= rateOverTime;
	}

	// パーティクルの生成間隔管理タイマー加算
	emitRateTimer += Timer::Instance().DeltaTime();
	// エミッターの経過時間加算
	emitLifeTimer += Timer::Instance().DeltaTime();
}

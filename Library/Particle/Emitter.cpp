#include "Emitter.h"
#include "EmitterManager.h"
#include "Particle.h"
#include "../Timer.h"
#include "../RegisterNum.h"
#include "../ErrorLogger.h"
#include "../Graphics/Graphics.h"
#include "../Math/Math.h"

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

	// emitterData 初期化 (外部から読み込めるようにしたい)
	emitterData.duration = 5;
	emitterData.looping = true;
	emitterData.burstsTime = 0.01f;
	emitterData.burstsCount = 16;

	
}

// 更新
void Emitter::Update()
{
	if (emitLifeTimer > emitterData.duration)
	{
		if (emitterData.looping)
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
	
	while (emitRateTimer > emitterData.burstsTime)
	{
		// エミッター定数バッファ更新
		Graphics* gfx = &Graphics::Instance();
		ID3D11DeviceContext* dc = gfx->GetDeviceContext();

		// このエミッターの値をいれてあげる
		emitterConstant.emitterPosition = position;

		// 生成パーティクル設定
		emitterConstant.particleColor.x = Math::RandomRange(emitterData.particleColorMin.x, emitterData.particleColorMax.x);
		emitterConstant.particleColor.y = Math::RandomRange(emitterData.particleColorMin.y, emitterData.particleColorMax.y);
		emitterConstant.particleColor.z = Math::RandomRange(emitterData.particleColorMin.z, emitterData.particleColorMax.z);
		emitterConstant.particleColor.w = Math::RandomRange(emitterData.particleColorMin.w, emitterData.particleColorMax.w);

		emitterConstant.particleLifeTime = Math::RandomRange(emitterData.particleLifeTimeMin, emitterData.particleLifeTimeMax);
		emitterConstant.particleSize.x = Math::RandomRange(emitterData.particleSizeMin.x, emitterData.particleSizeMax.x);
		emitterConstant.particleSize.y = Math::RandomRange(emitterData.particleSizeMin.y, emitterData.particleSizeMax.y);
		emitterConstant.particleFriction = Math::RandomRange(emitterData.particleFrictionMin, emitterData.particleFrictionMax);
		emitterConstant.particleAddAngle = emitterData.particleAddAngle;
		emitterConstant.particleGravity = emitterData.particleGravity;

		emitterConstant.particleKind = emitterData.particleStartKind;
		emitterConstant.particleBillboardType = emitterData.particleBillboardType;
		dc->UpdateSubresource(emitterConstantBuffer.Get(), 0, 0, &emitterConstant, 0, 0);
		dc->CSSetConstantBuffers(_emitterConstant, 1, emitterConstantBuffer.GetAddressOf());
		
		// パーティクルの生成
		Particle::Instance().Instance().Emit(emitterData.burstsCount);

		emitRateTimer -= emitterData.burstsTime;
	}

	// パーティクルの生成間隔管理タイマー加算
	emitRateTimer += Timer::Instance().DeltaTime();
	// エミッターの経過時間加算
	emitLifeTimer += Timer::Instance().DeltaTime();
}

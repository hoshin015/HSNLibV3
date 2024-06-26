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
	std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex()); // 排他制御

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
	while (emitRateTimer >= emitterData.burstsTime)
	{
		// エミッター定数バッファ更新
		Graphics* gfx = &Graphics::Instance();
		ID3D11DeviceContext* dc = gfx->GetDeviceContext();

		// このエミッターの値をいれてあげる
		emitterConstant.emitterPosition = position;
		emitterConstant.emitCount = emitCount;

		emitterConstant.particleSpeedMin = emitterData.particleSpeedMin;
		emitterConstant.particleSpeedMax = emitterData.particleSpeedMax;
		emitterConstant.particleColorMin = emitterData.particleColorMin;
		emitterConstant.particleColorMax = emitterData.particleColorMax;
		emitterConstant.particleLifeTimeMin = emitterData.particleLifeTimeMin;
		emitterConstant.particleLifeTimeMax = emitterData.particleLifeTimeMax;
		emitterConstant.particleSizeMin = emitterData.particleSizeMin;
		emitterConstant.particleSizeMax = emitterData.particleSizeMax;
		emitterConstant.particleAngleMin = emitterData.particleAngleMin;
		emitterConstant.particleAngleMax = emitterData.particleAngleMax;
		emitterConstant.particleAddAngle = emitterData.particleAddAngle;
		emitterConstant.particleGravity = emitterData.particleGravity;

		emitterConstant.particleKind = emitterData.particleKind;
		emitterConstant.particleBillboardType = emitterData.particleBillboardType;
		emitterConstant.particleTextureType = emitterData.particleTextureType;

		dc->UpdateSubresource(emitterConstantBuffer.Get(), 0, 0, &emitterConstant, 0, 0);
		dc->CSSetConstantBuffers(_emitterConstant, 1, emitterConstantBuffer.GetAddressOf());
		
		// パーティクルの生成
		Particle::Instance().Instance().Emit(emitterData.burstsCount);

		emitCount += Math::RandomRange(0.01, 1.0);
		emitRateTimer -= emitterData.burstsTime;


		if(emitterData.burstsOneShot > 0)
		{
			emitterData.burstsOneShot--;
			if (emitterData.burstsOneShot == 0)
			{
				// エミッター破棄処理
				EmitterManager::Instance().Remove(this);
				return;
			}
		}
	}

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

	// パーティクルの生成間隔管理タイマー加算
	emitRateTimer += Timer::Instance().DeltaTime();
	// エミッターの経過時間加算
	emitLifeTimer += Timer::Instance().DeltaTime();
}

#include "SpecialEffect.h"
#include "../../../../../Library/Particle/EmitterManager.h"
#include "../../../../../Library/RegisterNum.h"
#include "../../../../../Library/Timer.h"
#include "../Rock/RockEffect.h"
#include "../../../../../Library/Math/Math.h"
#include "../../../../../Library/3D/LightManager.h"

// 更新
void SpecialEffect::Update(RadialBlur* radialBlur)
{
	if (!isSpecialEffect) return;

	float deltaTime = Timer::Instance().DeltaTime();
	lifeTimer += deltaTime;

	switch (specialState)
	{
	case SpecialState::Init:
		{
			// --- パーティクル ---
			{
				Emitter* emitter                           = new Emitter();
				emitter->position                          = {0, 0, 0};
				emitter->emitterData.duration              = 3.0;
				emitter->emitterData.looping               = false;
				emitter->emitterData.burstsTime            = 0.05;
				emitter->emitterData.burstsCount           = 36;
				emitter->emitterData.particleKind          = pk_novaBurst;
				emitter->emitterData.particleLifeTimeMin   = 3.0f;
				emitter->emitterData.particleLifeTimeMax   = 4.0f;
				emitter->emitterData.particleSpeedMin      = 12.0f;
				emitter->emitterData.particleSpeedMax      = 36.0f;
				emitter->emitterData.particleSizeMin       = {0.1f, 0.1f};
				emitter->emitterData.particleSizeMax       = {0.3f, 0.3f};
				emitter->emitterData.particleColorMin      = {6.0, 0.8, 0.8, 1};
				emitter->emitterData.particleColorMax      = {10.0, 0.8, 0.8, 1};
				emitter->emitterData.particleGravity       = 0;
				emitter->emitterData.particleBillboardType = 0;
				emitter->emitterData.particleTextureType   = 0;
				emitter->emitterData.burstsOneShot         = 6;
				EmitterManager::Instance().Register(emitter);
			}


			// --- 炎 ---
			{
				Emitter* emitter                           = new Emitter();
				emitter->position                          = {0, 0, 0};
				emitter->emitterData.duration              = 3.0;
				emitter->emitterData.looping               = false;
				emitter->emitterData.burstsTime            = 0.1;
				emitter->emitterData.burstsCount           = 72;
				emitter->emitterData.particleKind          = pk_novaStartFire;
				emitter->emitterData.particleLifeTimeMin   = 1.5f;
				emitter->emitterData.particleLifeTimeMax   = 2.0f;
				emitter->emitterData.particleSpeedMin      = 80.0f;
				emitter->emitterData.particleSpeedMax      = 90.0f;
				emitter->emitterData.particleSizeMin       = {37.0f, 37.0f};
				emitter->emitterData.particleSizeMax       = {38.0f, 38.0f};
				emitter->emitterData.particleColorMin      = {5.0, 0.9, 0.9, 1};
				emitter->emitterData.particleColorMax      = {5.0, 1.0, 1.0, 1};
				emitter->emitterData.particleGravity       = 0;
				emitter->emitterData.particleBillboardType = 0;
				emitter->emitterData.particleTextureType   = 9;
				emitter->emitterData.burstsOneShot         = 1;
				EmitterManager::Instance().Register(emitter);
			}

			// --- radialBlur ON ---
			radialBlur->SetIsRadial(true);
			radialBlur->SetSamplingCount(0.0f);
			radialBlur->SetBlurPower(0.02f);

			// --- rock ---
			rockIntervalTimer = 0.0f;

			// --- smoke ---
			{
				Emitter* emitter = new Emitter();
				emitter->position = { 0, 0, 0 };
				emitter->emitterData.duration = 5.0;
				emitter->emitterData.looping = false;
				emitter->emitterData.burstsTime = 0.3;
				emitter->emitterData.burstsCount = 3;
				emitter->emitterData.particleKind = pk_smoke;
				emitter->emitterData.particleLifeTimeMin = 1.0f;
				emitter->emitterData.particleLifeTimeMax = 4.0f;
				emitter->emitterData.particleSpeedMin = 1.0f;
				emitter->emitterData.particleSpeedMax = 60.0f;
				emitter->emitterData.particleSizeMin = { 30.0f, 30.0f };
				emitter->emitterData.particleSizeMax = { 50.0f, 50.0f };
				emitter->emitterData.particleColorMin = { 2.9, 0.9, 0.9, 1 };
				emitter->emitterData.particleColorMax = { 3.0, 1.0, 1.0, 1 };
				emitter->emitterData.particleGravity = 0;
				emitter->emitterData.particleBillboardType = 0;
				emitter->emitterData.particleTextureType = 6;
				EmitterManager::Instance().Register(emitter);
			}

			// ステート更新
			specialState = SpecialState::firstNova;
		}
	//[[fallthrough]]
	case SpecialState::firstNova:
		{ 
			// --- radialBlur ---
			float sampCount;
			sampCount = Easing::GetNowParam(Easing::OutQuad<float>, lifeTimer, firstNovaSamplingUp);
			if (firstNovaSamplingDown.startTime < lifeTimer)
			{
				sampCount = Easing::GetNowParam(Easing::OutQuad<float>, lifeTimer, firstNovaSamplingDown);
			}
			radialBlur->SetSamplingCount(static_cast<float>(static_cast<int>(sampCount)));

			// --- rock ---
			if(lifeTimer > firstNovaRockStartTime)
			{
				rockIntervalTimer += deltaTime;
				while (rockIntervalTimer > rockIntervalTime)
				{
					for (int i = 0; i < 3; i++)
					{
						DirectX::XMFLOAT3 rPos = { (rand() % 30) - 15.0f, 0, rand() % 30 - 15.0f };
						DirectX::XMFLOAT3 rVec = { (rand() % 3) - 1.5f, rand() % 2 + 1.0f, rand() % 3 - 1.5f};

						RockData* rock = new RockData();
						rock->SetPos(rPos);
						rock->SetVeloicty(rVec);
						rock->SetLifeTime(10.0f);
						rock->SetEmissivePower(0.0f);
						float rScale = Math::RandomRange(0.05, 0.15);
						rock->SetScale({ rScale, rScale, rScale });
						float rX = rand() % 360;
						float rY = rand() % 360;
						float rZ = rand() % 360;
						rock->SetAngle({ rX, rY, rZ });
						rock->SetColor({ 2.0, 0.8, 0.8, 1 });
						rock->SetUpdateType(RockData::RockFuncEnum::Up);
						RockEffect::Instance().rockMesh1->Register(rock);
					}
					rockIntervalTimer -= rockIntervalTime;
				}
			}

			

			// --- ambientColor ---
			float rColor = Easing::GetNowParam(Easing::OutQuad<float>, lifeTimer, firstNovaColorUp);
			LightManager::Instance().SetAmbientColor({ rColor, 0.2f, 0.2f, 1.0f });
			

			// --- ステートチェック ---
			if (lifeTimer >= firstNovaTime)
			{
				lifeTimer = 0.0f;
				rockIntervalTimer = 0.0f;
				// ステート更新
				specialState = SpecialState::chargeNova;
			}
		}
		break;
	case SpecialState::chargeNova:
		{
			// --- radialBlur ---
			float sampCount;
			sampCount = Easing::GetNowParam(Easing::OutQuad<float>, lifeTimer, chargeNovaSamplingDown);
			radialBlur->SetSamplingCount(static_cast<float>(static_cast<int>(sampCount)));

			// --- rock ---
			rockIntervalTimer += deltaTime;
			while(rockIntervalTimer > rockIntervalTime)
			{
				for (int i = 0; i < 3; i++)
				{
					DirectX::XMFLOAT3 rPos = { (rand() % 30) - 15.0f, 0, rand() % 30 - 15.0f };
					DirectX::XMFLOAT3 rVec = { (rand() % 3) - 1.5f, rand() % 2 + 1.0f, rand() % 3 - 1.5f };

					RockData* rock = new RockData();
					rock->SetPos(rPos);
					rock->SetVeloicty(rVec);
					rock->SetLifeTime(10.0f);
					rock->SetEmissivePower(0.0f);
					float rScale = Math::RandomRange(0.05, 0.15);
					rock->SetScale({ rScale, rScale, rScale });
					float rX = rand() % 360;
					float rY = rand() % 360;
					float rZ = rand() % 360;
					rock->SetAngle({ rX, rY, rZ });
					rock->SetColor({ 2.0, 0.8, 0.8, 1 });
					rock->SetUpdateType(RockData::RockFuncEnum::Up);
					RockEffect::Instance().rockMesh1->Register(rock);
				}
				rockIntervalTimer -= rockIntervalTime;
			}

			// --- ステートチェック ---
			if (lifeTimer >= chargeNovaTime)
			{
				LightManager::Instance().SetAmbientColor({ 0.2f, 0.2f, 0.2f, 1.0f });

				radialBlur->SetIsRadial(false);
				lifeTimer = 0.0f;
				rockIntervalTimer = 0.0f;
				// ステート更新
				specialState = SpecialState::End;
			}
		}
		break;
	case SpecialState::End:
		{
			isSpecialEffect = false;
		}
		break;
	}
}

// 実行
void SpecialEffect::Emit()
{
	isSpecialEffect = true;

	specialState = SpecialState::Init;
}

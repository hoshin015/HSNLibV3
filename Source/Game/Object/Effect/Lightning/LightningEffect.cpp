#include "LightningEffect.h"

#include "../EffectDamageManager.h"
#include "../Rock/RockEffect.h"
#include "../../../../../Library/Timer.h"
#include "../../../../../Library/Math/Math.h"
#include "../../../../../Library/Particle/EmitterManager.h"

// èâä˙âª
void LightningEffect::Initialize()
{
	lightningMesh1 = std::make_unique<LightningMesh1>("Data/Fbx/Lightning/Main/1/mainLightning.model");
	lightningMesh2 = std::make_unique<LightningMesh1>("Data/Fbx/Lightning/Main/2/mainLightning.model");
	lightningMesh3 = std::make_unique<LightningMesh1>("Data/Fbx/Lightning/Area/LightningArea.model");
	lightningMesh4 = std::make_unique<LightningMesh1>("Data/Fbx/Lightning/Bottom/1/bottomLightning.model");
	lightningMesh5 = std::make_unique<LightningMesh1>("Data/Fbx/Lightning/Bottom/2/bottomLightning.model");
	lightningMesh6 = std::make_unique<LightningMesh1>("Data/Fbx/Lightning/Bottom/3/bottomLightning.model");
	lightningMesh7 = std::make_unique<LightningMesh1>("Data/Fbx/Lightning/Bottom/4/bottomLightning.model");
	lightningMesh8 = std::make_unique<LightningMesh1>("Data/Fbx/Lightning/Bottom/5/bottomLightning.model");
	lightningMesh9 = std::make_unique<LightningMesh1>("Data/Fbx/Lightning/Bottom/6/bottomLightning.model");

	damageRadius = 2.0f;
	damage = 20.0f;
	damageTimeStart = 1.2f;
	damageTimeEnd = 1.4f;
}

void LightningEffect::Update()
{
	float deltaTime = Timer::Instance().DeltaTime();

	int index = 0;
	for (auto& lightningEmit : lightningEmitters)
	{
		// É_ÉÅÅ[ÉWîªíË
		if (lightningEmit.timer >= damageTimeStart && lightningEmit.timer <= damageTimeEnd)
		{
			EffectDamageManager::EffectCollision effectCollision;
			EffectDamageManager::EffectCollision::SphereData sphere;
			sphere.position = lightningEmit.position;
			sphere.radius = damageRadius;
			sphere.damage = damage;
			effectCollision.spheres.emplace_back(sphere);
			EffectDamageManager::Instance().Register(effectCollision);
		}

		if (lightningEmit.addLightning0 && !lightningEmit.addLightning3)
		{
			bottomTimer += deltaTime;
			while (bottomTimer > bottomTime)
			{
				LightningData* l = new LightningData();
				l->SetPos(lightningEmit.position);
				l->SetLifeTime(0.1f);
				float s = Math::RandomRange(0.075f, 0.6f);
				l->SetScale({s, s, s});
				l->SetEmissivePower(1.0f);
				l->SetAngleY(rand() % 360);
				l->SetAngleX(-(rand() % 90));
				l->SetAngleZ(-(rand() % 90));
				l->SetUpdateType(LightningData::LightningFuncEnum::Bottom);
				l->SetColor({20.8, 2.8, 2.5, 1});

				if (rand() % 2)
				{
					lightningMesh4->Register(l);
				}
				else
				{
					lightningMesh5->Register(l);
				}

				bottomTimer -= bottomTime;
			}
		}


		lightningEmit.timer += deltaTime;
		if (!lightningEmit.addLightning0 && lightningEmit.timer >= lightning0GenerateTime)
		{
			PlayLightningSound();

			Easing::EasingValue scaleValue;
			scaleValue.startTime  = 0.0f;
			scaleValue.endTime    = 0.2f;
			scaleValue.startValue = 0.0f;
			scaleValue.endValue   = 0.2f;

			LightningData* l = new LightningData();
			l->SetPos(lightningEmit.position);
			l->SetLifeTime(0.1f);
			l->SetScale({1.0, 2.0, 1.0});
			l->SetEmissivePower(4.0f);
			l->SetVal1(scaleValue);
			l->SetEmissivePower(4.0f);
			l->SetAngleY(rand() % 360);
			l->SetUpdateType(LightningData::LightningFuncEnum::Main);
			l->SetColor({20.8, 2.8, 2.5, 1});
			lightningMesh1->Register(l);
			LightningData* l2 = new LightningData();
			l2->SetPos(lightningEmit.position);
			l2->SetLifeTime(0.1f);
			l2->SetScale({1.0, 2.0, 1.0});
			l2->SetEmissivePower(2.0f);
			l2->SetVal1(scaleValue);
			l2->SetEmissivePower(4.0f);
			l2->SetAngleY(rand() % 360);
			l2->SetUpdateType(LightningData::LightningFuncEnum::Main);
			l2->SetColor({20.8, 2.8, 2.5, 1});
			lightningMesh2->Register(l2);

			lightningEmit.addLightning0 = true;
		}

		if (!lightningEmit.addLightning1 && lightningEmit.timer >= lightning1GenerateTime)
		{
			Easing::EasingValue scaleValue;
			scaleValue.startTime  = 0.0f;
			scaleValue.endTime    = 0.2f;
			scaleValue.startValue = 0.0f;
			scaleValue.endValue   = 0.5f;

			LightningData* l = new LightningData();
			l->SetPos(lightningEmit.position);
			l->SetLifeTime(0.4f);
			l->SetScale({1.0, 2.0, 1.0});
			l->SetEmissivePower(4.0f);
			l->SetVal1(scaleValue);
			l->SetEmissivePower(4.0f);
			l->SetAngleY(rand() % 360);
			l->SetUpdateType(LightningData::LightningFuncEnum::Main);
			l->SetColor({20.8, 2.8, 2.5, 1});
			lightningMesh1->Register(l);
			LightningData* l2 = new LightningData();
			l2->SetPos(lightningEmit.position);
			l2->SetLifeTime(0.4f);
			l2->SetScale({1.0, 1.5, 1.0});
			l2->SetEmissivePower(4.0f);
			l2->SetVal1(scaleValue);
			l2->SetEmissivePower(4.0f);
			l2->SetAngleY(rand() % 360);
			l2->SetColor({20.8, 2.8, 2.5, 1});
			lightningMesh1->Register(l2);

			lightningEmit.addLightning1 = true;
		}

		if (!lightningEmit.addLightning2 && lightningEmit.timer >= lightning2GenerateTime)
		{
			Easing::EasingValue scaleValue;
			scaleValue.startTime  = 0.0f;
			scaleValue.endTime    = 0.2f;
			scaleValue.startValue = 0.0f;
			scaleValue.endValue   = 0.5f;

			LightningData* l2 = new LightningData();
			l2->SetPos(lightningEmit.position);
			l2->SetLifeTime(0.5f);
			l2->SetScale({1.0, 1.0, 1.0});
			l2->SetEmissivePower(4.0f);
			l2->SetVal1(scaleValue);
			l2->SetAngleY(rand() % 360);
			l2->SetUpdateType(LightningData::LightningFuncEnum::Main);
			l2->SetColor({20.8, 2.8, 2.5, 1});
			lightningMesh2->Register(l2);

			lightningEmit.addLightning2 = true;
		}

		if (!lightningEmit.addLightning3 && lightningEmit.timer >= lightning3GenerateTime)
		{
			Easing::EasingValue scaleValue;
			scaleValue.startTime  = 0.0f;
			scaleValue.endTime    = 0.2f;
			scaleValue.startValue = 0.0f;
			scaleValue.endValue   = 0.3f;

			LightningData* l2 = new LightningData();
			l2->SetPos(lightningEmit.position);
			l2->SetLifeTime(0.3f);
			l2->SetScale({1.0, 2.0, 1.0});
			l2->SetEmissivePower(4.0f);
			l2->SetVal1(scaleValue);
			l2->SetUpdateType(LightningData::LightningFuncEnum::Main);
			l2->SetAngleY(rand() % 360);
			l2->SetColor({20.8, 2.8, 2.5, 1});
			lightningMesh2->Register(l2);

			lightningEmit.addLightning3 = true;

			// rock
			DirectX::XMFLOAT3 rPos    = lightningEmit.position;
			int               rockNum = rand() % 15 + 5;
			for (int i = 0; i < rockNum; i++)
			{
				DirectX::XMFLOAT3 rVec = {(rand() % 6) - 3.0f, rand() % 8 + 0.5f, rand() % 6 - 3.0f};
				RockEffect::Instance().Emit(rPos, rVec, {0.15, 0.3});
			}

			Emitter* emitter                           = new Emitter();
			emitter->position                          = lightningEmit.position;
			emitter->emitterData.duration              = 0.7;
			emitter->emitterData.looping               = false;
			emitter->emitterData.burstsTime            = 0.05;
			emitter->emitterData.burstsCount           = 256;
			emitter->emitterData.particleKind          = 5;
			emitter->emitterData.particleLifeTimeMin   = 0.05f;
			emitter->emitterData.particleLifeTimeMax   = 2.0f;
			emitter->emitterData.particleSpeedMin      = 1.0f;
			emitter->emitterData.particleSpeedMax      = 4.0f;
			emitter->emitterData.particleSizeMin       = {0.4f, 0.2};
			emitter->emitterData.particleSizeMax       = {0.8f, 0.4f};
			emitter->emitterData.particleColorMin      = {20.8, 2.8, 2.5, 1};
			emitter->emitterData.particleColorMax      = {20.8, 2.8, 2.5, 1};
			emitter->emitterData.particleGravity       = 4;
			emitter->emitterData.particleBillboardType = 1;
			emitter->emitterData.burstsOneShot         = 1;
			EmitterManager::Instance().Register(emitter);
		}

		if (lightningEmit.timer >= lightningEmit.destoryTime)
		{
			lightningEmitters.erase(lightningEmitters.begin() + index);
			continue;
		}
		index++;
	}


	lightningMesh1->Update();
	lightningMesh2->Update();
	lightningMesh3->Update();
	lightningMesh4->Update();
	lightningMesh5->Update();
	lightningMesh6->Update();
	lightningMesh7->Update();
	lightningMesh8->Update();
	lightningMesh9->Update();
}

void LightningEffect::Render(bool isShadow)
{
	lightningMesh1->Render(isShadow);
	lightningMesh2->Render(isShadow);
	lightningMesh3->Render(isShadow);
	lightningMesh4->Render(isShadow);
	lightningMesh5->Render(isShadow);
	lightningMesh6->Render(isShadow);
	lightningMesh7->Render(isShadow);
	lightningMesh8->Render(isShadow);
	lightningMesh9->Render(isShadow);
}

void LightningEffect::Emit(DirectX::XMFLOAT3 pos)
{
	int emitNum = 1;
	for (int i = 0; i < emitNum; i++)
	{
		LightningData* l2 = new LightningData();
		l2->SetPos(pos);
		l2->SetLifeTime(1.7f);
		l2->SetPosY(l2->GetPosY() + 0.001);
		l2->SetEmissivePower(0.5f);
		l2->SetScale({4, 0.5, 4});
		l2->SetColor({1.5, 0.8, 0.8, 1});
		l2->SetUpdateType(LightningData::LightningFuncEnum::Area);
		lightningMesh3->Register(l2);

		Emitter* emitter                           = new Emitter();
		emitter->position                          = pos;
		emitter->emitterData.duration              = 0.9;
		emitter->emitterData.looping               = false;
		emitter->emitterData.burstsTime            = 0.1;
		emitter->emitterData.burstsCount           = 10;
		emitter->emitterData.particleKind          = 4;
		emitter->emitterData.particleLifeTimeMin   = 0.6f;
		emitter->emitterData.particleLifeTimeMax   = 1.0f;
		emitter->emitterData.particleSpeedMin      = 0.025f;
		emitter->emitterData.particleSpeedMax      = 2.0f;
		emitter->emitterData.particleSizeMin       = {0.2f, 0.1f};
		emitter->emitterData.particleSizeMax       = {0.8f, 0.2f};
		emitter->emitterData.particleColorMin      = {20.8, 2.8, 2.5, 1};
		emitter->emitterData.particleColorMax      = {20.8, 2.8, 2.5, 1};
		emitter->emitterData.particleGravity       = 0;
		emitter->emitterData.particleBillboardType = 1;
		EmitterManager::Instance().Register(emitter);

		LightningEmitter lightningEmitter;
		lightningEmitter.position = pos;
		lightningEmitters.push_back(lightningEmitter);
	}
}

void LightningEffect::PlayLightningSound()
{
	int musicRand;
	int checkCount = 0;
	while (1)
	{
		musicRand = rand() % 10;
		if (!AudioManager::Instance().IsInUseMusic((static_cast<MUSIC_LABEL>(static_cast<int>(MUSIC_LABEL::Lightning1)))))
		{
			break;
		}
		checkCount++;
		if (checkCount > 100) break;
	}
	AudioManager::Instance().PlayMusic((static_cast<MUSIC_LABEL>(static_cast<int>(MUSIC_LABEL::Lightning1) + musicRand)), false);
	AudioManager::Instance().SetMusicVolume((static_cast<MUSIC_LABEL>(static_cast<int>(MUSIC_LABEL::Lightning1) + musicRand)), 0.4f);
}

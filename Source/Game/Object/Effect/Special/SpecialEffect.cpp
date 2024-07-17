#include "SpecialEffect.h"
#include "../../../../../Library/Particle/EmitterManager.h"
#include "../../../../../Library/RegisterNum.h"
#include "../../../../../Library/Timer.h"
#include "../Rock/RockEffect.h"
#include "../../../../../Library/Math/Math.h"
#include "../../../../../Library/3D/LightManager.h"
#include "../../../../../Library/3D/CameraManager.h"
#include "../Lightning/LightningEffect.h"
#include "../../../../../Library/3D/CameraManager.h"
#include "../EffectDamageManager.h"
#include "../../StateMachine/Player/Player.h"
#include "../../../../../Library/Math/Collision.h"
#include "../../Stage/StageManager.h"
#include "../../Stage/StageMain.h"

// 更新
void SpecialEffect::Update(RadialBlur* radialBlur, HeatHaze* heatHaze)
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

			// --- heatHaze ON ---
			heatHaze->SetIsHeatHaze(true);
			heatHaze->SetMaxShift(0.0f);


			// --- rock ---
			rockIntervalTimer = 0.0f;

			// --- smoke ---
			{
				Emitter* emitter                           = new Emitter();
				emitter->position                          = {0, 0, 0};
				emitter->emitterData.duration              = 10.0;
				emitter->emitterData.looping               = false;
				emitter->emitterData.burstsTime            = 0.3;
				emitter->emitterData.burstsCount           = 3;
				emitter->emitterData.particleKind          = pk_smoke;
				emitter->emitterData.particleLifeTimeMin   = 2.0f;
				emitter->emitterData.particleLifeTimeMax   = 6.0f;
				emitter->emitterData.particleSpeedMin      = 1.0f;
				emitter->emitterData.particleSpeedMax      = 60.0f;
				emitter->emitterData.particleSizeMin       = {60.0f, 60.0f};
				emitter->emitterData.particleSizeMax       = {100.0f, 100.0f};
				emitter->emitterData.particleColorMin      = {2.9, 0.9, 0.9, 1};
				emitter->emitterData.particleColorMax      = {3.0, 1.0, 1.0, 1};
				emitter->emitterData.particleGravity       = 0;
				emitter->emitterData.particleBillboardType = 0;
				emitter->emitterData.particleTextureType   = 6;
				EmitterManager::Instance().Register(emitter);
			}

			AudioManager::Instance().StopMusic(MUSIC_LABEL::BATTLE1);
			AudioManager::Instance().PlayMusic(MUSIC_LABEL::MonsterRoar1, false);
			AudioManager::Instance().PlayMusic(MUSIC_LABEL::Fire1, false);

			AudioManager::Instance().PlayMusic(MUSIC_LABEL::NovaNoise1, false);
			AudioManager::Instance().SetMusicVolume(MUSIC_LABEL::NovaNoise1, 0.4);

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

			DirectX::XMFLOAT2 ndc = Math::ScreenToNdcPos(Math::WorldToScreenPos({ 0,0,0 }, CameraManager::Instance().GetCamera().get()));
			ndc.x = (ndc.x + 1.0f) / 2.0f;
			ndc.y = (ndc.y + 1.0f) / 2.0f;
			radialBlur->SetBlurPosition(ndc);

			// --- heatHaze ---
			float heatHazeMaxShift =
				Easing::GetNowParam(Easing::OutQuad<float>, lifeTimer, firstNovaHeatHazeMaxShiftUp);
			heatHaze->SetMaxShift(heatHazeMaxShift);

			// --- rock ---
			if (lifeTimer > firstNovaRockStartTime)
			{
				rockIntervalTimer += deltaTime;
				while (rockIntervalTimer > rockIntervalTime)
				{
					GenerateRock();
					rockIntervalTimer -= rockIntervalTime;
				}
			}

			// --- lightning ---
			lightningIntervalTimer += deltaTime;
			while (lightningIntervalTimer > lightningIntervalTime)
			{
				GenerateLightning();
				lightningIntervalTimer -= 0.2;
			}


			// --- ambientColor ---
			float rColor = Easing::GetNowParam(Easing::OutQuad<float>, lifeTimer, firstNovaColorUp);
			LightManager::Instance().SetAmbientColor({rColor, 0.2f, 0.2f, 1.0f});


			// --- ステートチェック ---
			if (lifeTimer >= firstNovaTime)
			{
				// カメラ揺らし
				CameraManager::Instance().shakeTimer = chargeNovaTime;
				CameraManager::Instance().shakePower = 2.0f;

				radialBlur->SetIsRadial(false);
				lifeTimer         = 0.0f;
				rockIntervalTimer = 0.0f;
				// ステート更新
				specialState = SpecialState::chargeNova;
			}
		}
		break;
	case SpecialState::chargeNova:
		{
			// --- heatHaze ---
			float heatHazeMaxShift =
				Easing::GetNowParam(Easing::OutQuad<float>, lifeTimer, chargeNovaHeatHazeMaxShiftDown);
			heatHaze->SetMaxShift(heatHazeMaxShift);

			// --- rock ---
			rockIntervalTimer += deltaTime;
			while (rockIntervalTimer > rockIntervalTime)
			{
				GenerateRock();
				rockIntervalTimer -= rockIntervalTime;
			}

			// --- lightning ---
			lightningIntervalTimer += deltaTime;
			while (lightningIntervalTimer > lightningIntervalTime)
			{
				GenerateLightning();
				lightningIntervalTimer -= 0.2;
			}

			// --- ambientColor ---
			float rColor = Easing::GetNowParam(Easing::OutQuad<float>, lifeTimer, firstNovaColorDown);
			LightManager::Instance().SetAmbientColor({rColor, 0.2f, 0.2f, 1.0f});

			// --- ChargeSound ---
			if(!isPlayChargeSound && lifeTimer > playChargeSoundTime)
			{
				AudioManager::Instance().PlayMusic(MUSIC_LABEL::NovaNoise2);
				AudioManager::Instance().SetMusicVolume(MUSIC_LABEL::NovaNoise2, 1.5f);
				isPlayChargeSound = true;
			}

			// --- ステートチェック ---
			if (lifeTimer >= chargeNovaTime)
			{
				AudioManager::Instance().StopMusic(MUSIC_LABEL::NovaNoise1);

				LightManager::Instance().SetAmbientColor({0.2f, 0.2f, 0.2f, 1.0f});

				radialBlur->SetIsRadial(false);
				heatHaze->SetIsHeatHaze(false);
				lifeTimer         = 0.0f;
				rockIntervalTimer = 0.0f;
				// ステート更新
				specialState = SpecialState::Nova;
			}
		}
		break;
	case SpecialState::Nova:
		{
			// ダメージ判定
			if (!isDamaged)
			{
				EffectDamageManager::EffectCollision effectCollision;
				EffectDamageManager::EffectCollision::SphereData sphere;
				sphere.position = {0,0,0};
				sphere.radius = damageRadius;
				sphere.damage = damage;
				effectCollision.spheres.emplace_back(sphere);
				EffectDamageManager::Instance().Register(effectCollision);

				// プレイヤーとの衝突判定
				// --- プレイヤーの球 ---
				for (auto& playerSphere : Player::Instance().GetModel()->GetModelResource()->GetSkeletonSphereCollisions())
				{
					if (isDamaged) continue;

					// --- プレイヤーの球の座標を取得 ---
					Vector3 playerBonePosition = Player::Instance().GetPos();
					playerBonePosition = (playerSphere.name == "") ? playerBonePosition + playerSphere.position : Player::Instance().GetBonePosition(playerSphere.name);


					Vector3 dummy;
					if (Collision::IntersectSphereVsSphere({ 0,0,0 }, damageRadius, playerBonePosition.vec_, playerSphere.radius, dummy.vec_))
					{
						// --- ここに当たった時の処理を書く ---
						isDamaged = true;

						CameraManager::Instance().shakeTimer = 1.0f;
						CameraManager::Instance().shakePower = 100.0f;

						Player& player = Player::Instance();
						float currentHP = player.AStatus().hp;
						player.AStatus().hp -= damage;

						// --- この攻撃でプレイヤーが死亡したとき ---
						if (player.AStatus().hp <= 0.0f && currentHP > 0.0f)
						{
							CameraManager::Instance().SetCurrentCamera("PlayerDeadCamera");
						}

						break;
					}
				}
			}

			drawAfterStage = true;

			// 仮置きm
			Emitter* emitter = new Emitter();
			emitter->position = { 0, 3, 3 };
			emitter->emitterData.duration = 5.0;
			emitter->emitterData.looping = false;
			emitter->emitterData.burstsTime = 0.1;
			emitter->emitterData.burstsCount = 256;
			emitter->emitterData.particleKind = pk_Dust;
			emitter->emitterData.particleLifeTimeMin = 1.0f;
			emitter->emitterData.particleLifeTimeMax = 1.0f;
			emitter->emitterData.particleSpeedMin = 1.0f;
			emitter->emitterData.particleSpeedMax = 5.0f;
			emitter->emitterData.particleSizeMin = { 0.1f, 0.1f };
			emitter->emitterData.particleSizeMax = { 0.4f, 0.4f };
			emitter->emitterData.particleColorMin = { 10.2, 0.0, 0.0, 1 };
			emitter->emitterData.particleColorMax = { 40.2, 0.8, 0.8, 1 };
			emitter->emitterData.particleGravity = -1;
			emitter->emitterData.particleBillboardType = 0;
			emitter->emitterData.particleTextureType = 0;
			emitter->emitterData.burstsOneShot = 1;
			EmitterManager::Instance().Register(emitter);

			// ステート更新
			specialState = SpecialState::UpMusic;
		}
		break;
	case SpecialState::UpMusic:
		{
			lifeTimer += deltaTime;

#if SPECIAL_AUDIO_DELAY
			if(AudioManager::Instance().GetSoundState(MUSIC_LABEL::BATTLE2) == DirectX::SoundState::PAUSED)
			{
				AudioManager::Instance().ResumeMusic(MUSIC_LABEL::BATTLE2);
#else
			if (!AudioManager::Instance().IsInUseMusic(MUSIC_LABEL::BATTLE2))
			{
				AudioManager::Instance().PlayMusic(MUSIC_LABEL::BATTLE2);
#endif
			}
			AudioManager::Instance().SetMusicVolume(MUSIC_LABEL::BATTLE2, Easing::GetNowParam(Easing::OutQuad<float>, lifeTimer, soundUpValue));

			if (lifeTimer >= chargeNovaTime)
			{
				lifeTimer = 0.0f;
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
	isPlayChargeSound = false;

	specialState = SpecialState::Init;
}

// 岩生成
void SpecialEffect::GenerateRock()
{
	for (int i = 0; i < 2; i++)
	{
		DirectX::XMFLOAT3 rPos = {(rand() % 60) - 30.0f, 0, rand() % 60 - 30.0f};
		DirectX::XMFLOAT3 rVec = {(rand() % 3) - 1.5f, rand() % 1 + 0.5f, rand() % 3 - 1.5f};

		RockData* rock = new RockData();
		rock->SetPos(rPos);
		rock->SetVeloicty(rVec);
		rock->SetLifeTime(10.0f);
		rock->SetEmissivePower(0.0f);
		float rScale = Math::RandomRange(0.05, 0.15);
		rock->SetScale({rScale + Math::RandomRange(0.05, 0.15), rScale, rScale});
		float rX = rand() % 360;
		float rY = rand() % 360;
		float rZ = rand() % 360;
		rock->SetAngle({rX, rY, rZ});
		rock->SetColor({10.0, 0.8, 0.8, 1});
		rock->SetUpdateType(RockData::RockFuncEnum::Up);
		RockEffect::Instance().rockMesh1->Register(rock);
	}
}

// lightning 生成
void SpecialEffect::GenerateLightning()
{
	for (int i = 0; i < 2; i++)
	{
		LightningData* l = new LightningData();

		float r = (rand() % 360) / 3.14f;

		l->SetPos({cos(r) * 6 - 3, sin(r) * 3, sin(r) * 6 - 3});

		float life = rand() % 1 * 0.3f + 0.1f;
		l->SetLifeTime(life);
		float s = (rand() % 1) * 5.0f + 1.5f;
		l->SetScale({s + rand() % 3, s, s});

		l->SetAngle({
			static_cast<float>(rand() % 360), static_cast<float>(rand() % 360),
			static_cast<float>(rand() % 360)
		});

		l->SetEmissivePower(2.0f);
		l->SetUpdateType(LightningData::LightningFuncEnum::Bottom);
		l->SetColor({20.8, 2.8, 2.5, 1});

		int rLightning = rand() % 6;
		switch (rLightning)
		{
		case 0: LightningEffect::Instance().lightningMesh4->Register(l); break;
		case 1: LightningEffect::Instance().lightningMesh5->Register(l); break;
		case 2: LightningEffect::Instance().lightningMesh6->Register(l); break;
		case 3: LightningEffect::Instance().lightningMesh7->Register(l); break;
		case 4: LightningEffect::Instance().lightningMesh8->Register(l); break;
		case 5: LightningEffect::Instance().lightningMesh9->Register(l); break;
		}
	}
}

#include <tchar.h>
#include <DirectXMath.h>
// --- external ---
#include "../../External/ImGui/imgui.h"
// --- libarary ---
#include "../../Library/Framework.h"
#include "../../Library/Graphics/Shader.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Math/OperatorXMFloat3.h"
#include "../../Library/Timer.h"
#include "../../Library/ImGui/ConsoleData.h"
#include "../../Library/ImGui/ImGuiManager.h"
#include "../../Library/Input/InputManager.h"
#include "../../Library/Effekseer/EffectManager.h"
#include "../../Library/3D/Camera.h"
#include "../../Library/3D/DebugPrimitive.h"
#include "../../Library/3D/LightManager.h"
#include "../../Library/Particle/Particle.h"
#include "../../Library/Particle/EmitterManager.h"
#include "../../Library/Text/DispString.h"
#include "../../Library/Math/Math.h"
// --- Scene ---
#include "SceneTest.h"
#include "SceneManager.h"
// --- Game ---
#include "../Game/Object/Stage/StageManager.h"
#include "../Game/Object/Stage/StageMain.h"
#include "../Game/Object/StateMachine/Enemy/Enemy.h"
#include "../Game/Object/StateMachine/Player/Player.h"
#include "../Game/Object/Effect/Breath/BreathEffect.h"
#include "../Game/Object/Effect/Special/SpecialEffect.h"
#include "../Game/Object/Effect/Lightning/LightningEffect.h"
#include "../Game/Object/Effect/Rock/RockEffect.h"
// --- UserInterface ---
#include "../UserInterface//UiPause.h"
#include "../UserInterface/DamageTextManager.h"
#include "../UserInterface/UiGame.h"
#include "../Game/Object/Effect/EffectDamageManager.h"
#include "../UserInterface/UiClearAfter.h"
#include "../Game/Object/DodgeEffect/DodgeEffect.h"
#include "../UserInterface/CaptureScreen.h"


void SceneTest::Initialize()
{
		
	// --- カメラ初期設定 ---
#if 0
	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(0, 20, 20), // カメラ座標
		DirectX::XMFLOAT3(0, 0, 0),   // ターゲット(設定しても意味ない)
		DirectX::XMFLOAT3(0, 1, 0)    // 上方向ベクトル
	);
	Camera::Instance().SetAngle({DirectX::XMConvertToRadians(45), DirectX::XMConvertToRadians(180), 0});
	Camera::Instance().cameraType = Camera::CAMERA::TARGET_PLAYER;
#else
	CameraManager::Instance().Initialize();
	CameraManager::Instance().SetCurrentCamera("PlayerCamera");
	CameraManager::Instance().GetCamera("PlayerCamera")->Initialize();
	//camera = &playerCamera;
	//camera = &lockOnCamera;
#endif

	// --- ライト初期設定 ---
	Light* directionLight = new Light(LightType::Directional);
	directionLight->SetDirection(DirectX::XMFLOAT3(0.5, -1, -1));
	directionLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	LightManager::Instance().Register(directionLight);
	LightManager::Instance().SetAmbientColor({0.2f, 0.2f, 0.2f, 1.0f});


	// --- ステージ初期化 ---
	float stageScale = 4.0f;
	StageManager& stageManager = StageManager::Instance();
	StageMain*    stageBefore    = new StageMain("Data/Fbx/BeforeStage/BeforeStage.model");
	stageBefore->SetScale({ stageScale, stageScale, stageScale });
	stageManager.Register(stageBefore);
	StageMain* stage = new StageMain("./Data/Fbx/Stage/StageCollision2.model");
	stage->GetModel()->GetModelResource()->SetScale(4.0f);
	stageManager.Register(stage);
	StageMain* stageAfter = new StageMain("Data/Fbx/AfterStage/AfterStage.model");
	stageAfter->SetScale({ stageScale, stageScale, stageScale });
	stageManager.Register(stageAfter);
	StageMain* stageObject = new StageMain("Data/Fbx/StageObject/StageObject.model");
	stageObject->SetScale({ stageScale, stageScale, stageScale });
	stageManager.Register(stageObject);

	// --- buffer 系初期化 ---
	{
		std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex());

		bitBlockTransfer = std::make_unique<FullScreenQuad>();
		frameBuffer      = std::make_unique<FrameBuffer>(Framework::Instance().GetScreenWidthF(),
		                                            Framework::Instance().GetScreenHeightF(), true);
		bloom = std::make_unique<Bloom>(Framework::Instance().GetScreenWidthF(), Framework::Instance().GetScreenHeightF());
		shadow = std::make_unique<Shadow>();
		wbOitBuffer = std::make_unique<WbOitBuffer>(Framework::Instance().GetScreenWidthF(),
		                                            Framework::Instance().GetScreenHeightF());
		radialBlur = std::make_unique<RadialBlur>(Framework::Instance().GetScreenWidthF(),
		                                          Framework::Instance().GetScreenHeightF());
		heatHaze = std::make_unique<HeatHaze>(Framework::Instance().GetScreenWidthF(),
		                                          Framework::Instance().GetScreenHeightF());
		colorFilter = std::make_unique<ColorFilter>(Framework::Instance().GetScreenWidthF(),
		                                          Framework::Instance().GetScreenHeightF());
		colorFilter->SetIsColorFilter(true);
		colorFilter->SetBrightness(2.0f);
		swordTrailBuffer = std::make_unique<FrameBuffer>(Framework::Instance().GetScreenWidthF(),
		                                          Framework::Instance().GetScreenHeightF());
		swordTrailBufferSub = std::make_unique<FrameBuffer>(Framework::Instance().GetScreenWidthF(),
		                                          Framework::Instance().GetScreenHeightF());
		// 画面キャプチャの初期化
		CaptureScreen::Instance().Initialize();

		Enemy::Instance().radialBlur = radialBlur.get();

		Player::Instance().SetCamera(CameraManager::Instance().GetCamera().get());	// 今のカメラを設定
		UiPause::Instance();
		UiGame::Instance().Initialize();
		UiClearAfter::Instance();
		LightningEffect::Instance().Initialize();
		RockEffect::Instance().Initialize();

		// ------- ps 生成 -------
		CreatePsFromCso("Data/Shader/SwordTrailPS.cso", swordTrailPisxelShader.GetAddressOf());
		// テスト
		AudioManager::Instance().PlayMusic(MUSIC_LABEL::BATTLE1, true);
		AudioManager::Instance().SetMusicVolume(MUSIC_LABEL::BATTLE1, 0.5f);

		// --- skyMap 初期化 ---
		skyMap = std::make_unique<SkyMap>(L"Data/Texture/winter_evening_4k.DDS");
	}



	// --- AnimatedObject 初期化 ---
	//blendTestPlayer = std::make_unique<BlendTestPlayer>("Data/Fbx/BlendTestPlayer/BlendTestPlayer.model");


	Gate::Instance().Initialize();

	Enemy::Instance().Initialize();
	Player::Instance().Initialize();
	Player::Instance().SetColorFilter(colorFilter.get());


	UiPause::Instance().Initialize();
	//UiGame::Instance().Initialize();
	UiClearAfter::Instance().Initialize();

	//LightningEffect::Instance().Initialize();
	//RockEffect::Instance().Initialize();
	BreathEffect::Instance().Initialize();

	SpecialEffect::Instance().drawAfterStage = false;

	// ------- ps 生成 -------
	//CreatePsFromCso("Data/Shader/SwordTrailPS.cso", swordTrailPisxelShader.GetAddressOf());

	// テスト
	//AudioManager::Instance().PlayMusic(MUSIC_LABEL::BATTLE1, true);
	//AudioManager::Instance().SetMusicVolume(MUSIC_LABEL::BATTLE1, 0.5f);

	isFirst = true;

	// ゲーム用のタイマーリセット
	UiGame::Instance().gameTimer = 0.0f;

#if SPECIAL_AUDIO_DELAY
	AudioManager::Instance().PlayMusic(MUSIC_LABEL::BATTLE2, true);
	AudioManager::Instance().SetMusicVolume(MUSIC_LABEL::BATTLE2, 0.0f);
	stopSoundTimer = false;
	soundTimer = 0.0f;
#endif
}

void SceneTest::Finalize()
{
	StageManager::Instance().Clear();
	LightManager::Instance().Clear();
	EmitterManager::Instance().Clear();
	Enemy::Instance().Finalize();
	DodgeEffect::Instance().Clear();

	AudioManager::Instance().StopMusic(MUSIC_LABEL::BATTLE1);
	AudioManager::Instance().StopMusic(MUSIC_LABEL::BATTLE2);
}

void SceneTest::Update()
{
	if(isFirst)
	{
		isFirst = false;
		Particle::Instance().Initialize();
	}
	

#if USE_IMGUI
	// --- imguiManager処理 ---
	ImGuiManager::Instance().Update();
#endif
	// --- inputManager処理 ---
	InputManager::Instance().Update();


	if (UiPause::Instance().Update()) return;

	// クリアフラグがたっていないならタイムを加算
	if(!UiClearAfter::Instance().clearFlag)
	{
		UiGame::Instance().gameTimer += Timer::Instance().DeltaTime();
	}

	UiGame::Instance().Update();
	UiClearAfter::Instance().Update();

	// --- effectManager処理 ---
	EffectManager::Instance().Update();

	// --- カメラ処理 ---
#if 0
	Camera::Instance().SetTarget(Player::Instance().GetPos());
	//Camera::Instance().SetTarget(blendTestPlayer->GetPos());
	Camera::Instance().Update();
#else
	CameraManager::Instance().UpdateShake();
	CameraManager::Instance().UpdateClearTimer();
	CameraManager::Instance().Update();
	//camera->Update();
#endif


	// タイマーの定数バッファの更新
	UpdateTimerConstnat();

	// ステージ更新
	StageManager::Instance().Update();

	Enemy::Instance().Update();

	Player::Instance().Update();
	//blendTestPlayer->Update();
	Enemy::Instance().CollisionVSPlayer();

	DamageTextManager::Instance().Update();

	EmitterManager::Instance().Update();
	Particle::Instance().Update();

	Gate::Instance().Update();

	// テストエミッター
	if (InputManager::Instance().GetKeyPressed(Keyboard::F1))
	{
		for(int i = 0; i < 10; i++)
		{
			DirectX::XMFLOAT3 rPos = { (rand() % 10) - 5.0f, 0, rand() % 10 - 5.0f };
			DirectX::XMFLOAT3 rVec = { (rand() % 10) - 5.0f, 5, rand() % 10 - 5.0f };
			RockEffect::Instance().Emit(rPos, rVec,{0.1, 1});
		}
	}
	if (InputManager::Instance().GetKeyPressed(Keyboard::F2))
	{
		for (int i = 0; i < 30; i++)
		{
			DirectX::XMFLOAT3 rPos = { (rand() % 30) - 15.0f, 0, rand() % 30 - 15.0f };
			DirectX::XMFLOAT3 rVec = { (rand() % 3) - 1.5f, 5, rand() % 3 - 1.5f };

			RockData* rock = new RockData();
			rock->SetPos(rPos);
			rock->SetVeloicty(rVec);
			rock->SetLifeTime(10.0f);
			rock->SetEmissivePower(0.0f);
			float rScale = Math::RandomRange(0.25, 0.5);
			rock->SetScale({ rScale, rScale, rScale });
			float rX = rand() % 360;
			float rY = rand() % 360;
			float rZ = rand() % 360;
			rock->SetAngle({ rX, rY, rZ });
			rock->SetColor({ 1.0, 1.0, 1.0, 1 });
			rock->SetUpdateType(RockData::RockFuncEnum::Up);
			RockEffect::Instance().rockMesh1->Register(rock);
		}
	}
	if (InputManager::Instance().GetKeyPressed(Keyboard::F3))
	{
		for(int i = 0; i < 1; i++)
		{
			DirectX::XMFLOAT3 rPos = { (rand() % 40) - 20.0f, 0, rand() % 40 - 20.0f };
			//DirectX::XMFLOAT3 rPos = { 0,0,0};
			LightningEffect::Instance().Emit(rPos);
		}
	}
	if (InputManager::Instance().GetKeyPressed(Keyboard::F4))
	{
		Emitter* emitter0 = new Emitter();
		emitter0->position = { 0, 0, 0 };
		emitter0->emitterData.duration = 3.0;
		emitter0->emitterData.looping = false;
		emitter0->emitterData.burstsTime = 0.3;
		emitter0->emitterData.burstsCount = 3;
		emitter0->emitterData.particleKind = pk_smoke;
		emitter0->emitterData.particleLifeTimeMin = 1.0f;
		emitter0->emitterData.particleLifeTimeMax = 2.0f;
		emitter0->emitterData.particleSpeedMin = 20.0f;
		emitter0->emitterData.particleSpeedMax = 30.0f;
		emitter0->emitterData.particleSizeMin = { 30.0f, 30.0f };
		emitter0->emitterData.particleSizeMax = { 50.0f, 50.0f };
		emitter0->emitterData.particleColorMin = { 0.9, 0.9, 0.9, 1 };
		emitter0->emitterData.particleColorMax = { 1.0, 1.0, 1.0, 1 };
		emitter0->emitterData.particleGravity = 0;
		emitter0->emitterData.particleBillboardType = 0;
		emitter0->emitterData.particleTextureType = 6;
		EmitterManager::Instance().Register(emitter0);
	}
	if (InputManager::Instance().GetKeyPressed(Keyboard::F5))
	{
		Emitter* emitter0 = new Emitter();
		emitter0->position = { 0, 3, 0 };
		emitter0->emitterData.duration = 10.0;
		emitter0->emitterData.looping = false;
		emitter0->emitterData.burstsTime = 0.05;
		emitter0->emitterData.burstsCount = 1;
		emitter0->emitterData.particleKind = pk_novaEndFire;
		emitter0->emitterData.particleLifeTimeMin = 0.5f;
		emitter0->emitterData.particleLifeTimeMax = 1.0f;
		emitter0->emitterData.particleSpeedMin = 1.0f;
		emitter0->emitterData.particleSpeedMax = 1.0f;
		emitter0->emitterData.particleSizeMin = { 40.0f, 40.0f };
		emitter0->emitterData.particleSizeMax = { 40.0f, 40.0f };
		emitter0->emitterData.particleColorMin = { 3.0, 1.0, 1.0, 1 };
		emitter0->emitterData.particleColorMax = { 3.0, 1.0, 1.0, 1 };
		emitter0->emitterData.particleGravity = 0;
		emitter0->emitterData.particleBillboardType = 0;
		emitter0->emitterData.particleTextureType = 11;
		emitter0->emitterData.burstsOneShot = 1;
		EmitterManager::Instance().Register(emitter0);
	}
	if (InputManager::Instance().GetKeyPressed(Keyboard::F6))
	{
		Emitter* emitter0 = new Emitter();
		emitter0->position = { 0, 0, 0 };
		emitter0->emitterData.duration = 3.0;
		emitter0->emitterData.looping = false;
		emitter0->emitterData.burstsTime = 0.05;
		emitter0->emitterData.burstsCount = 24;
		emitter0->emitterData.particleKind = pk_novaBurst;
		emitter0->emitterData.particleLifeTimeMin = 2.0f;
		emitter0->emitterData.particleLifeTimeMax = 2.0f;
		emitter0->emitterData.particleSpeedMin = 24.0f;
		emitter0->emitterData.particleSpeedMax = 36.0f;
		emitter0->emitterData.particleSizeMin = { 0.1f, 0.1f };
		emitter0->emitterData.particleSizeMax = { 0.3f, 0.3f };
		emitter0->emitterData.particleColorMin = { 6.0, 0.8, 0.8, 1 };
		emitter0->emitterData.particleColorMax = { 10.0, 0.8, 0.8, 1 };
		emitter0->emitterData.particleGravity = 0;
		emitter0->emitterData.particleBillboardType = 0;
		emitter0->emitterData.particleTextureType = 0;
		emitter0->emitterData.burstsOneShot = 6;
		EmitterManager::Instance().Register(emitter0);
	}

	if (InputManager::Instance().GetKeyPressed(Keyboard::F7))
	{
		Emitter* emitter0 = new Emitter();
		emitter0->position = { 0, 0, 0 };
		emitter0->emitterData.duration = 3.0;
		emitter0->emitterData.looping = false;
		emitter0->emitterData.burstsTime = 0.1;
		emitter0->emitterData.burstsCount = 36;
		emitter0->emitterData.particleKind = pk_novaStartFire;
		emitter0->emitterData.particleLifeTimeMin = 0.7f;
		emitter0->emitterData.particleLifeTimeMax = 0.9f;
		emitter0->emitterData.particleSpeedMin = 50.0f;
		emitter0->emitterData.particleSpeedMax = 60.0f;
		emitter0->emitterData.particleSizeMin = { 17.0f, 17.0f };
		emitter0->emitterData.particleSizeMax = { 18.0f, 18.0f };
		emitter0->emitterData.particleColorMin = { 5.0, 0.9, 0.9, 1 };
		emitter0->emitterData.particleColorMax = { 5.0, 1.0, 1.0, 1 };
		emitter0->emitterData.particleGravity = 0;
		emitter0->emitterData.particleBillboardType = 0;
		emitter0->emitterData.particleTextureType = 9;
		emitter0->emitterData.burstsOneShot = 1;
		EmitterManager::Instance().Register(emitter0);
	}

	if (InputManager::Instance().GetKeyPressed(Keyboard::F8))
	{
		Emitter* emitter0 = new Emitter();
		emitter0->position = { 0, 0, 0 };
		emitter0->emitterData.duration = 3.0;
		emitter0->emitterData.looping = false;
		emitter0->emitterData.burstsTime = 0.025;
		emitter0->emitterData.burstsCount = 5;
		emitter0->emitterData.particleKind = pk_fireBreath;
		emitter0->emitterData.particleLifeTimeMin = 2.0f;
		emitter0->emitterData.particleLifeTimeMax = 4.0f;
		emitter0->emitterData.particleSpeedMin = 10.0f;
		emitter0->emitterData.particleSpeedMax = 15.0f;
		emitter0->emitterData.particleSizeMin = { 2.0f, 2.0f };
		emitter0->emitterData.particleSizeMax = { 3.0f, 3.0f };
		emitter0->emitterData.particleColorMin = { 3.0, 1.0, 1.0, 1 };
		emitter0->emitterData.particleColorMax = { 3.0, 1.0, 1.0, 1 };
		emitter0->emitterData.particleGravity = 0;
		emitter0->emitterData.particleBillboardType = 0;
		emitter0->emitterData.particleTextureType = 10;
		EmitterManager::Instance().Register(emitter0);
	}

	if (InputManager::Instance().GetKeyPressed(Keyboard::F9))
	{
		BreathEffect::Instance().Emit();
	}

	if (InputManager::Instance().GetKeyPressed(Keyboard::F10))
	{
		SpecialEffect::Instance().Emit();
	}

	static float f11TotalTimer = 0.0f;
	static float f11Timer = 0.0f;
	static float f11Time = 0.025f;

	if(f11TotalTimer > 0.0f)
	{
		f11TotalTimer -= Timer::Instance().DeltaTime();
		f11Timer += Timer::Instance().DeltaTime();
		while(f11Timer > f11Time)
		{
			f11Timer -= f11Time;

			LightningData* l = new LightningData();
			l->SetLifeTime(0.2f);
			l->SetEmissivePower(5.0f);
			float rScale = Math::RandomRange(0.0f, 0.2f) + 0.05f;
			l->SetScale({ Math::RandomRange(0.0f, 0.05f) + 0.01f, rScale, rScale });
			l->SetColor({ 1.5, 0.8, 0.8, 1 });
			l->SetAngle({ static_cast<float>(rand() % 360), static_cast<float>(rand() % 360), static_cast<float>(rand() % 360) });
			l->SetUpdateType(LightningData::LightningFuncEnum::HeadAura);
			l->SetUvScroll({ 0, static_cast<float>(rand() % 1) ,0,0 });

			DirectX::XMFLOAT3 s = Enemy::Instance().GetBonePosition("tosaka");
			DirectX::XMFLOAT3 e = Enemy::Instance().GetBonePosition("tosaka_end");
			DirectX::XMVECTOR S = DirectX::XMLoadFloat3(&s);
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&e);
			DirectX::XMVECTOR L = DirectX::XMVectorSubtract(E, S);
			float length = DirectX::XMVectorGetX(DirectX::XMVector3Length(L));

			l->SetBufferLength(Math::RandomRange(0.0f, length));

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

	if (InputManager::Instance().GetKeyPressed(Keyboard::F11))
	{
		f11TotalTimer = 1.0f;
	}


	LightningEffect::Instance().Update();
	RockEffect::Instance().Update();
	BreathEffect::Instance().Update();
	SpecialEffect::Instance().Update(radialBlur.get(), heatHaze.get());
	DodgeEffect::Instance().Update();

#if SPECIAL_AUDIO_DELAY
	// sound
	if(!stopSoundTimer)
	{
		soundTimer += Timer::Instance().DeltaTime();
		if (soundTimer >= 48)
		{
			AudioManager::Instance().PauseMusic(MUSIC_LABEL::BATTLE2);
			stopSoundTimer = true;
		}
	}
#endif
}

void SceneTest::Render()
{
	// 必要なポインタ取得
	Graphics*            gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc  = gfx->GetDeviceContext();

	// renderTargetの設定
	dc->OMSetRenderTargets(1, gfx->GetRTVAddress(), gfx->GetDSV());

	// imGuiBufferを使用しない場合はこっちを記述する
	{
		// renderTargetのクリア
		dc->ClearRenderTargetView(gfx->GetRTV(), gfx->GetBgColor());
		// depthStencilViewのクリア
		dc->ClearDepthStencilView(gfx->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	// カメラの定数バッファの更新
#if 0
	Camera::Instance().UpdateCameraConstant();
#else
	CameraManager::Instance().UpdateConstants();
	//camera->UpdateConstants();
#endif
	// ライトの定数バッファの更新
	LightManager::Instance().UpdateConstants();

	// ====== shadowMap ======
	{
		shadow->Clear();                   // シャドウマップクリア
		shadow->UpdateShadowCasterBegin(); // シャドウマップ描画準備

		for (int i = 0; i < SHADOWMAP_COUNT; i++)
		{
			shadow->Activate(i, CameraManager::Instance().GetCamera().get());
			// 影を付けたいモデルはここで描画を行う(Render の引数に true をいれる)
			{
				// --- animated object ---
				shadow->SetAnimatedShader(); // animated object の影描画開始
				//StageManager::Instance().Render(true);

				if(SpecialEffect::Instance().drawAfterStage)
					StageManager::Instance().Render(2, true);
				else
					StageManager::Instance().Render(0, true);
				StageManager::Instance().Render(3, true);

				Enemy::Instance().Render(true);

				Player::Instance().Render(true);
				//blendTestPlayer->Render(true);

				// --- static object ---
				shadow->SetStaticShader(); // static object の影描画開始

				RockEffect::Instance().Render(true);
				//testStatic->Render(true);

				Gate::Instance().Render(true);
			}
			shadow->DeActivate();
		}

		// 通常描画用にテクスチャと定数バッファ更新
		shadow->SetShadowTextureAndConstants();
	}

	// ====== 不透明描画 ======
	frameBuffer->Clear(gfx->GetBgColor());
	frameBuffer->Activate();
	{
		gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
		gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
		gfx->SetBlend(BLEND_STATE::ALPHA);

		// ここに不透明オブジェクトの描画
		if (SpecialEffect::Instance().drawAfterStage)
			StageManager::Instance().Render(2, false);
		else
			StageManager::Instance().Render(0, false);
		StageManager::Instance().Render(3, false);

		//testStatic->Render();
		Enemy::Instance().Render();

		if(Player::Instance().GetModel()->data.materialColorKd.w >= 0.99f)
		{
			Player::Instance().Render(Player::Instance().pbrPS.Get());
		}
		

		RockEffect::Instance().Render();

		if (showCollision)
		{
			Enemy::Instance().DrawDebugPrimitive();
			Player::Instance().DrawDebugPrimitive();
			EffectDamageManager::Instance().Render();
		}
		Enemy::Instance().DrawDebug();
		Player::Instance().DrawDebug();
		DebugPrimitive::Instance().Render();

		Gate::Instance().Render();

		skyMap->Render();
	}
	frameBuffer->DeActivate();

	// ====== 半透明描画 ======
	wbOitBuffer->Clear();
	wbOitBuffer->Activate(frameBuffer->depthStencilView.Get());
	{
		gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);

		// ここに半透明オブジェクトの描画
		Particle::Instance().Render();

		gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_CULL_NONE);
		LightningEffect::Instance().Render();
		BreathEffect::Instance().Render();

		gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
		if (Player::Instance().GetModel()->data.materialColorKd.w < 0.99f)
		{
			Player::Instance().Render(Player::Instance().wboitPbrPS.Get());
		}
		DodgeEffect::Instance().Render();
	}
	wbOitBuffer->DeActivate();

	// ====== 不透明・半透明の統合 ======
	frameBuffer->Activate();
	{
		gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
		gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
		gfx->SetBlend(BLEND_STATE::ALPHA);

		ID3D11ShaderResourceView* srvs[2] =
		{
			wbOitBuffer->shaderResourceViews[_accumTexture].Get(),
			wbOitBuffer->shaderResourceViews[_revealTexture].Get(),
		};
		bitBlockTransfer->blit(srvs, 0, ARRAYSIZE(srvs), wbOitBuffer->GetWbOitPS());
	}
	frameBuffer->DeActivate();

	// ======= swrodTrail =======
	swordTrailBufferSub->Activate();
	{
		bitBlockTransfer->blit(frameBuffer->shaderResourceViews[0].GetAddressOf(), 0, 1);
	}
	swordTrailBufferSub->DeActivate();

	float swordTrailClear[4] = {0,0,0,0};
	swordTrailBuffer->Clear(swordTrailClear);
	swordTrailBuffer->Activate();
	{
		dc->OMSetRenderTargets(1, swordTrailBuffer->renderTargetView.GetAddressOf(), frameBuffer->depthStencilView.Get());

		gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
		gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_CULL_NONE);
		Player::Instance().swordTrail->Render();
		gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	}
	swordTrailBuffer->DeActivate();

	frameBuffer->Activate();
	{
		ID3D11ShaderResourceView* swordShvs[2] =
		{
			swordTrailBufferSub->shaderResourceViews[0].Get(),
			swordTrailBuffer->shaderResourceViews[0].Get(),
		};
		bitBlockTransfer->blit(swordShvs, 0, ARRAYSIZE(swordShvs), swordTrailPisxelShader.Get());
	}
	frameBuffer->DeActivate();


	// ポストエフェクトをかけるたびにこれを更新する
	ID3D11ShaderResourceView* useSrv = frameBuffer->shaderResourceViews[0].Get();
	//bitBlockTransfer->blit(frameBuffer->shaderResourceViews[0].GetAddressOf(), 0, 1);

	// ====== colorFilter =====
	if(colorFilter->GetIsColorFilter())
	{
		colorFilter->Make(useSrv);
		useSrv = colorFilter->GetSrv();
	}
	
	// ====== heatHaze =====
	if(heatHaze->GetIsHeatHaze())
	{
		heatHaze->Make(useSrv);
		useSrv = heatHaze->GetSrv();
	}
	
	// ====== ラジアルブラー ======
	if(radialBlur->GetIsRadial())
	{
		radialBlur->Make(useSrv);
		useSrv = radialBlur->GetSrv();
	}
	
	// ====== ブルーム処理しての描画 ======
	bloom->Make(useSrv);
	ID3D11ShaderResourceView* shvs[2] =
	{
		useSrv,
		bloom->GetSrv()
	};
	bitBlockTransfer->blit(shvs, 0, 2, bloom->GetFinalPassPs());

	if (CaptureScreen::Instance().BeginCapture())
	{
		bitBlockTransfer->blit(shvs, 0, 2, bloom->GetFinalPassPs());
		CaptureScreen::Instance().EndCapture();
	}

	// ======　ブルームなしの描画　======　

	// ここでスプライト描画
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_CULL_NONE);

	UiGame::Instance().Render();
	UiPause::Instance().Render();
	UiClearAfter::Instance().Render();

	// ここで文字描画
	DamageTextManager::Instance().Render();

	// テキストデータの文字描画
	//DispString::Instance().Draw(L"てすとめっせーじ", {200, 50}, 50);


#if USE_IMGUI
	// --- デバッグGUI描画 ---
	DrawDebugGUI();

	ImGuiManager::Instance().Console();

	UiGame::Instance().DrawDebugImGui();
	UiClearAfter::Instance().DrawDebugImGui();

	Player::Instance().DrawDebugImGui(0);
	Enemy::Instance().DrawDebugImGui(0);

	StageManager::Instance().DrawDebugImGui();
	Enemy::Instance().DrawDebugGui();
	CameraManager::Instance().DrawDebugGui();

	LightManager::Instance().DrawDebugGui();
	bloom->DrawDebugGui();
	shadow->DrawDebugGui();
	wbOitBuffer->DrawDebugGui();
	radialBlur->DrawDebugGui();
	heatHaze->DrawDebugGui();
	colorFilter->DrawDebugGui();
	Gate::Instance().DrawDebugImGui();
	CaptureScreen::Instance().DrawDebugGui();


	CameraManager::Instance().GetCamera()->DrawDebugGui();

#if SHOW_PERFORMANCE
	// --- パフォーマンス描画 ---
	ImGuiManager::Instance().DisplayPerformanceStats();
#endif

	// --- imguiManager 描画 ---
	ImGuiManager::Instance().Render();
#endif
}

// デバッグ描画
void SceneTest::DrawDebugGUI()
{
	// メニューバー描画
	DrawMenuBar();

	ImGui::Begin("TestScene");
	{
		ImGui::Checkbox("collision", &showCollision);


		Vector3 playerPos = Player::Instance().GetPos();
		float length = playerPos.Length();
		ImGui::Text(u8"プレイヤーの中心からの距離 : %f", length);

		// --- カメラ関連 ---
		//static bool cameraFlag = true;
		//InputManager& input = InputManager::Instance();
		//if (input.GetKeyPressed(DirectX::Keyboard::Keys::L))
		//{
		//	// --- プレイヤーカメラをセット ---
		//	if (!cameraFlag)
		//	{
		//		auto camera = CameraManager::Instance().GetCamera();
		//		Vector3 position = camera->GetCurrentPosition();
		//		Vector3 target = camera->GetTarget();
		//		CameraManager::Instance().SetCurrentCamera("PlayerCamera");
		//		auto ptr = std::dynamic_pointer_cast<PlayerCamera>(CameraManager::Instance().GetCamera());
		//		ptr->SetCurrentPosition(position);
		//		ptr->SetTarget(target);
		//		ptr->currentTarget = target;
		//		//ptr->OnSetCamera();
		//		//camera = &playerCamera;
		//		//playerCamera.OnSetCamera();
		//		Player::Instance().SetCamera(CameraManager::Instance().GetCamera().get());

		//		cameraFlag = !cameraFlag;
		//	}

		//	// --- ロックオンカメラをセット ---
		//	else
		//	{
		//		if(!Enemy::Instance().IsDead())
		//		{
		//			auto camera = CameraManager::Instance().GetCamera();
		//			Vector3 position = camera->GetPosition();
		//			Vector3 target = camera->GetTarget();
		//			CameraManager::Instance().SetCurrentCamera("LockOnCamera");
		//			camera = CameraManager::Instance().GetCamera();
		//			camera->Initialize();
		//			//camera = &lockOnCamera;
		//			//camera->Initialize();

		//			camera->SetPosition(position);
		//			camera->SetCurrentPosition(position);
		//			camera->SetTarget(target);
		//			Player::Instance().SetCamera(camera.get());

		//			cameraFlag = !cameraFlag;
		//		}
		//	}
		//}

		ImGui::Image(swordTrailBuffer->shaderResourceViews[0].Get(), ImVec2(200, 200));
	}
	ImGui::End();
}

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
#include "../../Library/ImGui/ImGuiManager.h"
#include "../../Library/Input/InputManager.h"
#include "../../Library/Effekseer/EffectManager.h"
#include "../../Library/3D/Camera.h"
#include "../../Library/3D/LightManager.h"
#include "../../Library/Particle/Particle.h"
#include "../../Library/Particle/EmitterManager.h"
#include "../../Library/Text/DispString.h"
// --- Scene ---
#include "SceneTest.h"
#include "SceneManager.h"
// --- Game ---
#include "../Game/Object/Stage/StageManager.h"
#include "../Game/Object/Stage/StageMain.h"
#include "../Game/Object/StateMachine/Enemy/Enemy.h"
#include "../Game/Object/StateMachine/Player/Player.h"
// --- UserInterface ---
#include "../../Library/3D/DebugPrimitive.h"
#include "../UserInterface//UiPause.h"
#include "../../Library/ImGui/ConsoleData.h"


void SceneTest::Initialize()
{
	// --- カメラ初期設定 ---
	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(0, 20, 20), // カメラ座標
		DirectX::XMFLOAT3(0, 0, 0),   // ターゲット(設定しても意味ない)
		DirectX::XMFLOAT3(0, 1, 0)    // 上方向ベクトル
	);
	Camera::Instance().SetAngle({DirectX::XMConvertToRadians(45), DirectX::XMConvertToRadians(180), 0});
	Camera::Instance().cameraType = Camera::CAMERA::TARGET_PLAYER;

	// --- ライト初期設定 ---
	Light* directionLight = new Light(LightType::Directional);
	directionLight->SetDirection(DirectX::XMFLOAT3(0.5, -1, -1));
	directionLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	LightManager::Instance().Register(directionLight);
	LightManager::Instance().SetAmbientColor({0.2f, 0.2f, 0.2f, 1.0f});


	// --- ステージ初期化 ---
	StageManager& stageManager = StageManager::Instance();
	StageMain*    stageMain    = new StageMain("Data/Fbx/ExampleStage/ExampleStage.model");
	stageManager.Register(stageMain);

	// --- buffer 系初期化 ---
	bitBlockTransfer = std::make_unique<FullScreenQuad>();
	frameBuffer      = std::make_unique<FrameBuffer>(Framework::Instance().GetScreenWidthF(),
	                                            Framework::Instance().GetScreenHeightF(), true);
	bloom = std::make_unique<Bloom>(Framework::Instance().GetScreenWidthF(), Framework::Instance().GetScreenHeightF());
	shadow = std::make_unique<Shadow>();
	wbOitBuffer = std::make_unique<WbOitBuffer>(Framework::Instance().GetScreenWidthF(),
	                                            Framework::Instance().GetScreenHeightF());
	radialBlur = std::make_unique<RadialBlur>(Framework::Instance().GetScreenWidthF(),
	                                          Framework::Instance().GetScreenHeightF());

	// --- AnimatedObject 初期化 ---
	blendTestPlayer = std::make_unique<BlendTestPlayer>("Data/Fbx/BlendTestPlayer/BlendTestPlayer.model");

	// --- StaticObject 初期化 ---
	testStatic = std::make_unique<TestStatic>("Data/Fbx/StaticAlbino/StaticAlbino.model");

	// --- Sprite 初期化 ---
	sprTest  = std::make_unique<Sprite>("Data/Texture/bomb/bomb.sprite");
	sprTest2 = std::make_unique<Sprite>("Data/Texture/Icon.sprite");
	sprTest2->SetPos({200, 100});
	sprTest3 = std::make_unique<Sprite>("Data/Texture/Nessie.sprite");
	sprTest3->SetPos({500, 100});
	sprTest3->SetScale({0.2, 0.2});
	sprTest3->UpdateAnimation();

	sprText = std::make_unique<Sprite>("Data/Texture/Font/font0.sprite");
	sprText->UpdateAnimation();


	Enemy::Instance().Initialize();
	Player::Instance().Initialize();


	Particle::Instance().Initialize();

	// --- Emitter 登録 ---
	Emitter* emitter0                           = new Emitter();
	emitter0->position                          = {0, 3, 3};
	emitter0->emitterData.duration              = 2;
	emitter0->emitterData.looping               = false;
	emitter0->emitterData.burstsTime            = 0.5;
	emitter0->emitterData.burstsCount           = 99;
	emitter0->emitterData.particleKind          = 0;
	emitter0->emitterData.particleLifeTimeMin   = 1.0f;
	emitter0->emitterData.particleLifeTimeMax   = 1.0f;
	emitter0->emitterData.particleSizeMin       = {0.4f, 0.1f};
	emitter0->emitterData.particleSizeMax       = {0.4f, 0.1f};
	emitter0->emitterData.particleColorMin      = {1.8, 1.8, 1.8, 1};
	emitter0->emitterData.particleColorMax      = {1.8, 1.8, 1.8, 1};
	emitter0->emitterData.particleFrictionMin   = 0;
	emitter0->emitterData.particleFrictionMax   = 1;
	emitter0->emitterData.particleGravity       = 1;
	emitter0->emitterData.particleBillboardType = 1;
	EmitterManager::Instance().Register(emitter0);

	//Emitter* emitter1       = new Emitter();
	//emitter1->position      = {0, 3, 0};
	//emitter1->rate          = 32;
	//emitter1->startKind     = 1;
	//emitter1->rateOverTime  = 0.25f;
	//emitter1->startLifeTime = 6.0f;
	//emitter1->startSize = { 0.05f, 0.05f };

	//EmitterManager::Instance().Register(emitter1);


	UiPause::Instance().Initialize();


	lightningEffect = std::make_unique<LightningMainMesh>("Data/Fbx/normal/lightning1.model");
}

void SceneTest::Finalize()
{
	StageManager::Instance().Clear();
	LightManager::Instance().Clear();
	EmitterManager::Instance().Clear();
}

void SceneTest::Update()
{
#if USE_IMGUI
	// --- imguiManager処理 ---
	ImGuiManager::Instance().Update();
#endif
	// --- inputManager処理 ---
	InputManager::Instance().Update();


	if (UiPause::Instance().Update()) return;

	// --- effectManager処理 ---
	EffectManager::Instance().Update();

	// --- カメラ処理 ---
	Camera::Instance().SetTarget(Player::Instance().GetPos());
	//Camera::Instance().SetTarget(blendTestPlayer->GetPos());
	Camera::Instance().Update();


	// タイマーの定数バッファの更新
	UpdateTimerConstnat();

	// ステージ更新
	StageManager::Instance().Update();

	testStatic->Update();

	Enemy::Instance().Update();

	Player::Instance().Update();
	//blendTestPlayer->Update();

	sprTest->SetAngle(sprTest->GetAngle() + 180 * Timer::Instance().DeltaTime());
	sprTest->UpdateAnimation();

	sprTest3->SetAngle(sprTest->GetAngle() + 180 * Timer::Instance().DeltaTime());


	EmitterManager::Instance().Update();
	Particle::Instance().Update();

	if (InputManager::Instance().GetKeyPressed(Keyboard::F2))
	{
		lightningEffect->Initialize();
	}
	lightningEffect->Update();
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
	Camera::Instance().UpdateCameraConstant();
	// ライトの定数バッファの更新
	LightManager::Instance().UpdateConstants();

	// ====== shadowMap ======
	{
		shadow->Clear();                   // シャドウマップクリア
		shadow->UpdateShadowCasterBegin(); // シャドウマップ描画準備

		for (int i = 0; i < SHADOWMAP_COUNT; i++)
		{
			shadow->Activate(i);
			// 影を付けたいモデルはここで描画を行う(Render の引数に true をいれる)
			{
				// --- animated object ---
				shadow->SetAnimatedShader(); // animated object の影描画開始
				StageManager::Instance().Render(true);
				Enemy::Instance().Render(true);

				Player::Instance().Render(true);
				//blendTestPlayer->Render(true);

				// --- static object ---
				shadow->SetStaticShader(); // static object の影描画開始
				testStatic->Render(true);
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
		StageManager::Instance().Render();

		testStatic->Render();
		Enemy::Instance().Render();

		Player::Instance().Render();

		if (showCollision)
		{
			Enemy::Instance().DrawDebugPrimitive();
			Player::Instance().DrawDebugPrimitive();
		}
		DebugPrimitive::Instance().Render();
	}
	frameBuffer->DeActivate();

	// ====== 半透明描画 ======
	wbOitBuffer->Clear();
	wbOitBuffer->Activate(frameBuffer->depthStencilView.Get());
	{
		gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_CULL_NONE);

		// ここに半透明オブジェクトの描画
		Particle::Instance().Render();

		lightningEffect->Render();
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

	// ====== ラジアルブラー ======
	radialBlur->Make(frameBuffer->shaderResourceViews[0].Get());

#if 1
	// ====== ブルーム処理しての描画 ======
	bloom->Make(radialBlur->GetSrv());
	bitBlockTransfer->blit(bloom->GetSrvAddress(), 0, 1);
#else
	// ====== そのまま描画 ======
	bitBlockTransfer->blit(frameBuffer->shaderResourceViews[0].GetAddressOf(), 0, 1);
#endif

	// ======　ブルームなしの描画　======　

	// ここでスプライト描画
	sprTest->Render();
	sprTest2->Render();
	sprTest3->Render();


	UiPause::Instance().Render();

	// ここで文字描画

	// テキストデータの文字描画
	DispString::Instance().Draw(L"てすとめっせーじ", {200, 50}, 50);

	// スプライトデータの文字描画　(色のついた文字を描画したい場合はこっちを使用)
	sprText->SprTextOut("0542", {300, 300});

#if USE_IMGUI
	// --- デバッグGUI描画 ---
	DrawDebugGUI();

	ImGuiManager::Instance().Console();

	Player::Instance().DrawDebugImGui(0);

	LightManager::Instance().DrawDebugGui();
	bloom->DrawDebugGui();
	shadow->DrawDebugGui();
	wbOitBuffer->DrawDebugGui();
	radialBlur->DrawDebugGui();

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
	}
	ImGui::End();
}

#include <tchar.h>
#include <DirectXMath.h>

// --- external ---
#include "../../External/ImGui/imgui.h"
// --- libarary ---
#include "../../Library/Framework.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Math/OperatorXMFloat3.h"
#include "../../Library/Timer.h"
#include "../../Library/ImGui/ImGuiManager.h"
#include "../../Library/Input/InputManager.h"
#include "../../Library/Effekseer/EffectManager.h"
#include "../../Library/3D/Camera.h"
// --- Scene ---
#include "SceneTitle.h"
#include "SceneManager.h"
// --- Game ---
#include "../Game/Object/Stage/StageManager.h"
#include "../Game/Object/Stage/StageMain.h"
// --- UserInterface ---
#include "../UserInterface/UiTitle.h"
#include "../../Library/3D/LightManager.h"
#include "../../Library/Particle/Particle.h"
#include "../../Library/Particle/EmitterManager.h"

void SceneTitle::Initialize()
{
	Framework* frameWork = &Framework::Instance();

	// --- skyMap 初期化 ---
	skyMap = std::make_unique<SkyMap>(L"Data/Texture/winter_evening_4k.DDS");

	// --- カメラ初期設定 ---
	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(0, 7, 20), // カメラ座標
		DirectX::XMFLOAT3(0, 5, 0),   // ターゲット
		DirectX::XMFLOAT3(0, 1, 0)    // 上方向ベクトル
	);
	Camera::Instance().cameraType = Camera::CAMERA::FREE;

	// --- ライト初期設定 ---
	Light* directionLight = new Light(LightType::Directional);
	directionLight->SetDirection(DirectX::XMFLOAT3(0.5, -1, -1));
	directionLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	LightManager::Instance().Register(directionLight);
	LightManager::Instance().SetAmbientColor({0.2f, 0.2f, 0.2f, 1.0f});

	// --- ステージ初期化 ---
	StageManager& stageManager = StageManager::Instance();
	StageMain*    stageMain    = new StageMain("Data/Fbx/BeforeStage/BeforeStage.model");
	stageManager.Register(stageMain);

	// player
	titlePlayer = std::make_unique<TestAnimated>("Data/Fbx/TitlePlayer/TitlePlayer.model");
	titlePlayer->SetPos({ -2, 3, 5 });
	titlePlayer->PlayAnimation(1, true);

	// titleFloor
	titleFloor = std::make_unique<TestAnimated>("Data/Fbx/Floor/Floor.model");
	titleFloor->SetPos({ -2, 3, 5 });
	titleFloor->SetScale({ 40, 40, 40 });

	// frameBuffer
	bitBlockTransfer = std::make_unique<FullScreenQuad>();
	frameBuffer = std::make_unique<FrameBuffer>(Framework::Instance().GetScreenWidthF(),
		Framework::Instance().GetScreenHeightF(), true);
	wbOitBuffer = std::make_unique<WbOitBuffer>(Framework::Instance().GetScreenWidthF(),
		Framework::Instance().GetScreenHeightF());

	UiTitle::Instance().Initialize();

	Particle::Instance().Initialize();

	AudioManager::Instance().PlayMusic(MUSIC_LABEL::TITLE, true);
	AudioManager::Instance().SetMusicVolume(MUSIC_LABEL::TITLE, 0.75f);
}

void SceneTitle::Finalize()
{
	StageManager::Instance().Clear();
	LightManager::Instance().Clear();
	EmitterManager::Instance().Clear();

	AudioManager::Instance().StopMusic(MUSIC_LABEL::TITLE);
}

void SceneTitle::Update()
{
#if USE_IMGUI
	// --- imguiManager処理 ---
	ImGuiManager::Instance().Update();
#endif
	// --- inputManager処理 ---
	InputManager::Instance().Update();

	// --- effectManager処理 ---
	EffectManager::Instance().Update();

	// --- カメラ処理 ---
	//Camera::Instance().Update();

	// タイマーの定数バッファの更新
	UpdateTimerConstnat();

	// ステージ更新
	StageManager::Instance().Update();

	// player
	titlePlayer->Update();
	// titleFloor
	titleFloor->Update();


	if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Enter))
	{
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTest));
	}
	// テストエミッター
	if (InputManager::Instance().GetKeyPressed(Keyboard::F1))
	{
		Emitter* emitter0                           = new Emitter();
		emitter0->position                          = {100, 100, 0};
		emitter0->emitterData.duration              = 5.0;
		emitter0->emitterData.looping               = false;
		emitter0->emitterData.burstsTime            = 0.1;
		emitter0->emitterData.burstsCount           = 1;
		emitter0->emitterData.particleKind          = pk_titleSelect;
		emitter0->emitterData.particleLifeTimeMin   = 1.0f;
		emitter0->emitterData.particleLifeTimeMax   = 2.0f;
		emitter0->emitterData.particleSpeedMin      = 0.0f;
		emitter0->emitterData.particleSpeedMax      = 0.0f;
		emitter0->emitterData.particleSizeMin       = {30.0f, 30.0f};
		emitter0->emitterData.particleSizeMax       = {30.0f, 30.0f};
		emitter0->emitterData.particleColorMin      = {1.0, 1.0, 1.0, 1};
		emitter0->emitterData.particleColorMax      = {1.0, 1.0, 1.0, 1};
		emitter0->emitterData.particleGravity       = 1;
		emitter0->emitterData.particleBillboardType = 0;
		emitter0->emitterData.particleTextureType   = 0;
		emitter0->emitterData.burstsOneShot = 1;
		EmitterManager::Instance().Register(emitter0);
	}

	UiTitle::Instance().Update();

	EmitterManager::Instance().Update();
	Particle::Instance().Update();

#if USE_IMGUI
#endif
}

void SceneTitle::Render()
{
	// 必要なポインタ取得
	Graphics*            gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc  = gfx->GetDeviceContext();

	// renderTargetの設定
	dc->OMSetRenderTargets(1, gfx->GetRTVAddress(), gfx->GetDSV());

	// imGuiBufferを使用しない場合はこっちを記述する
	{
		float bgcolor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
		// renderTargetのクリア
		dc->ClearRenderTargetView(gfx->GetRTV(), bgcolor);
		// depthStencilViewのクリア
		dc->ClearDepthStencilView(gfx->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	// カメラの定数バッファの更新
	Camera::Instance().UpdateCameraConstant();
	// ライトの定数バッファの更新
	LightManager::Instance().UpdateConstants();


	// ====== 不透明描画 ======
	frameBuffer->Clear(gfx->GetBgColor());
	frameBuffer->Activate();
	{
		gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
		gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
		gfx->SetBlend(BLEND_STATE::ALPHA);

		// ここに不透明オブジェクトの描画
		// ここに不透明オブジェクトの描画
		if (UiTitle::Instance().GetIsStageRender())
		{
			StageManager::Instance().Render();
			skyMap->Render();
		}
		if (UiTitle::Instance().GetIsCharacterRender())
		{
			titlePlayer->Render();
			titleFloor->NoAnimRender();
		}
		skyMap->Render();

		// UI 描画
		UiTitle::Instance().Render();
	}
	frameBuffer->DeActivate();

	// ====== 半透明描画 ======
	wbOitBuffer->Clear();
	wbOitBuffer->Activate(frameBuffer->depthStencilView.Get());
	{
		gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_TRUE_SOLID);

		// ここに半透明オブジェクトの描画
		if(UiTitle::Instance().isEmitterRender) Particle::Instance().ScreenRender();
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

	bitBlockTransfer->blit(frameBuffer->shaderResourceViews[0].GetAddressOf(), 0, 1);
	

	

#if USE_IMGUI
	// --- デバッグ描画 ---
	DrawDebugGUI();
	UiTitle::Instance().DrawDebugImGui();
	LightManager::Instance().DrawDebugGui();

#if SHOW_PERFORMANCE
	// --- パフォーマンス描画 ---
	ImGuiManager::Instance().DisplayPerformanceStats();
#endif

	// --- imguiManager 描画 ---
	ImGuiManager::Instance().Render();
#endif
}

// デバッグ描画
void SceneTitle::DrawDebugGUI()
{
	// メニューバー描画
	DrawMenuBar();
}

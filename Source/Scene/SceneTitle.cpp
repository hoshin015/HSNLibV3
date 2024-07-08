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
	StageMain*    stageMain    = new StageMain("Data/Fbx/StageMain/StageMain.model");
	stageManager.Register(stageMain);
	StageMain* StageFence = new StageMain("Data/Fbx/StageFence/StageFence.model");
	stageManager.Register(StageFence);

	// player
	titlePlayer = std::make_unique<TestAnimated>("Data/Fbx/TitlePlayer/TitlePlayer.model");
	titlePlayer->SetPos({ -2, 3, 5 });
	titlePlayer->PlayAnimation(1, true);

	// titleFloor
	titleFloor = std::make_unique<TestAnimated>("Data/Fbx/Floor/Floor.model");
	titleFloor->SetPos({ -2, 3, 5 });
	titleFloor->SetScale({ 40, 40, 40 });

	frameBuffer = std::make_unique<FrameBuffer>(frameWork->GetScreenWidthF(), frameWork->GetScreenHeightF());

	UiTitle::Instance().Initialize();
}

void SceneTitle::Finalize()
{
	StageManager::Instance().Clear();
	LightManager::Instance().Clear();
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

	UiTitle::Instance().Update();

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


	// rasterizerStateの設定
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	// depthStencilStateの設定
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	// blendStateの設定
	gfx->SetBlend(BLEND_STATE::ALPHA);


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


	// UI 描画
	UiTitle::Instance().Render();

#if USE_IMGUI
	// --- デバッグ描画 ---
	DrawDebugGUI();
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

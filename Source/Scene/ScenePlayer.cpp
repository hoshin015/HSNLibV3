#include "ScenePlayer.h"

#include "../../Library/3D/Camera.h"
#include "../../Library/3D/LightManager.h"
#include "../Game/Object/Stage/StageManager.h"
#include "../Game/Object/Stage/StageMain.h"
#include "../../Library/Input/InputManager.h"
#include "../../Library/ImGui/ImGuiManager.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Source/Game/Object/Player/PlayerManager.h"
#include "../../Library/3D/LineRenderer.h"

void ScenePlayer::Initialize()
{
	// カメラ初期設定
	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(0, 20, 20),		// カメラ座標
		DirectX::XMFLOAT3(0, 0, 0),			// ターゲット(設定しても意味ない)
		DirectX::XMFLOAT3(0, 1, 0)			// 上方向ベクトル
	);
	Camera::Instance().SetAngle({ DirectX::XMConvertToRadians(45), DirectX::XMConvertToRadians(180), 0 });
	Camera::Instance().cameraType = Camera::CAMERA::TARGET_PLAYER;

	// ライト初期設定
	Light* directionLight = new Light(LightType::Directional);
	directionLight->SetDirection(DirectX::XMFLOAT3(0.5, -1, -1));
	directionLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	LightManager::Instance().Register(directionLight);
	LightManager::Instance().SetAmbientColor({ 0.2f, 0.2f, 0.2f, 1.0f });

	// ステージ初期化
	StageManager& stageManager = StageManager::Instance();
	StageMain* stageMain = new StageMain("Data/Fbx/ExampleStage/ExampleStage.model");
	stageManager.Register(stageMain);

	//プレイヤー初期化
	PlayerManager& playerManager = PlayerManager::Instance();
	Player* player1 = new Player("Data/Fbx/Jummo/Jummo.model",false);
	playerManager.Register(player1);
	Player* player2 = new Player("Data/Fbx/Jummo/Jummo.model",true);
	player2->SetPosX(10.0f);
	playerManager.Register(player2);
}

void ScenePlayer::Finalize()
{
	StageManager::Instance().Clear();
	LightManager::Instance().Clear();
	PlayerManager::Instance().Clear();
}

void ScenePlayer::Update()
{
#if USE_IMGUI
	// --- imguiManager処理 ---
	ImGuiManager::Instance().Update();
#endif
	// --- inputManager処理 ---
	InputManager::Instance().Update();
	// --- カメラ処理 ---
	Camera::Instance().SetTarget(PlayerManager::Instance().GetPositionCenter());
	Camera::Instance().Update();
	// ステージ更新
	StageManager::Instance().Update();
	//プレイヤー更新
	PlayerManager::Instance().Update();


}

void ScenePlayer::Render()
{
	// 必要なポインタ取得
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

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

	// マップの描画
	StageManager::Instance().Render();
	//プレイヤーの描画
	PlayerManager::Instance().Render();
	//線の描画
	LineRenderer::Instance().Render();

#if USE_IMGUI
	// --- デバッグGUI描画 ---
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

void ScenePlayer::DrawDebugGUI()
{
	// メニューバー描画
	DrawMenuBar();
	PlayerManager::Instance().DrawDebugImGui();
}

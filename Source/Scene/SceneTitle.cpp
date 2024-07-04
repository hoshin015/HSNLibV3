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
// --- Scene ---
#include "SceneTitle.h"
#include "SceneManager.h"
// --- Game ---
#include "../UserInterface/UiTitle.h"


void SceneTitle::Initialize()
{
	Framework* frameWork = &Framework::Instance();

	spr1 = std::make_unique<Sprite>("Data/Texture/Text/PressAnyButton.sprite");
	spr1->UpdateAnimation();
	spr1->SetPos({640, 550});
	//spr1->SetDissolveTexture(L"Data/Texture/dissolve.png");

	imGuiFrameBuffer = std::make_unique<FrameBuffer>(frameWork->GetScreenWidthF(), frameWork->GetScreenHeightF());

	UiTitle::Instance().Initialize();
}

void SceneTitle::Finalize()
{
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

	// rasterizerStateの設定
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	// depthStencilStateの設定
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	// blendStateの設定
	gfx->SetBlend(BLEND_STATE::ALPHA);	

	//spr1->Render();
	UiTitle::Instance().Render();

#if USE_IMGUI
	// --- デバッグ描画 ---
	DrawDebugGUI();

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

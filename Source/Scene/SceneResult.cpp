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
#include "SceneResult.h"
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


void SceneResult::Initialize()
{
	sprBackground = std::make_unique<Sprite>("Data/Texture/UserInterface/Result/resultBackground.png");
}

void SceneResult::Finalize()
{
}

void SceneResult::Update()
{
#if USE_IMGUI
	// --- imguiManager処理 ---
	ImGuiManager::Instance().Update();
#endif
	// --- inputManager処理 ---
	InputManager::Instance().Update();

	// --- effectManager処理 ---
	EffectManager::Instance().Update();

	// タイマーの定数バッファの更新
	UpdateTimerConstnat();
}

void SceneResult::Render()
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

	// ここでスプライト描画
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_CULL_NONE);
	gfx->SetBlend(BLEND_STATE::ALPHA);
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_OFF_ZW_OFF);

	sprBackground->Render();

	// ここで文字描画
	DamageTextManager::Instance().Render();

	// テキストデータの文字描画
	//DispString::Instance().Draw(L"てすとめっせーじ", {200, 50}, 50);


#if USE_IMGUI
	// --- デバッグGUI描画 ---
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
void SceneResult::DrawDebugGUI()
{
	// メニューバー描画
	DrawMenuBar();
}

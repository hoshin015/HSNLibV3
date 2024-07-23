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
#include "../UserInterface/CaptureScreen.h"


void SceneResult::Initialize()
{
	capturedBackground = std::make_unique<FullScreenQuad>();
	capturedSrv = CaptureScreen::Instance().GetRandomTexture();

	sprBlack = std::make_unique<Sprite>("Data/Texture/Black.png");
	sprBackground = std::make_unique<Sprite>("Data/Texture/UserInterface/Result/resultBackground.png");
	sprTimeBoard = std::make_unique<Sprite>("Data/Texture/UserInterface/Result/timeBoard.sprite");
	sprTimeBoard->UpdateAnimation();
	sprTimeBoard->SetPos({ 640, 250 });
	sprRankBoard = std::make_unique<Sprite>("Data/Texture/UserInterface/Result/rankBoard.sprite");
	sprRankBoard->UpdateAnimation();
	sprRankBoard->SetPos({ 640, 450 });


	sprRankS = std::make_unique<Sprite>("Data/Texture/UserInterface/Result/rankS.sprite");
	sprRankS->UpdateAnimation();
	sprRankS->SetPos({ 900, 250 });
	sprRankA = std::make_unique<Sprite>("Data/Texture/UserInterface/Result/rankA.sprite");
	sprRankA->UpdateAnimation();
	sprRankA->SetPos({ 900, 250 });
	sprRankB = std::make_unique<Sprite>("Data/Texture/UserInterface/Result/rankB.sprite");
	sprRankB->UpdateAnimation();
	sprRankB->SetPos({ 900, 250 });
	sprRankC = std::make_unique<Sprite>("Data/Texture/UserInterface/Result/rankC.sprite");
	sprRankC->UpdateAnimation();
	sprRankC->SetPos({ 900, 250 });


	sprGoTitle = std::make_unique<Sprite>("Data/Texture/UserInterface/Result/goTitle.sprite");
	sprGoTitle->UpdateAnimation();
	sprGoTitle->SetPos({ 640, 650 });


	colorFilter = std::make_unique<ColorFilter>(Framework::Instance().GetScreenWidthF(),
		Framework::Instance().GetScreenHeightF());
	colorFilter->SetIsColorFilter(true);
	colorFilter->SetSaturation(1.15f);
	colorFilter->SetBrightness(1.3f);
	colorFilter->SetContrast(1.3f);
	colorFilter->SetContrast(0.8f);


	// ランクの決定
	float gameClearTime = UiGame::Instance().gameTimer;

	if(gameClearTime <= (60.0f * 2)) rank = static_cast<int>(Rank::S);
	else if(gameClearTime <= (60.0f * 3)) rank = static_cast<int>(Rank::A);
	else if(gameClearTime <= (60.0f * 5)) rank = static_cast<int>(Rank::B);
	else rank = static_cast<int>(Rank::C);
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

	resultTimer += Timer::Instance().DeltaTime();

    sprBlack->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, resultTimer, blackAlpha));
	sprTimeBoard->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, resultTimer, timeRankBoardAlpha));
	sprRankBoard->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, resultTimer, timeRankBoardAlpha));


	switch (static_cast<Rank>(rank))
	{
	case Rank::S:
		{
			sprRankS->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, resultTimer, rankAlpha));
			float scale = Easing::GetNowParam(Easing::OutBounce<float>, resultTimer, rankScale);
			sprRankS->SetScale({ scale, scale });
		}
		break;
	case Rank::A:
		{
			sprRankA->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, resultTimer, rankAlpha));
			float scale = Easing::GetNowParam(Easing::OutBounce<float>, resultTimer, rankScale);
			sprRankA->SetScale({ scale, scale });
		}
		break;
	case Rank::B:
		{
			sprRankB->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, resultTimer, rankAlpha));
			float scale = Easing::GetNowParam(Easing::OutBounce<float>, resultTimer, rankScale);
			sprRankB->SetScale({ scale, scale });
		}
		break;
	case Rank::C:
		{
			sprRankB->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, resultTimer, rankAlpha));
			float scale = Easing::GetNowParam(Easing::OutBounce<float>, resultTimer, rankScale);
			sprRankB->SetScale({ scale, scale });
		}
		break;
	}

	


	sprGoTitle->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, resultTimer, goTitleAlpha));

	if(resultTimer > resultTime)
	{
		if(InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Enter) ||
			InputManager::Instance().GetGamePadButtonPressed(GAMEPADBUTTON_STATE::a))
		{
			SceneManager::Instance().ChangeScene(new SceneTitle);
		}
	}
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

	//sprBackground->Render();
	ID3D11ShaderResourceView* useSrv;
		// ====== colorFilter =====
	if (colorFilter->GetIsColorFilter())
	{
		colorFilter->Make(capturedSrv.Get());
		useSrv = colorFilter->GetSrv();
	}
	capturedBackground->blit(&useSrv, 0, 1);
	sprRankBoard->Render();
	sprTimeBoard->Render();


	switch (static_cast<Rank>(rank))
	{
	case Rank::S: sprRankS->Render(); break;
	case Rank::A: sprRankA->Render(); break;
	case Rank::B: sprRankB->Render(); break;
	case Rank::C: sprRankC->Render(); break;
	}

	sprGoTitle->Render();
	sprBlack->Render();

	// ここで文字描画
	DamageTextManager::Instance().Render();

	// テキストデータの文字描画
	//DispString::Instance().Draw(L"てすとめっせーじ", {200, 50}, 50);


#if USE_IMGUI
	// --- デバッグGUI描画 ---
	DrawDebugGUI();
	colorFilter->DrawDebugGui();


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

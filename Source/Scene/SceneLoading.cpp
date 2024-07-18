#include "SceneLoading.h"
#include "SceneManager.h"
#include "../../Library/Input/InputManager.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Timer.h"

void SceneLoading::Initialize()
{
	bg = std::make_unique<Sprite>("Data/Texture/UserInterface/Loading/bg.png");
	

	sprite = std::make_unique<Sprite>("Data/Texture/slash.sprite");
	sprite->SetPos({ 640,360 });
	sprite->UpdateAnimation();

	// スレッド開始
	thread = new std::thread(LoadingThread, this);

	// スレッドの管理を放棄
	//thread->detach();
}

void SceneLoading::Finalize()
{
	if (thread->joinable()) {
		thread->join();		// スレッドが終わるまでまつ。(おわったら delete)
	}
	delete thread;
}

void SceneLoading::Update()
{
	sprite->UpdateAnimation();

	loadTimer += Timer::Instance().DeltaTime();
	if(loadTimer > loadTime)
	{
		return;
	}

	// 次のシーンの準備ができたらシーンを切り替える
	if(nextScene->IsReady())
	{
		SceneManager::Instance().ChangeScene(nextScene);
	}
}

void SceneLoading::Render()
{
	std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex()); // 排他制御

	// 必要なポインタ取得
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	// renderTargetの設定
	dc->OMSetRenderTargets(1, gfx->GetRTVAddress(), gfx->GetDSV());

	// imGuiBufferを使用しない場合はこっちを記述する
	{
		// renderTargetのクリア
		float c[4] = { 0,0,0,1 };
		dc->ClearRenderTargetView(gfx->GetRTV(), c);
		// depthStencilViewのクリア
		dc->ClearDepthStencilView(gfx->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}


	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	gfx->SetBlend(BLEND_STATE::ALPHA);

	bg->Render();
	sprite->Render();
}

// ローディングスレッド
void SceneLoading::LoadingThread(SceneLoading* scene)
{
	// COM関連の初期化でスレッド毎に呼ぶ必要がある
	CoInitialize(nullptr);

	// 次のシーンの初期化を行う
	scene->nextScene->Initialize();

	// スレッドが終わる前にCOM関連の終了化
	CoUninitialize();

	// 次のシーンの準備完了設定
	scene->nextScene->SetReady();
}

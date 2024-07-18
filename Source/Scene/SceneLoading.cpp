#include "SceneLoading.h"
#include "SceneManager.h"
#include "../../Library/Input/InputManager.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Timer.h"
#include "../../Library/Framework.h"
#include "../../Library/Particle/Particle.h"
#include "../../Library/Particle/EmitterManager.h"

void SceneLoading::Initialize()
{
	// --- buffer 系初期化 ---
	bitBlockTransfer = std::make_unique<FullScreenQuad>();
	frameBuffer = std::make_unique<FrameBuffer>(Framework::Instance().GetScreenWidthF(),
		Framework::Instance().GetScreenHeightF(), true);
	bloom = std::make_unique<Bloom>(Framework::Instance().GetScreenWidthF(), Framework::Instance().GetScreenHeightF());
	wbOitBuffer = std::make_unique<WbOitBuffer>(Framework::Instance().GetScreenWidthF(),
		Framework::Instance().GetScreenHeightF());
	colorFilter = std::make_unique<ColorFilter>(Framework::Instance().GetScreenWidthF(),
		Framework::Instance().GetScreenHeightF());
	colorFilter->SetIsColorFilter(true);
	colorFilter->SetBrightness(2.0f);
	swordTrailBuffer = std::make_unique<FrameBuffer>(Framework::Instance().GetScreenWidthF(),
		Framework::Instance().GetScreenHeightF());
	swordTrailBufferSub = std::make_unique<FrameBuffer>(Framework::Instance().GetScreenWidthF(),
		Framework::Instance().GetScreenHeightF());

	// particle
	Particle::Instance().Initialize();
	emitterPos = { 640, 360 };


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

	EmitterManager::Instance().Update();
	Particle::Instance().Update();

	EmitUpdate();

	sprite->UpdateAnimation();

	// loadTimer += Timer::Instance().DeltaTime();
	// if(loadTimer > loadTime)
	// {
	// 	return;
	// }

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

	// ====== 不透明描画 ======
	float c[4] = { 0,0,0,1 };
	frameBuffer->Clear(c);
	frameBuffer->Activate();
	{
		gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
		gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
		gfx->SetBlend(BLEND_STATE::ALPHA);

		// ここに不透明オブジェクトの描画
	}
	frameBuffer->DeActivate();

	// ====== 半透明描画 ======
	wbOitBuffer->Clear();
	wbOitBuffer->Activate(frameBuffer->depthStencilView.Get());
	{

		gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_TRUE_SOLID);

		// ここに半透明オブジェクトの描画
		Particle::Instance().ScreenRender();
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

	// ポストエフェクトをかけるたびにこれを更新する
	ID3D11ShaderResourceView* useSrv = frameBuffer->shaderResourceViews[0].Get();
	//bitBlockTransfer->blit(frameBuffer->shaderResourceViews[0].GetAddressOf(), 0, 1);

	// ====== colorFilter =====
	if (colorFilter->GetIsColorFilter())
	{
		colorFilter->Make(useSrv);
		useSrv = colorFilter->GetSrv();
	}

	// ====== ブルーム処理しての描画 ======
	bloom->Make(useSrv);
	ID3D11ShaderResourceView* shvs[2] =
	{
		useSrv,
		bloom->GetSrv()
	};
	bitBlockTransfer->blit(shvs, 0, 2, bloom->GetFinalPassPs());


	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	gfx->SetBlend(BLEND_STATE::ALPHA);

	bg->Render();
	//sprite->Render();
}

void SceneLoading::EmitUpdate()
{
	uiEmitterTimer += Timer::Instance().DeltaTime();

	while (uiEmitterTimer > uiEmitterTime)
	{
		uiEmitterTimer -= uiEmitterTime;

		Emitter* emitter0 = new Emitter();
		emitter0->position = {640, 360,0};
		emitter0->emitterData.duration = 5.0;
		emitter0->emitterData.looping = false;
		emitter0->emitterData.burstsTime = 0.1;
		emitter0->emitterData.burstsCount = 3;
		emitter0->emitterData.particleKind = pk_loadingParticle;
		emitter0->emitterData.particleLifeTimeMin = 2.0f;
		emitter0->emitterData.particleLifeTimeMax = 3.0f;
		emitter0->emitterData.particleSpeedMin = 5.0f;
		emitter0->emitterData.particleSpeedMax = 30.0f;
		emitter0->emitterData.particleSizeMin = { 5.0f, 10.0f };
		emitter0->emitterData.particleSizeMax = { 20.0f, 20.0f };
		emitter0->emitterData.particleColorMin = { 1.0, 2.0, 2.0, 1 };
		emitter0->emitterData.particleColorMax = { 1.0, 2.0, 2.0, 1 };
		emitter0->emitterData.particleGravity = 1;
		emitter0->emitterData.particleBillboardType = 0;
		emitter0->emitterData.particleTextureType = 0;
		emitter0->emitterData.burstsOneShot = 1;
		EmitterManager::Instance().Register(emitter0);
	}
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

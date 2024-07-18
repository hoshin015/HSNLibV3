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
	// --- buffer �n������ ---
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
	emitterPos = { 610, 360 };


	bg = std::make_unique<Sprite>("Data/Texture/UserInterface/Loading/bg.png");

	sprite = std::make_unique<Sprite>("Data/Texture/Effect/particle0.sprite");
	sprite->SetPos({ 620,360 });
	sprite->SetScale({ 0.5f,0.5f });
	sprite->SetColor({ 1.0,2.0,2.0,0.3f });
	sprite->UpdateAnimation();

	// �X���b�h�J�n
	thread = new std::thread(LoadingThread, this);

	// �X���b�h�̊Ǘ������
	//thread->detach();

	loadTimer = 0.0f;
}

void SceneLoading::Finalize()
{
	if (thread->joinable()) {
		thread->join();		// �X���b�h���I���܂ł܂B(��������� delete)
	}
	delete thread;
}

void SceneLoading::Update()
{

	EmitterManager::Instance().Update();
	Particle::Instance().Update();

	EmitUpdate();

	 loadTimer += Timer::Instance().DeltaTime();
	 if(loadTimer < loadTime)
	 {
	 	return;
	 }

	// ���̃V�[���̏������ł�����V�[����؂�ւ���
	if(nextScene->IsReady())
	{
		SceneManager::Instance().ChangeScene(nextScene);
	}
}

void SceneLoading::Render()
{
	//std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex()); // �r������

	// �K�v�ȃ|�C���^�擾
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	// renderTarget�̐ݒ�
	dc->OMSetRenderTargets(1, gfx->GetRTVAddress(), gfx->GetDSV());

	// imGuiBuffer���g�p���Ȃ��ꍇ�͂��������L�q����
	{
		// renderTarget�̃N���A
		dc->ClearRenderTargetView(gfx->GetRTV(), gfx->GetBgColor());
		// depthStencilView�̃N���A
		dc->ClearDepthStencilView(gfx->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	// ====== �s�����`�� ======
	float c[4] = { 0,0,0,1 };
	frameBuffer->Clear(c);
	frameBuffer->Activate();
	{
		gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
		gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
		gfx->SetBlend(BLEND_STATE::ALPHA);

		// �����ɕs�����I�u�W�F�N�g�̕`��
	}
	frameBuffer->DeActivate();

	// ====== �������`�� ======
	wbOitBuffer->Clear();
	wbOitBuffer->Activate(frameBuffer->depthStencilView.Get());
	{

		gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_TRUE_SOLID);

		// �����ɔ������I�u�W�F�N�g�̕`��
		Particle::Instance().ScreenRender();
	}
	wbOitBuffer->DeActivate();

	// ====== �s�����E�������̓��� ======
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

	// �|�X�g�G�t�F�N�g�������邽�тɂ�����X�V����
	ID3D11ShaderResourceView* useSrv = frameBuffer->shaderResourceViews[0].Get();
	//bitBlockTransfer->blit(frameBuffer->shaderResourceViews[0].GetAddressOf(), 0, 1);

	// ====== colorFilter =====
	if (colorFilter->GetIsColorFilter())
	{
		colorFilter->Make(useSrv);
		useSrv = colorFilter->GetSrv();
	}

	// ====== �u���[���������Ă̕`�� ======
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

	sprite->Render();
	bg->Render();
}

void SceneLoading::EmitUpdate()
{
	uiEmitterTimer += Timer::Instance().DeltaTime();

	while (uiEmitterTimer > uiEmitterTime)
	{
		uiEmitterTimer -= uiEmitterTime;

		Emitter* emitter0 = new Emitter();
		emitter0->position = {620, 360,0};
		emitter0->emitterData.duration = 5.0;
		emitter0->emitterData.looping = false;
		emitter0->emitterData.burstsTime = 0.1;
		emitter0->emitterData.burstsCount = 1;
		emitter0->emitterData.particleKind = pk_loadingParticle;
		emitter0->emitterData.particleLifeTimeMin = 2.0f;
		emitter0->emitterData.particleLifeTimeMax = 3.0f;
		emitter0->emitterData.particleSpeedMin = 40.0f;
		emitter0->emitterData.particleSpeedMax = 70.0f;
		emitter0->emitterData.particleSizeMin = { 5.0f, 10.0f };
		emitter0->emitterData.particleSizeMax = { 30.0f, 20.0f };
		emitter0->emitterData.particleColorMin = { 1.0, 2.0, 2.0, 1 };
		emitter0->emitterData.particleColorMax = { 1.0, 2.0, 2.0, 1 };
		emitter0->emitterData.particleGravity = 1;
		emitter0->emitterData.particleBillboardType = 0;
		emitter0->emitterData.particleTextureType = 0;
		emitter0->emitterData.burstsOneShot = 1;
		EmitterManager::Instance().Register(emitter0);
	}
}

// ���[�f�B���O�X���b�h
void SceneLoading::LoadingThread(SceneLoading* scene)
{
	// COM�֘A�̏������ŃX���b�h���ɌĂԕK�v������
	CoInitialize(nullptr);

	// ���̃V�[���̏��������s��
	scene->nextScene->Initialize();

	// �X���b�h���I���O��COM�֘A�̏I����
	CoUninitialize();

	// ���̃V�[���̏��������ݒ�
	scene->nextScene->SetReady();
}

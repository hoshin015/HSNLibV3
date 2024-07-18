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

	// �X���b�h�J�n
	thread = new std::thread(LoadingThread, this);

	// �X���b�h�̊Ǘ������
	//thread->detach();
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
	sprite->UpdateAnimation();

	// loadTimer += Timer::Instance().DeltaTime();
	// if(loadTimer > loadTime)
	// {
	// 	return;
	// }

	// ���̃V�[���̏������ł�����V�[����؂�ւ���
	if(nextScene->IsReady())
	{
		SceneManager::Instance().ChangeScene(nextScene);
	}
}

void SceneLoading::Render()
{
	std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex()); // �r������

	// �K�v�ȃ|�C���^�擾
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	// renderTarget�̐ݒ�
	dc->OMSetRenderTargets(1, gfx->GetRTVAddress(), gfx->GetDSV());

	// imGuiBuffer���g�p���Ȃ��ꍇ�͂��������L�q����
	{
		// renderTarget�̃N���A
		float c[4] = { 0,0,0,1 };
		dc->ClearRenderTargetView(gfx->GetRTV(), c);
		// depthStencilView�̃N���A
		dc->ClearDepthStencilView(gfx->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}


	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	gfx->SetBlend(BLEND_STATE::ALPHA);

	bg->Render();
	sprite->Render();
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

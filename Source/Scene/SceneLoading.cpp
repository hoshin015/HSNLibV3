#include "SceneLoading.h"
#include "SceneManager.h"
#include "../../Library/Input/InputManager.h"
#include "../../Library/Graphics/Graphics.h"

void SceneLoading::Initialize()
{
	sprite = std::make_unique<Sprite>("Data/Texture/slash.sprite");
	sprite->SetPos({ 1000,600 });
	sprite->UpdateAnimation();

	// �X���b�h�J�n
	thread = new std::thread(LoadingThread, this);

	// �X���b�h�̊Ǘ������
	//thread->detach();
}

void SceneLoading::Finalize()
{
	if (thread->joinable()) {
		thread->join();
	}
	delete thread;
}

void SceneLoading::Update()
{
	sprite->UpdateAnimation();

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
		dc->ClearRenderTargetView(gfx->GetRTV(), gfx->GetBgColor());
		// depthStencilView�̃N���A
		dc->ClearDepthStencilView(gfx->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}


	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	gfx->SetBlend(BLEND_STATE::ALPHA);

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

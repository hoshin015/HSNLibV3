#include "EffectManager.h"
#include "../Graphics/Graphics.h"
#include "../Timer.h"


// ������
void EffectManager::Initialize()
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11Device* device = gfx->GetDevice();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	// Effekseer �����_������
	effekseerRenderer = EffekseerRendererDX11::Renderer::Create(device, dc, 2048);

	// Effekseer �}�l�[�W���[����
	effekseerManager = Effekseer::Manager::Create(2048);

	// Effekseer �����_���̊e��ݒ�
	effekseerManager->SetSpriteRenderer(effekseerRenderer->CreateSpriteRenderer());
	effekseerManager->SetRibbonRenderer(effekseerRenderer->CreateRibbonRenderer());
	effekseerManager->SetRingRenderer(effekseerRenderer->CreateRingRenderer());
	effekseerManager->SetTrackRenderer(effekseerRenderer->CreateTrackRenderer());
	effekseerManager->SetModelRenderer(effekseerRenderer->CreateModelRenderer());

	// Effekseer ���ł̃��[�_�[�̐ݒ�
	effekseerManager->SetTextureLoader(effekseerRenderer->CreateTextureLoader());
	effekseerManager->SetModelLoader(effekseerRenderer->CreateModelLoader());
	effekseerManager->SetMaterialLoader(effekseerRenderer->CreateMaterialLoader());

	// Effekseer ��������W�n�Ōv�Z����
	effekseerManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
}

// �I����
void EffectManager::Finalize()
{
	// EffekseerManager �Ȃǂ��̓X�}�[�g�|�C���^�ɂ���Ĕj�������̂ŉ������Ȃ�
}

// �X�V����
void EffectManager::Update()
{
	// �G�t�F�N�g�X�V����
	effekseerManager->Update(Timer::Instance().DeltaTime() * 60.0f);
}



// �`�揈��
void EffectManager::Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	// �r���[���v���W�F�N�V�����s���Effekseer�����_���ɐݒ�
	effekseerRenderer->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&view));
	effekseerRenderer->SetProjectionMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&projection));

	// Effekseer �`��J�n
	effekseerRenderer->BeginRendering();

	// Effekseer �`����s
	// �}�l�[�W���[�P�ʂŕ`�悷�邽�ߕ`�揇�𐧌䂷��ꍇ�̓}�l�[�W���[�𕡐��쐬��Draw()�֐������s���鏇���Ő���ł�����
	effekseerManager->Draw();

	// Effekseer �`��I��
	effekseerRenderer->EndRendering();
}

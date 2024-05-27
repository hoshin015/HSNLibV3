#include "../Graphics/Graphics.h"
#include "Effect.h"
#include "EffectManager.h"


// ������
void Effect::Initialize()
{
	CreateEffect(EffectType::Hit1, "Data/Effect/Hit.efk");
	CreateEffect(EffectType::Move, "Data/Effect/move.efk");
	CreateEffect(EffectType::PowerUp, "Data/Effect/PowerUp.efk");
	CreateEffect(EffectType::Sleep, "Data/Effect/Sleep.efk");
	CreateEffect(EffectType::Test1, "Data/Effect/Test1.efk");
}

// �f�[�^�쐬
void Effect::CreateEffect(EffectType index, const char* filename)
{
	// �G�t�F�N�g��ǂݍ��ޑO�Ƀ��b�N����
	// --- �}���`�X���b�h��Effect���쐬�����deviceContext�𓯎��A�N�Z�X���ăt���[�Y����\�������邽�ߔr�����䂷��
	std::lock_guard<std::mutex>	lock(Graphics::Instance().GetMutex());

	// Effekseer �̃��\�[�X��ǂݍ���
	// Effekseer ��UTF-16�̃t�@�C���p�X�ȊO�͑Ή����Ă��Ȃ����ߕ����R�[�h�̕ϊ����K�v
	char16_t utf16Filename[256];
	Effekseer::ConvertUtf8ToUtf16(utf16Filename, 256, filename);

	// Effekseer::Manager ���擾
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	// Effekseer�G�t�F�N�g��ǂݍ���
	effekseerEffects[static_cast<int>(index)] = Effekseer::Effect::Create(effekseerManager, (EFK_CHAR*)utf16Filename);
}

// �Đ�
Effekseer::Handle Effect::Play(EffectType index, const DirectX::XMFLOAT3& position, DirectX::XMFLOAT3 angle, float scale)
{
	int effectIndex = static_cast<int>(index);

	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	Effekseer::Handle handle = effekseerManager->Play(effekseerEffects[effectIndex], position.x, position.y, position.z);
	effekseerManager->SetScale(handle, scale, scale, scale);
	DirectX::XMFLOAT3 radians = {
		DirectX::XMConvertToRadians(angle.x),
		DirectX::XMConvertToRadians(angle.y),
		DirectX::XMConvertToRadians(angle.z),
	};
	effekseerManager->SetRotation(handle, radians.x, radians.y, radians.z);
	return handle;
}

// ��~
void Effect::Stop(Effekseer::Handle handle)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	effekseerManager->StopEffect(handle);
}

// ���W�ݒ�
void Effect::SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	effekseerManager->SetLocation(handle, position.x, position.y, position.z);
}

// �X�P�[���ݒ�
void Effect::SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	effekseerManager->SetScale(handle, scale.x, scale.y, scale.z);
}

// �p�x�ݒ�
void Effect::SetAngle(Effekseer::Handle handle, const DirectX::XMFLOAT3& angle)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	effekseerManager->SetRotation(handle, angle.x, angle.y, angle.z);
}


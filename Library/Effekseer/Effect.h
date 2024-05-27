#pragma once

#include <DirectXMath.h>
#include <Effekseer.h>

enum class EffectType
{
	Hit1,
	Move,
	PowerUp,
	Sleep,
	Test1,
	LAST,
};

// �G�t�F�N�g
class Effect
{
private:
	Effect() {}
	~Effect() {}


public:
	static Effect& Instance()
	{
		static Effect instance;
		return instance;
	};


	// ������
	void Initialize();

	// �f�[�^�쐬
	void CreateEffect(EffectType index, const char* filename);

	// �Đ�
	Effekseer::Handle Play(EffectType index, const DirectX::XMFLOAT3& position, DirectX::XMFLOAT3 angle, float scale = 1.0f);

	// ��~
	void Stop(Effekseer::Handle handle);

	// ���W�ݒ�
	void SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position);

	// �X�P�[���ݒ�
	void SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale);

	// �p�x�ݒ�
	void SetAngle(Effekseer::Handle handle, const DirectX::XMFLOAT3& angle);

private:
	Effekseer::EffectRef effekseerEffects[static_cast<int>(EffectType::LAST)];
};
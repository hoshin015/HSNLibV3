#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>


class Emitter
{
public:
	Emitter();
	~Emitter() {}

	// �X�V
	void Update();

public:
	float				duration;					// �p�[�e�B�N���̐�������(���̎��Ԃ����G�~�b�^�[���@�\����)
	bool				looping;					// duration ���J��Ԃ����ǂ���
	float				startLifeTime;				// ��������p�[�e�B�N���̎���
	float				startSpeed;					// ��������p�[�e�B�N���̏���	
	DirectX::XMFLOAT2	startSize;					// ��������p�[�e�B�N���̃T�C�Y(��X x,y,z �Ɨ�������)
	DirectX::XMFLOAT4	startColor = { 1,1,1,1 };	// ��������p�[�e�B�N���̃J���[
	float				startKind;					// ��������p�[�e�B�N���̃^�C�v

	float				billboardType;	// billboard�̃^�C�v
	
	float rateOverTime;	// �p�[�e�B�N���̐����Ԋu(s)
	float rate;			// ��x�ɐ�������p�[�e�B�N����
	

	DirectX::XMFLOAT3	position = { 0,0,0 };	// �G�~�b�^�[�̃|�W�V����
	DirectX::XMFLOAT3	velocity = { 0,0,0 };

	float emitRateTimer;	// �p�[�e�B�N���̐����Ԋu���Ǘ�����p�̃^�C�}�[
	float emitLifeTimer;	// �������ꂽ�o�ߎ��Ԃ�����

	// �萔�o�b�t�@�p
	struct EmitterConstants
	{
		DirectX::XMFLOAT2 particleSize;
		float particleLifeTime;
		float particleKind;
		DirectX::XMFLOAT4 particleColor;
		DirectX::XMFLOAT3 emitterPosition;
		float billboardType;
	};
	EmitterConstants emitterConstant;

	Microsoft::WRL::ComPtr<ID3D11Buffer> emitterConstantBuffer;
};
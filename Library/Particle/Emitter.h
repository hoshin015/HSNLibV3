#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>


class Emitter
{
public:
	Emitter();

	~Emitter()
	{
	}

	// �X�V
	void Update();

public:
	// �G�~�b�^�[�̏��
	struct EmitterData
	{
		float duration;           // �p�[�e�B�N���̐�������(���̎��Ԃ����G�~�b�^�[���@�\����)
		bool  looping;            // duration ���J��Ԃ����ǂ���
		float burstsTime;         // �p�[�e�B�N���̐����Ԋu(s)
		float burstsCount;        // ��x�ɐ�������p�[�e�B�N����
		int   burstsOneShot = -1; // oneShot�ŌĂяo����

		// ��������p�[�e�B�N���̏��(�p�[�e�B�N�����������ɕK�v�ȏ��)
		float             particleLifeTimeMin;             // ��������p�[�e�B�N���̎���(�ŏ��l)
		float             particleLifeTimeMax;             // ��������p�[�e�B�N���̎���(�ő�l)
		float             particleSpeedMin;                // ��������p�[�e�B�N���̏���(�ŏ��l)
		float             particleSpeedMax;                // ��������p�[�e�B�N���̏���(�ő�l)
		DirectX::XMFLOAT2 particleSizeMin;                 // ��������p�[�e�B�N���̃T�C�Y(�ŏ��l)
		DirectX::XMFLOAT2 particleSizeMax;                 // ��������p�[�e�B�N���̃T�C�Y(�ő�l)
		DirectX::XMFLOAT4 particleColorMin = {1, 1, 1, 1}; // ��������p�[�e�B�N���̃J���[(�ŏ��l)
		DirectX::XMFLOAT4 particleColorMax = {1, 1, 1, 1}; // ��������p�[�e�B�N���̃J���[(�ő�l)
		float             particleGravity  = 0.0f;         // �d��(�������̗͂̑傫��)
		float             particleAngleMin;                // ��������p�[�e�B�N���̊p�x(�ŏ��l)
		float             particleAngleMax;                // ��������p�[�e�B�N���̊p�x(�ő�l)
		float             particleAddAngle = 0.0f;         // �P�b�Ԃɉ�]����p�x

		float particleKind;              // ��������p�[�e�B�N���̃^�C�v
		float particleBillboardType = 0; // billboard�̃^�C�v
		float particleTextureType;
	};

	EmitterData emitterData;


	DirectX::XMFLOAT3 position = {0, 0, 0}; // �G�~�b�^�[�̃|�W�V����
	DirectX::XMFLOAT3 velocity = {0, 0, 0}; // ��������p�[�e�B�N���� velocity (���K������ speed ����Z����)

	float emitRateTimer; // �p�[�e�B�N���̐����Ԋu���Ǘ�����p�̃^�C�}�[
	float emitLifeTimer; // �������ꂽ�o�ߎ��Ԃ�����

	float emitCount = 1; // ����������

	// �萔�o�b�t�@�p
	struct EmitterConstants
	{
		DirectX::XMFLOAT2 particleSizeMin;
		DirectX::XMFLOAT2 particleSizeMax;
		DirectX::XMFLOAT4 particleColorMin;
		DirectX::XMFLOAT4 particleColorMax;

		float             particleSpeedMin;
		float             particleSpeedMax;
		float             particleLifeTimeMin;
		float             particleLifeTimeMax;

		float             particleKind;
		float             particleAngleMin;
		float             particleAngleMax;
		float             particleAddAngle;

		float             particleGravity;
		float             particleBillboardType;
		float             particleTextureType;
		float             emitterPad1;

		DirectX::XMFLOAT3 emitterPosition;
		float             emitCount;
	};

	EmitterConstants emitterConstant;

	Microsoft::WRL::ComPtr<ID3D11Buffer> emitterConstantBuffer;
};

#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include "../2D/Sprite.h"

class Particle
{
private:
	Particle();

	~Particle()
	{
	}

public:
	static Particle& Instance()
	{
		static Particle instance;
		return instance;
	}

	const int MAX_PARTICLE = 500000;
	const int THREAD_NUM_X = 16;

	// RWStructuredBuffer �� GPU �Ƃ��Ƃ肷��\���̃f�[�^(�ۑ����Ă����ׂ��f�[�^�݂�����)
	struct ParticleData
	{
		DirectX::XMFLOAT3 position      = {0, 0, 0};    // �|�W�V����	
		DirectX::XMFLOAT3 velocity      = {0, 0, 0};    // �����x
		DirectX::XMFLOAT3 startVelocity = {0, 0, 0};    // �������̉����x
		float             friction      = 0.0f;         // ��R
		float             gravity       = 0.0f;         // �d��(�������̗͂̑傫��)
		float             angle         = 0.0f;         // �p�x
		float             addAngle      = 0.0f;         // �P�b�Ԃɉ�]����p�x
		DirectX::XMFLOAT2 scale         = {1, 1};       // �X�P�[��
		DirectX::XMFLOAT2 startScale    = {1, 1};       // �������̃X�P�[��
		DirectX::XMFLOAT4 color         = {1, 1, 1, 1}; // �F

		float lifeTime;              // �p�[�e�B�N���������ł��鎞��
		float lifeTimer;             //	�������ԗp�^�C�}�[	
		bool  isActive      = false; // ���̃p�[�e�B�N�����g�p�����ǂ���
		float kind          = 0;     // �p�[�e�B�N���̎��
		float billboardType = 0;     // billboard �̃^�C�v
		float textureType   = 0;     // texture �̎��
	};

	void Initialize();
	void Update();
	void Render();

	// ���w��̃p�[�e�B�N���N��
	void Emit(int num);

	// ���ݔ�A�N�e�B�u��Ԃ̃p�[�e�B�N�������擾
	int GetPoolBufferCount();

private:
	size_t particleCount     = 0;
	size_t freeParticleCount = MAX_PARTICLE;

	// �S�Ẵp�[�e�B�N�������o�b�t�@�[
	Microsoft::WRL::ComPtr<ID3D11Buffer>              particleBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particleBufferUav;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  particleBufferSrv;

	// ���g�p�̃p�[�e�B�N�������o�b�t�@�[
	Microsoft::WRL::ComPtr<ID3D11Buffer>              particlePoolBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particlePoolBufferUav;

	// ���g�p�̃o�b�t�@�[����l���擾����ǂݎ���p�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer> particlePoolReadBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>   vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>    pixelShader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometryShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>    inputLayout;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>  initCs;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>  updateCs;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>  emitCs;
	Microsoft::WRL::ComPtr<ID3D11Buffer>         constantBuffer;

	std::unique_ptr<Sprite> sprParticles;
};

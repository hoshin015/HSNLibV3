#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

// ����̃N���X��

// �^�C�}�[���
struct TimerConstants
{
	float deltaTime;
	float nowTime;
	DirectX::XMFLOAT2 pad;
};

// �J�������
struct CameraConstants
{
	DirectX::XMFLOAT4X4 viewProjection;
	DirectX::XMFLOAT4 cameraPosition;
};

// ���s�������
struct DirectionalLightData
{
	DirectX::XMFLOAT4 direction;	// ����
	DirectX::XMFLOAT4 color;		// �F
};
// �_�������
struct PointLightData
{
	DirectX::XMFLOAT4 position;		// ���W
	DirectX::XMFLOAT4 color;		// �F
	float range;					// �͈�
	DirectX::XMFLOAT3 pad;
};
// �_�����̍ő吔
static constexpr int PointLightMax = 8;
// �X�|�b�g���C�g���
struct SpotLightData
{
	DirectX::XMFLOAT4 position;		// ���W
	DirectX::XMFLOAT4 direction;	// ����
	DirectX::XMFLOAT4 color;		// �F
	float range;					// �͈�
	float innerCorn;				// �C���i�[�p�x�͈�
	float outerCorn;				// �A�E�^�[�p�x�͈�
	float pad;
};
// �X�|�b�g���C�g�̍ő吔
static constexpr int SpotLightMax = 8;

// ���C�g�p�萔
struct LightConstants
{
	DirectionalLightData directionalLightData;
	PointLightData pointLightData[PointLightMax];
	SpotLightData spotLightData[SpotLightMax];
	int pointLightCount = 0;
	int spotLightCount = 0;
	DirectX::XMFLOAT2 pad;
	DirectX::XMFLOAT4 ambientLightColor;
};


class RenderContext
{
private:
	RenderContext() {}
	~RenderContext() {}

public:
	static RenderContext& Instance()
	{
		static RenderContext instance;
		return instance;
	}

	void Initialize();

	CameraConstants cameraConstant;
	Microsoft::WRL::ComPtr<ID3D11Buffer> cameraConstantBuffer;

	LightConstants lightConstant;
	Microsoft::WRL::ComPtr<ID3D11Buffer> lightConstantBuffer;
	
	TimerConstants timerConstant;
	Microsoft::WRL::ComPtr<ID3D11Buffer> timerConstantBuffer;

};
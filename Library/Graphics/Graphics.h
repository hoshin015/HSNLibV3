#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <mutex>
#include "FrameBuffer.h"
#include "FullScreenQuad.h"

enum class SAMPLER_STATE
{
	POINT,
	LINEAR,
	ANISOTROPIC,
	TEXT_LINEAR,
	LINEAR_BORDER_BLACK,
	LINEAR_BORDER_WHITE,
	SHADOW,

	NUM,
};

enum class RASTERIZER_STATE
{
	CLOCK_FALSE_SOLID,
	CLOCK_FALSE_WIREFRAME,
	CLOCK_FALSE_CULL_NONE,
	CLOCK_FALSE_WIREFRAME_CULL_NONE,
	CLOCK_TRUE_SOLID,
	CLOCK_TRUE_WIREFRAME,
	CLOCK_TRUE_CULL_NONE,
	CLOCK_TRUE_WIREFRAME_CULL_NONE,

	NUM,
};

enum class BLEND_STATE
{
	NONE,
	ALPHA,
	ADD,
	MULTIPLY,

	PARTICLE_APLHA,
	WBOIT,
	WBOIT_FINAL,

	NUM,
};

enum class DEPTHSTENCIL_STATE
{
	ZT_ON_ZW_ON,
	ZT_ON_ZW_OFF,
	ZT_OFF_ZW_ON,
	ZT_OFF_ZW_OFF,

	NUM,
};



//--------------------------------------------------------------
//  Graphics
//--------------------------------------------------------------
class Graphics
{
private:
	Graphics() {}
	~Graphics() {}

public:
	static Graphics& Instance()
	{
		static Graphics instance;
		return instance;
	}

	// �~���[�e�b�N�X�擾
	std::mutex& GetMutex() { return mutex; }

	// ������
	void Initialize(HWND hwnd, int windowWidth, int windowHeight);

	// �o�b�t�@�؂�ւ�
	void Present();

	// depthStencil�̐ݒ�
	void SetDepthStencil(DEPTHSTENCIL_STATE state);
	// rasterizer�̐ݒ�
	void SetRasterizer(RASTERIZER_STATE state);
	// blend�̐ݒ�
	void SetBlend(BLEND_STATE state);

public:
	// �J���[�t�B���^�[���
	struct ColorFilter
	{
		float hueShift;
		float saturation;
		float brightness;
		float pad;
	};
	ColorFilter colorFilterConstant;

	struct ShadowMapData
	{
		DirectX::XMFLOAT4X4 lightViewProjection;				// ���C�g�r���[�v���W�F�N�V�����s��
		DirectX::XMFLOAT3 shadowColor = { 0.2f,0.2f,0.2f };		// �e�̐F
		float shadowBias = 0.0001f;								// �[�x��r�p�̃I�t�Z�b�g�l
	};
	ID3D11ShaderResourceView* shadowMap;						// �V���h�E�}�b�v�e�N�X�`��
	float shadowDrawRect = 200.0f;
	ShadowMapData shadowMapData;

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilStates[static_cast<size_t>(DEPTHSTENCIL_STATE::NUM)];
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStates[static_cast<size_t>(RASTERIZER_STATE::NUM)];
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendStates[static_cast<size_t>(BLEND_STATE::NUM)];
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerStates[static_cast<size_t>(SAMPLER_STATE::NUM)];

	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffers[8];
	std::unique_ptr<FrameBuffer> frameBuffers[8];
	std::unique_ptr<FullScreenQuad> bitBlockTransfer;
public:
	ID3D11Device* GetDevice() { return device.Get(); }
	ID3D11Device** GetDeviceAddress() { return device.GetAddressOf(); }
	ID3D11DeviceContext* GetDeviceContext() { return deviceContext.Get(); }
	ID3D11RenderTargetView* GetRTV() { return renderTargetView.Get(); }
	ID3D11RenderTargetView** GetRTVAddress() { return renderTargetView.GetAddressOf(); }
	ID3D11DepthStencilView* GetDSV() { return depthStencilView.Get(); }


	float* GetBgColor() { return bgcolor; }
private:
	std::mutex mutex;

	bool sync = false;								// fps �Œ�t���O
	float bgcolor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };	// �w�i�F

};
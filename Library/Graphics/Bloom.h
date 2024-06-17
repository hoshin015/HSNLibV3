#pragma once
#include <DirectXMath.h>
#include <memory>
#include "FrameBuffer.h"
#include "FullScreenQuad.h"
#include "../RegisterNum.h"

class Bloom
{
public:
	Bloom(uint32_t width, uint32_t height);
	~Bloom() {}

	void Make(ID3D11ShaderResourceView* shaderResourceView);

	void DrawDebugGui();

	// �ŏI�o�͌��ʂ�Ԃ�
	ID3D11ShaderResourceView* GetSrv() const { return glowExtractionBuffer->shaderResourceViews[0].Get(); }
	ID3D11ShaderResourceView** GetSrvAddress() const { return glowExtractionBuffer->shaderResourceViews[0].GetAddressOf(); }

	ID3D11PixelShader* GetFinalPassPs() { return finalPassPixelShader.Get(); }
private:
	// FullScreenQuad
	std::unique_ptr<FullScreenQuad> bitBlockTransfer;

	// FrameBuffer
	std::unique_ptr<FrameBuffer> glowExtractionBuffer;								// ���P�x���o�p�o�b�t�@
	std::unique_ptr<FrameBuffer> gaussianBuffers[GAUSSIAN_DOWNSAMPLING_COUNT][2];	// �K�E�V�A���t�B���^�p�o�b�t�@(�c���łQ�p�X)

	// PixelShader
	Microsoft::WRL::ComPtr<ID3D11PixelShader> finalPassPixelShader;

	Microsoft::WRL::ComPtr<ID3D11PixelShader> glowExtractionPs;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussianBlurDownSamplingPs;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussianBlurHorizontalPs;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussianBlurVerticalPs;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussianBlurUpSamplingPs;

	// ���P�x���o�萔�o�b�t�@
	struct LuminanceExtractionConstants
	{
		float threshould = 1.0f;
		float intensity = 2.0f;
		DirectX::XMFLOAT2 lumiPad;
	};
	LuminanceExtractionConstants luminanceExtractionConstants;
	Microsoft::WRL::ComPtr<ID3D11Buffer> luminanceExtractionConstantBuffer;

	// �K�E�V�A���t�B���^�p�萔�o�b�t�@
	static const int NUM_WEIGHTS = 8;
	struct GaussianConstants
	{
		float weights[NUM_WEIGHTS];
	};
	GaussianConstants gaussianConstants;
	Microsoft::WRL::ComPtr<ID3D11Buffer> gaussianConstantBuffer;

	// �K�E�V�A���֐��𗘗p���ďd�݃e�[�u�����v�Z����
	float gaussianPower = 16.0f;
	void CalcWeightsTableFromGaussian(float blurPower);
};
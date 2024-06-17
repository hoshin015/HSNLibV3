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

	// 最終出力結果を返す
	ID3D11ShaderResourceView* GetSrv() const { return glowExtractionBuffer->shaderResourceViews[0].Get(); }
	ID3D11ShaderResourceView** GetSrvAddress() const { return glowExtractionBuffer->shaderResourceViews[0].GetAddressOf(); }

	ID3D11PixelShader* GetFinalPassPs() { return finalPassPixelShader.Get(); }
private:
	// FullScreenQuad
	std::unique_ptr<FullScreenQuad> bitBlockTransfer;

	// FrameBuffer
	std::unique_ptr<FrameBuffer> glowExtractionBuffer;								// 高輝度抽出用バッファ
	std::unique_ptr<FrameBuffer> gaussianBuffers[GAUSSIAN_DOWNSAMPLING_COUNT][2];	// ガウシアンフィルタ用バッファ(縦横で２パス)

	// PixelShader
	Microsoft::WRL::ComPtr<ID3D11PixelShader> finalPassPixelShader;

	Microsoft::WRL::ComPtr<ID3D11PixelShader> glowExtractionPs;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussianBlurDownSamplingPs;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussianBlurHorizontalPs;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussianBlurVerticalPs;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussianBlurUpSamplingPs;

	// 高輝度抽出定数バッファ
	struct LuminanceExtractionConstants
	{
		float threshould = 1.0f;
		float intensity = 2.0f;
		DirectX::XMFLOAT2 lumiPad;
	};
	LuminanceExtractionConstants luminanceExtractionConstants;
	Microsoft::WRL::ComPtr<ID3D11Buffer> luminanceExtractionConstantBuffer;

	// ガウシアンフィルタ用定数バッファ
	static const int NUM_WEIGHTS = 8;
	struct GaussianConstants
	{
		float weights[NUM_WEIGHTS];
	};
	GaussianConstants gaussianConstants;
	Microsoft::WRL::ComPtr<ID3D11Buffer> gaussianConstantBuffer;

	// ガウシアン関数を利用して重みテーブルを計算する
	float gaussianPower = 16.0f;
	void CalcWeightsTableFromGaussian(float blurPower);
};
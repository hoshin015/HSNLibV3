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
	ID3D11ShaderResourceView* GetSrv() const { return finalPassBuffer->shaderResourceViews[0].Get(); }
	ID3D11ShaderResourceView** GetSrvAddress() const { return finalPassBuffer->shaderResourceViews[0].GetAddressOf(); }


private:
	// FullScreenQuad
	std::unique_ptr<FullScreenQuad> bitBlockTransfer;

	// FrameBuffer
	std::unique_ptr<FrameBuffer> luminanceExtractionBuffer;							// 高輝度抽出用バッファ
	std::unique_ptr<FrameBuffer> gaussianBuffers[GAUSSIAN_DOWNSAMPLING_COUNT][2];	// ガウシアンフィルタ用バッファ(縦横で２パス)
	std::unique_ptr<FrameBuffer> gaussianAvgBuffer;									// ガウシアンフィルタ用バッファ(平均値を保存する)
	std::unique_ptr<FrameBuffer> finalPassBuffer;									// 元画像に加算する用バッファ

	// VertexShader
	Microsoft::WRL::ComPtr<ID3D11VertexShader> gaussianBlurVertexShaders[2];

	// PixelShader
	Microsoft::WRL::ComPtr<ID3D11PixelShader> luminanceExtractionPixelShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussianBlurPixelShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussianBlurAvgPixelShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> finalPassPixelShader;

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
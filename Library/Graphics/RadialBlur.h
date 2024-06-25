#pragma once
#include <DirectXMath.h>
#include <memory>
#include "FrameBuffer.h"
#include "FullScreenQuad.h"
#include "../RegisterNum.h"

class RadialBlur
{
public:
	RadialBlur(uint32_t width, uint32_t height);
	~RadialBlur() {}

	void Make(ID3D11ShaderResourceView* shaderResourceView);

	void DrawDebugGui();

	// 最終出力結果を返す
	ID3D11ShaderResourceView* GetSrv() const { return radialBuffer->shaderResourceViews[0].Get(); }
	ID3D11ShaderResourceView** GetSrvAddress() const { return radialBuffer->shaderResourceViews[0].GetAddressOf(); }

	void SetIsRadial(bool isRadial) { this->isRadial = isRadial; }
	bool GetIsRadial() { return isRadial; }
private:
	bool isRadial = false;

	// FullScreenQuad
	std::unique_ptr<FullScreenQuad> bitBlockTransfer;

	// FrameBuffer
	std::unique_ptr<FrameBuffer> radialBuffer;

	// PixelShader
	Microsoft::WRL::ComPtr<ID3D11PixelShader> radialBlurPixelShader;

	// 高輝度抽出定数バッファ
	struct RadialBlurConstants
	{
		DirectX::XMFLOAT2 blurCenter = { 0.5, 0.5 };
		float blurPower = 0.0f;
		float samplingCount = 6.0f;
	};
	RadialBlurConstants radialBlurConstants;
	Microsoft::WRL::ComPtr<ID3D11Buffer> radialBlurConstantBuffer;
};
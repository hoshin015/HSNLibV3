#pragma once
#include <DirectXMath.h>
#include <memory>
#include "FrameBuffer.h"
#include "FullScreenQuad.h"
#include "../RegisterNum.h"

class ColorFilter
{
public:
	ColorFilter(uint32_t width, uint32_t height);
	~ColorFilter() {}

	void Make(ID3D11ShaderResourceView* shaderResourceView);

	void DrawDebugGui();

	// 最終出力結果を返す
	ID3D11ShaderResourceView* GetSrv() const { return colorFilterBuffer->shaderResourceViews[0].Get(); }
	ID3D11ShaderResourceView** GetSrvAddress() const { return colorFilterBuffer->shaderResourceViews[0].GetAddressOf(); }

	void SetIsColorFilter(bool isColorFilter) { this->isColorFilter = isColorFilter; }
	bool GetIsColorFilter() { return isColorFilter; }

	void SetHueShift(float hueShift) { this->colorFilterConstants.hueShift = hueShift; }
	void SetSaturation(float saturation) { this->colorFilterConstants.saturation = saturation; }
	void SetBrightness(float brightness) { this->colorFilterConstants.brightness = brightness; }

private:
	bool isColorFilter = false;

	// FullScreenQuad
	std::unique_ptr<FullScreenQuad> bitBlockTransfer;

	// FrameBuffer
	std::unique_ptr<FrameBuffer> colorFilterBuffer;

	// PixelShader
	Microsoft::WRL::ComPtr<ID3D11PixelShader> colorFilterPixelShader;

	// 定数バッファ
	struct ColorFilterConstants
	{
		float hueShift = 0.0f;
		float saturation = 1.0f;
		float brightness = 1.0f;
		float contrast = 1.0f;
	};
	ColorFilterConstants colorFilterConstants;
	Microsoft::WRL::ComPtr<ID3D11Buffer> colorFilterConstantBuffer;
};
#pragma once
#include <DirectXMath.h>
#include <memory>
#include "FrameBuffer.h"
#include "FullScreenQuad.h"
#include "../RegisterNum.h"

class HeatHaze
{
public:
	HeatHaze(uint32_t width, uint32_t height);
	~HeatHaze() {}

	void Make(ID3D11ShaderResourceView* shaderResourceView);

	void DrawDebugGui();

	// 最終出力結果を返す
	ID3D11ShaderResourceView* GetSrv() const { return heatHazeBuffer->shaderResourceViews[0].Get(); }
	ID3D11ShaderResourceView** GetSrvAddress() const { return heatHazeBuffer->shaderResourceViews[0].GetAddressOf(); }

	void SetIsHeatHaze(bool isHeatHaze) { this->isHeatHaze = isHeatHaze; }
	bool GetIsHeatHaze() { return isHeatHaze; }

	void SetShiftSpeed(float shiftSpeed) { this->heatHazeConstants.shiftSpeed = shiftSpeed; }
	void SetMaxShift(float maxShift) { this->heatHazeConstants.maxShift = maxShift; }

private:
	bool isHeatHaze = false;

	// FullScreenQuad
	std::unique_ptr<FullScreenQuad> bitBlockTransfer;

	// FrameBuffer
	std::unique_ptr<FrameBuffer> heatHazeBuffer;

	// PixelShader
	Microsoft::WRL::ComPtr<ID3D11PixelShader> heatHazePixelShader;

	// 定数バッファ
	struct HeatHazeConstants
	{
		float shiftSpeed = 0.04f;
		float maxShift = 0.015;
		DirectX::XMFLOAT2 heatHazePad;
	};
	HeatHazeConstants heatHazeConstants;
	Microsoft::WRL::ComPtr<ID3D11Buffer> heatHazeConstantBuffer;
};
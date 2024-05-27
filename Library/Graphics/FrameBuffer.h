#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <cstdint>
class FrameBuffer
{
public:
	FrameBuffer(uint32_t width, uint32_t height, bool useDepth = false);
	virtual ~FrameBuffer() = default;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViews[2];
	D3D11_VIEWPORT viewport;

	void Clear(const float r = 0, const  float g = 0, const float b = 0, const float a = 1, const float depth = 1);
	void Clear(const float* color, const float depth = 1);
	void Activate();
	void DeActivate();

private:
	UINT viewportCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	D3D11_VIEWPORT cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cachedRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cachedDepthStencilView;
};



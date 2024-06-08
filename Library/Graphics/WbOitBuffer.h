#pragma once

#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <cstdint>
class WbOitBuffer
{
public:
	WbOitBuffer(uint32_t width, uint32_t height);
	virtual ~WbOitBuffer() = default;

	// カラーとウェイト情報を保存するために２つレンダーターゲットを用意
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetViews[2];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViews[2];
	D3D11_VIEWPORT viewport;

	void Clear();
	void Activate(ID3D11DepthStencilView* useDepth);
	void DeActivate();

	void DrawDebugGui();

private:
	UINT viewportCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	D3D11_VIEWPORT cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cachedRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cachedDepthStencilView;
};



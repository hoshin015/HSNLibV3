#include "WbOitBuffer.h"
#include "Graphics.h"
#include "Shader.h"
#include "../ErrorLogger.h"
#include "../ImGui/ImGuiManager.h"

WbOitBuffer::WbOitBuffer(uint32_t width, uint32_t height)
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11Device* device = gfx->GetDevice();

	HRESULT hr = S_OK;

	// -------- RenderTarget [0]--------
	Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTargetBuffer;
	D3D11_TEXTURE2D_DESC texture2dDesc{};
	texture2dDesc.Width = width;
	texture2dDesc.Height = height;
	texture2dDesc.MipLevels = 1;
	texture2dDesc.ArraySize = 1;
	texture2dDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texture2dDesc.SampleDesc.Count = 1;
	texture2dDesc.SampleDesc.Quality = 0;
	texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2dDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texture2dDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texture2dDesc.MiscFlags = 0;
	hr = device->CreateTexture2D(&texture2dDesc, 0, renderTargetBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
	renderTargetViewDesc.Format = texture2dDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	hr = device->CreateRenderTargetView(renderTargetBuffer.Get(), &renderTargetViewDesc, renderTargetViews[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	//-------- ShaderResourceView [0] --------
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
	shaderResourceViewDesc.Format = texture2dDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(renderTargetBuffer.Get(), &shaderResourceViewDesc, shaderResourceViews[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// -------- RenderTarget [1]--------
	Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTargetBuffer1;
	D3D11_TEXTURE2D_DESC texture2dDesc1{};
	texture2dDesc1.Width = width;
	texture2dDesc1.Height = height;
	texture2dDesc1.MipLevels = 1;
	texture2dDesc1.ArraySize = 1;
	texture2dDesc1.Format = DXGI_FORMAT_R8_UNORM;
	texture2dDesc1.SampleDesc.Count = 1;
	texture2dDesc1.SampleDesc.Quality = 0;
	texture2dDesc1.Usage = D3D11_USAGE_DEFAULT;
	texture2dDesc1.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texture2dDesc1.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texture2dDesc1.MiscFlags = 0;
	hr = device->CreateTexture2D(&texture2dDesc1, 0, renderTargetBuffer1.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc1{};
	renderTargetViewDesc1.Format = texture2dDesc1.Format;
	renderTargetViewDesc1.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	hr = device->CreateRenderTargetView(renderTargetBuffer1.Get(), &renderTargetViewDesc1, renderTargetViews[1].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	//-------- ShaderResourceView [1] --------
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc1{};
	shaderResourceViewDesc1.Format = texture2dDesc1.Format;
	shaderResourceViewDesc1.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc1.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(renderTargetBuffer1.Get(), &shaderResourceViewDesc1, shaderResourceViews[1].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));


	// ------- viewport -------
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;


	// ------- ps ¶¬ -------
	CreatePsFromCso("Data/Shader/WbOitPS.cso", wbOitPixelShader.GetAddressOf());
}


void WbOitBuffer::Clear()
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	float clearColor[4] = { 0,0,0,0 };
	dc->ClearRenderTargetView(renderTargetViews[0].Get(), clearColor);
	float clearReveal[4] = { 1,0,0,0 };
	dc->ClearRenderTargetView(renderTargetViews[1].Get(), clearReveal);
}

void WbOitBuffer::Activate(ID3D11DepthStencilView* useDepth)
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	viewportCount = D3D10_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	dc->RSGetViewports(&viewportCount, cachedViewports);
	dc->OMGetRenderTargets(1, cachedRenderTargetView.ReleaseAndGetAddressOf(), cachedDepthStencilView.ReleaseAndGetAddressOf());
	dc->RSSetViewports(1, &viewport);
	ID3D11RenderTargetView* rtvs[2] =
	{
		renderTargetViews[0].Get(),
		renderTargetViews[1].Get(),
	};
	dc->OMSetRenderTargets(ARRAYSIZE(rtvs), rtvs, useDepth);

	// depthStencilState‚ÌÝ’è
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_OFF);
	// blendState‚ÌÝ’è
	gfx->SetBlend(BLEND_STATE::WBOIT);
}

void WbOitBuffer::DeActivate()
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	dc->RSSetViewports(viewportCount, cachedViewports);
	dc->OMSetRenderTargets(1, cachedRenderTargetView.GetAddressOf(), cachedDepthStencilView.Get());
}

void WbOitBuffer::DrawDebugGui()
{
	ImGui::Begin("WbOit");
	{
		ImGui::Image(shaderResourceViews[0].Get(), { 256, 256 });
		ImGui::Image(shaderResourceViews[1].Get(), { 256, 256 });
		ImGui::Image(shaderResourceViews[2].Get(), { 256, 256 });
	}
	ImGui::End();
}

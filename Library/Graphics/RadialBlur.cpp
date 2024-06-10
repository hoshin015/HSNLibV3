#include "RadialBlur.h"
#include "Graphics.h"
#include "Shader.h"
#include "../ErrorLogger.h"
#include "../../External/ImGui/imgui.h"

RadialBlur::RadialBlur(uint32_t width, uint32_t height)
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11Device* device = gfx->GetDevice();

	// FullScreenQuad ì¬
	bitBlockTransfer = std::make_unique<FullScreenQuad>();

	// FrameBuffer ì¬
	radialBuffer = std::make_unique<FrameBuffer>(width, height);

	// PixelShader ì¬
	CreatePsFromCso("Data/Shader/RadialBlurPS.cso", radialBlurPixelShader.GetAddressOf());

	// ConstantBuffer ì¬
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(RadialBlurConstants);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	HRESULT hr = device->CreateBuffer(&bufferDesc, nullptr, radialBlurConstantBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

void RadialBlur::Make(ID3D11ShaderResourceView* shaderResourceView)
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	// rasterizerState‚Ìİ’è
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	// depthStencilState‚Ìİ’è
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	// blendState‚Ìİ’è
	gfx->SetBlend(BLEND_STATE::ALPHA);

	// ===== radialBlurˆ— =====
	dc->UpdateSubresource(radialBlurConstantBuffer.Get(), 0, 0, &radialBlurConstants, 0, 0);
	dc->PSSetConstantBuffers(_radialBlurConstant, 1, radialBlurConstantBuffer.GetAddressOf());
	radialBuffer->Activate();
	bitBlockTransfer->blit(&shaderResourceView, 0, 1, radialBlurPixelShader.Get());
	radialBuffer->DeActivate();
}

void RadialBlur::DrawDebugGui()
{
	ImGui::Begin("RadialBlur");
	ImGui::SliderFloat2("blurCenter", &radialBlurConstants.blurCenter.x, 0.0f, 1.0f);
	ImGui::DragFloat("blurPower", &radialBlurConstants.blurPower, 0.01f, 0.0f, 0.2f);
	ImGui::DragFloat("samplingCount", &radialBlurConstants.samplingCount, 1, 1.0f, 12.0f);

	ImGui::Image(radialBuffer->shaderResourceViews[0].Get(), { 200, 200 });

	ImGui::End();
}

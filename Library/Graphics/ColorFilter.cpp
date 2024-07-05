#include "ColorFilter.h"
#include "Graphics.h"
#include "Shader.h"
#include "../ErrorLogger.h"
#include "../../External/ImGui/imgui.h"

ColorFilter::ColorFilter(uint32_t width, uint32_t height)
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11Device* device = gfx->GetDevice();

	// FullScreenQuad ì¬
	bitBlockTransfer = std::make_unique<FullScreenQuad>();

	// FrameBuffer ì¬
	colorFilterBuffer = std::make_unique<FrameBuffer>(width, height);

	// PixelShader ì¬
	CreatePsFromCso("Data/Shader/ColorFilterPS.cso", colorFilterPixelShader.GetAddressOf());

	// ConstantBuffer ì¬
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(ColorFilterConstants);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	HRESULT hr = device->CreateBuffer(&bufferDesc, nullptr, colorFilterConstantBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

void ColorFilter::Make(ID3D11ShaderResourceView* shaderResourceView)
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	// rasterizerState‚Ìİ’è
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	// depthStencilState‚Ìİ’è
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	// blendState‚Ìİ’è
	gfx->SetBlend(BLEND_STATE::ALPHA);

	// ===== colorFilterˆ— =====
	dc->UpdateSubresource(colorFilterConstantBuffer.Get(), 0, 0, &colorFilterConstants, 0, 0);
	dc->PSSetConstantBuffers(_colorFilterConstant, 1, colorFilterConstantBuffer.GetAddressOf());
	colorFilterBuffer->Activate();
	bitBlockTransfer->blit(&shaderResourceView, 0, 1, colorFilterPixelShader.Get());
	colorFilterBuffer->DeActivate();
}

void ColorFilter::DrawDebugGui()
{
	ImGui::Begin("ColorFilter");
	ImGui::Checkbox("IsColorFilter", &isColorFilter);
	ImGui::SliderFloat("hueShift", &colorFilterConstants.hueShift, 0.0f, 360.0f);
	ImGui::SliderFloat("saturation", &colorFilterConstants.saturation, 0.0f, 2.0f);
	ImGui::SliderFloat("brightness", &colorFilterConstants.brightness, 0.0f, 2.0f);
	ImGui::SliderFloat("contrast", &colorFilterConstants.contrast, 0.1f, 2.0f);

	ImGui::Image(colorFilterBuffer->shaderResourceViews[0].Get(), { 200, 200 });

	ImGui::End();
}

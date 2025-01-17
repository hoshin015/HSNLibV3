#include "ColorFilter.h"
#include "Graphics.h"
#include "Shader.h"
#include "../ErrorLogger.h"
#include "../../External/ImGui/imgui.h"

ColorFilter::ColorFilter(uint32_t width, uint32_t height)
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11Device* device = gfx->GetDevice();

	// FullScreenQuad 作成
	bitBlockTransfer = std::make_unique<FullScreenQuad>();

	// FrameBuffer 作成
	colorFilterBuffer = std::make_unique<FrameBuffer>(width, height);

	// PixelShader 作成
	CreatePsFromCso("Data/Shader/ColorFilterPS.cso", colorFilterPixelShader.GetAddressOf());

	// ConstantBuffer 作成
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

	// rasterizerStateの設定
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	// depthStencilStateの設定
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	// blendStateの設定
	gfx->SetBlend(BLEND_STATE::ALPHA);

	// ===== colorFilter処理 =====
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

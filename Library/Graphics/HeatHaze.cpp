#include "HeatHaze.h"
#include "Graphics.h"
#include "Shader.h"
#include "../ErrorLogger.h"
#include "../../External/ImGui/imgui.h"

HeatHaze::HeatHaze(uint32_t width, uint32_t height)
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11Device* device = gfx->GetDevice();

	// FullScreenQuad 作成
	bitBlockTransfer = std::make_unique<FullScreenQuad>();

	// FrameBuffer 作成
	heatHazeBuffer = std::make_unique<FrameBuffer>(width, height);

	// PixelShader 作成
	CreatePsFromCso("Data/Shader/HeatHazePS.cso", heatHazePixelShader.GetAddressOf());

	// ConstantBuffer 作成
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(HeatHazeConstants);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	HRESULT hr = device->CreateBuffer(&bufferDesc, nullptr, heatHazeConstantBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

void HeatHaze::Make(ID3D11ShaderResourceView* shaderResourceView)
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	// rasterizerStateの設定
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	// depthStencilStateの設定
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	// blendStateの設定
	gfx->SetBlend(BLEND_STATE::ALPHA);

	// ===== heatHaze処理 =====
	dc->UpdateSubresource(heatHazeConstantBuffer.Get(), 0, 0, &heatHazeConstants, 0, 0);
	dc->PSSetConstantBuffers(_heatHazeConstant, 1, heatHazeConstantBuffer.GetAddressOf());
	heatHazeBuffer->Activate();
	bitBlockTransfer->blit(&shaderResourceView, 0, 1, heatHazePixelShader.Get());
	heatHazeBuffer->DeActivate();
}

void HeatHaze::DrawDebugGui()
{
	ImGui::Begin("HeatHaze");
	ImGui::Checkbox("IsHeatHaze", &isHeatHaze);
	ImGui::DragFloat("shiftSpeed", &heatHazeConstants.shiftSpeed, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("maxShift", &heatHazeConstants.maxShift, 0.01f, 0.0f, 1.0f);

	ImGui::Image(heatHazeBuffer->shaderResourceViews[0].Get(), { 200, 200 });

	ImGui::End();
}

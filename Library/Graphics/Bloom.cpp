#include <vector>
#include "Bloom.h"
#include "Shader.h"
#include "Graphics.h"
#include "../ErrorLogger.h"
#include "../../External/ImGui/imgui.h"

Bloom::Bloom(uint32_t width, uint32_t height)
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11Device* device = gfx->GetDevice();

	// FullScreenQuad 作成
	bitBlockTransfer = std::make_unique<FullScreenQuad>();

	// フレームバッファの解像度をダウンサンプリング
	uint32_t downsampledWidth = width / 2;
	uint32_t downsampledHeight = height / 2;

	// FrameBuffer 作成
	glowExtractionBuffer = std::make_unique<FrameBuffer>(downsampledWidth, downsampledHeight);
	for (size_t downSamplingIndex = 0; downSamplingIndex < GAUSSIAN_DOWNSAMPLING_COUNT; downSamplingIndex++)
	{
		uint32_t dsWidth = downsampledWidth / (2 * downSamplingIndex + 1);
		uint32_t dsHeight = downsampledHeight / (2 * downSamplingIndex + 1);
		gaussianBuffers[downSamplingIndex][0] = std::make_unique<FrameBuffer>(dsWidth, dsHeight);
		gaussianBuffers[downSamplingIndex][1] = std::make_unique<FrameBuffer>(dsWidth, dsHeight);
	}

	// PixelShader 作成
	CreatePsFromCso("Data/Shader/BloomFinalPass_PS.cso", finalPassPixelShader.GetAddressOf());

	CreatePsFromCso("Data/Shader/GlowExtractionPS.cso", glowExtractionPs.GetAddressOf());
	CreatePsFromCso("Data/Shader/GaussianBlurDownSamplingPS.cso", gaussianBlurDownSamplingPs.GetAddressOf());
	CreatePsFromCso("Data/Shader/GaussianBlurHorizontalPS.cso", gaussianBlurHorizontalPs.GetAddressOf());
	CreatePsFromCso("Data/Shader/GaussianBlurVerticalPS.cso", gaussianBlurVerticalPs.GetAddressOf());
	CreatePsFromCso("Data/Shader/GaussianBlurUpSamplingPS.cso", gaussianBlurUpSamplingPs.GetAddressOf());

	// ConstantBuffer 作成
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(LuminanceExtractionConstants);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	HRESULT hr = device->CreateBuffer(&bufferDesc, nullptr, luminanceExtractionConstantBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

void Bloom::Make(ID3D11ShaderResourceView* shaderResourceView)
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	// rasterizerStateの設定
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	// depthStencilStateの設定
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	// blendStateの設定
	gfx->SetBlend(BLEND_STATE::ALPHA);

	// ===== 高輝度抽出 =====
	dc->UpdateSubresource(luminanceExtractionConstantBuffer.Get(), 0, 0, &luminanceExtractionConstants, 0, 0);
	dc->PSSetConstantBuffers(_luminanceExtractionConstant, 1, luminanceExtractionConstantBuffer.GetAddressOf());
	glowExtractionBuffer->Clear();
	glowExtractionBuffer->Activate();
	bitBlockTransfer->blit(&shaderResourceView, 0, 1, glowExtractionPs.Get());
	glowExtractionBuffer->DeActivate();

	// ===== ガウシアンフィルタ =====

	// ダウンサンプリング
	gaussianBuffers[0][0]->Activate();
	bitBlockTransfer->blit(glowExtractionBuffer->shaderResourceViews[0].GetAddressOf(), 0, 1, gaussianBlurDownSamplingPs.Get());
	gaussianBuffers[0][0]->DeActivate();

	// 横
	gaussianBuffers[0][1]->Activate();
	bitBlockTransfer->blit(gaussianBuffers[0][0]->shaderResourceViews[0].GetAddressOf(), 0, 1, gaussianBlurHorizontalPs.Get());
	gaussianBuffers[0][1]->DeActivate();

	// 縦
	gaussianBuffers[0][0]->Activate();
	bitBlockTransfer->blit(gaussianBuffers[0][1]->shaderResourceViews[0].GetAddressOf(), 0, 1, gaussianBlurVerticalPs.Get());
	gaussianBuffers[0][0]->DeActivate();


	for (size_t downSamplingIndex = 1; downSamplingIndex < GAUSSIAN_DOWNSAMPLING_COUNT; downSamplingIndex++)
	{
		// ダウンサンプリング
		gaussianBuffers[downSamplingIndex][0]->Activate();
		bitBlockTransfer->blit(gaussianBuffers[downSamplingIndex-1][0]->shaderResourceViews[0].GetAddressOf(), 0, 1, gaussianBlurDownSamplingPs.Get());
		gaussianBuffers[downSamplingIndex][0]->DeActivate();

		// Ping-pong gaussian blur
		// 横
		gaussianBuffers[downSamplingIndex][1]->Activate();
		bitBlockTransfer->blit(gaussianBuffers[downSamplingIndex][0]->shaderResourceViews[0].GetAddressOf(), 0, 1, gaussianBlurHorizontalPs.Get());
		gaussianBuffers[downSamplingIndex][1]->DeActivate();

		// 縦
		gaussianBuffers[downSamplingIndex][0]->Activate();
		bitBlockTransfer->blit(gaussianBuffers[downSamplingIndex - 1][0]->shaderResourceViews[0].GetAddressOf(), 0, 1, gaussianBlurVerticalPs.Get());
		gaussianBuffers[downSamplingIndex][0]->DeActivate();
	}

	// ダウンサンプリング
	glowExtractionBuffer->Activate();
	std::vector<ID3D11ShaderResourceView*> gaussianSrvs;
	for (size_t downSamplingIndex = 0; downSamplingIndex < GAUSSIAN_DOWNSAMPLING_COUNT; downSamplingIndex++)
	{
		gaussianSrvs.push_back(gaussianBuffers[downSamplingIndex][0]->shaderResourceViews[0].Get());
	}
	bitBlockTransfer->blit(gaussianSrvs.data(), 0, GAUSSIAN_DOWNSAMPLING_COUNT, gaussianBlurUpSamplingPs.Get());
	glowExtractionBuffer->DeActivate();
}

void Bloom::DrawDebugGui()
{
	ImGui::Begin("Bloom");
	ImGui::SliderFloat("threshould", &luminanceExtractionConstants.threshould, 0.0f, 1.0f);
	ImGui::SliderFloat("intensity", &luminanceExtractionConstants.intensity, 0.0f, 2.0f);

	float imgSize = 200;
	ImGui::Image(glowExtractionBuffer->shaderResourceViews[0].Get(), { imgSize, imgSize });
	for (int i = 0; i < GAUSSIAN_DOWNSAMPLING_COUNT; i++)
	{
		ImGui::Text((std::to_string(i) + u8" パス").c_str());
		ImGui::Image(gaussianBuffers[i][1]->shaderResourceViews[0].Get(), { imgSize, imgSize });
	}
	ImGui::End();
}

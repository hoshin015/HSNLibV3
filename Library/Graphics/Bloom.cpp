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

	// FrameBuffer 作成
	luminanceExtractionBuffer = std::make_unique<FrameBuffer>(width, height);
	for (size_t downSamplingIndex = 0; downSamplingIndex < GAUSSIAN_DOWNSAMPLING_COUNT; downSamplingIndex++)
	{
		gaussianBuffers[downSamplingIndex][0] = std::make_unique<FrameBuffer>(width / (2 * downSamplingIndex + 1), height);
		gaussianBuffers[downSamplingIndex][1] = std::make_unique<FrameBuffer>(width / (2 * downSamplingIndex + 1), height / (2 * downSamplingIndex + 1));
	}
	gaussianAvgBuffer = std::make_unique<FrameBuffer>(width, height);
	finalPassBuffer = std::make_unique<FrameBuffer>(width, height);

	// VertexShader 作成
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
	};
	CreateVsFromCso("Data/Shader/GaussianBlurX_VS.cso", gaussianBlurVertexShaders[0].ReleaseAndGetAddressOf(), nullptr, nullptr, 0);
	CreateVsFromCso("Data/Shader/GaussianBlurY_VS.cso", gaussianBlurVertexShaders[1].ReleaseAndGetAddressOf(), nullptr, nullptr, 0);

	// PixelShader 作成
	CreatePsFromCso("Data/Shader/LuminanceExtraction_PS.cso", luminanceExtractionPixelShader.GetAddressOf());
	CreatePsFromCso("Data/Shader/GaussianBlur_PS.cso", gaussianBlurPixelShader.GetAddressOf());
	CreatePsFromCso("Data/Shader/GaussianAvgPs.cso", gaussianBlurAvgPixelShader.GetAddressOf());
	CreatePsFromCso("Data/Shader/BloomFinalPass_PS.cso", finalPassPixelShader.GetAddressOf());

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
	

	bufferDesc.ByteWidth = sizeof(GaussianConstants);
	hr = device->CreateBuffer(&bufferDesc, nullptr, gaussianConstantBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// 重みテーブルを作成してガウシアンフィルタの定数バッファに入れておく
	CalcWeightsTableFromGaussian(2.0f);
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
	luminanceExtractionBuffer->Clear();
	luminanceExtractionBuffer->Activate();
	bitBlockTransfer->blit(&shaderResourceView, 0, 1, luminanceExtractionPixelShader.Get());
	luminanceExtractionBuffer->DeActivate();

	// ===== ガウシアンフィルタ =====
	CalcWeightsTableFromGaussian(gaussianPower);
	dc->UpdateSubresource(gaussianConstantBuffer.Get(), 0, 0, &gaussianConstants, 0, 0);
	dc->PSSetConstantBuffers(_gaussianConstant, 1, gaussianConstantBuffer.GetAddressOf());

	// 横
	//gaussianBuffers[0][0]->Clear();
	gaussianBuffers[0][0]->Activate();
	bitBlockTransfer->blit(luminanceExtractionBuffer->shaderResourceViews[0].GetAddressOf(), 0, 1, gaussianBlurPixelShader.Get(), gaussianBlurVertexShaders[0].Get());
	gaussianBuffers[0][0]->DeActivate();

	// 縦
	//gaussianBuffers[0][1]->Clear();
	gaussianBuffers[0][1]->Activate();
	bitBlockTransfer->blit(gaussianBuffers[0][0]->shaderResourceViews[0].GetAddressOf(), 0, 1, gaussianBlurPixelShader.Get(), gaussianBlurVertexShaders[1].Get());
	gaussianBuffers[0][1]->DeActivate();

	for (size_t downSamplingIndex = 1; downSamplingIndex < GAUSSIAN_DOWNSAMPLING_COUNT; downSamplingIndex++)
	{
		// 横
		//gaussianBuffers[downSamplingIndex][0]->Clear();
		gaussianBuffers[downSamplingIndex][0]->Activate();
		bitBlockTransfer->blit(gaussianBuffers[downSamplingIndex-1][1]->shaderResourceViews[0].GetAddressOf(), 0, 1, gaussianBlurPixelShader.Get(), gaussianBlurVertexShaders[0].Get());
		gaussianBuffers[downSamplingIndex][0]->DeActivate();

		// 縦
		//gaussianBuffers[downSamplingIndex][1]->Clear();
		gaussianBuffers[downSamplingIndex][1]->Activate();
		bitBlockTransfer->blit(gaussianBuffers[downSamplingIndex][0]->shaderResourceViews[0].GetAddressOf(), 0, 1, gaussianBlurPixelShader.Get(), gaussianBlurVertexShaders[1].Get());
		gaussianBuffers[downSamplingIndex][1]->DeActivate();
	}

	// 平均を取得
	std::vector<ID3D11ShaderResourceView*> gaussianSrvs;
	for (size_t downSamplingIndex = 0; downSamplingIndex < GAUSSIAN_DOWNSAMPLING_COUNT; downSamplingIndex++)
	{
		gaussianSrvs.push_back(gaussianBuffers[downSamplingIndex][1]->shaderResourceViews[0].Get());
	}
	gaussianAvgBuffer->Activate();
	bitBlockTransfer->blit(gaussianSrvs.data(), 0, GAUSSIAN_DOWNSAMPLING_COUNT, gaussianBlurAvgPixelShader.Get());
	gaussianAvgBuffer->DeActivate();

	// ===== ファイルナルパス（元画像に加算）=====
	ID3D11ShaderResourceView* shvs[2] =
	{
		shaderResourceView,
		gaussianAvgBuffer->shaderResourceViews[0].Get()
	};
	finalPassBuffer->Activate();
	bitBlockTransfer->blit(shvs, 0, 2, finalPassPixelShader.Get());
	finalPassBuffer->DeActivate();
}

void Bloom::DrawDebugGui()
{
	ImGui::Begin("Bloom");
	ImGui::SliderFloat("threshould", &luminanceExtractionConstants.threshould, 0.0f, 1.0f);
	ImGui::SliderFloat("intensity", &luminanceExtractionConstants.intensity, 0.0f, 10.0f);
	ImGui::DragFloat("gaussianPower", &gaussianPower, 0.1f, 0.1f, 16.0f);

	float imgSize = 200;
	ImGui::Image(luminanceExtractionBuffer->shaderResourceViews[0].Get(), { imgSize, imgSize });
	for (int i = 0; i < GAUSSIAN_DOWNSAMPLING_COUNT; i++)
	{
		ImGui::Text((std::to_string(i) + u8" パス").c_str());
		ImGui::Image(gaussianBuffers[i][1]->shaderResourceViews[0].Get(), { imgSize, imgSize });
	}
	ImGui::Text(u8"avgパス");
	ImGui::Image(gaussianAvgBuffer->shaderResourceViews[0].Get(), { imgSize, imgSize });
	ImGui::Text(u8"最終パス");
	ImGui::Image(finalPassBuffer->shaderResourceViews[0].Get(), { imgSize, imgSize });

	ImGui::End();
}


/// <summary>
/// ガウシアン関数を利用して重みテーブルを計算する
/// </summary>
/// <param name="blurPower">分散具合。この数値が大きくなると分散具合が強くなる</param>
void  Bloom::CalcWeightsTableFromGaussian(float blurPower)
{
	float total = 0;
	for (int i = 0; i < NUM_WEIGHTS; i++) {
		gaussianConstants.weights[i] = expf(-0.5f * (float)(i * i) / blurPower);
		total += 2.0f * gaussianConstants.weights[i];

	}
	// 規格化
	for (int i = 0; i < NUM_WEIGHTS; i++) {
		gaussianConstants.weights[i] /= total;
	}
}
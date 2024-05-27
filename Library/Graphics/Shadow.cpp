#include "Shadow.h"
#include "Graphics.h"
#include "Shader.h"
#include "../ErrorLogger.h"
#include "../ImGui/ImGuiManager.h"
#include "../3D/LightManager.h"
#include "../3D/Camera.h"

Shadow::Shadow(uint32_t width, uint32_t height)
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11Device* device = gfx->GetDevice();

	HRESULT hr = S_OK;

	for (int i = 0; i < SHADOWMAP_COUNT; i++)
	{
		D3D11_TEXTURE2D_DESC texture2dDesc{};
		texture2dDesc.Width = width;
		texture2dDesc.Height = height;
		texture2dDesc.MipLevels = 1;
		texture2dDesc.ArraySize = 1;
		texture2dDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		texture2dDesc.SampleDesc.Count = 1;
		texture2dDesc.SampleDesc.Quality = 0;
		texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
		texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		texture2dDesc.CPUAccessFlags = 0;
		texture2dDesc.MiscFlags = 0;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
		hr = device->CreateTexture2D(&texture2dDesc, 0, depthStencilBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Flags = 0;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
			hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, depthStencilViews[i].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	

		//-------- ShaderResourceView --------
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
		shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		hr = device->CreateShaderResourceView(depthStencilBuffer.Get(), &shaderResourceViewDesc, shaderResourceViews[i].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	}


	// ------- viewport -------
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// ------ vertexShader -------
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"BONES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
	};
	CreateVsFromCso("Data/Shader/ShadowMapCasterVS.cso", shadowVertexShader.ReleaseAndGetAddressOf(), shadowInputLayout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	CreateVsFromCso("Data/Shader/ShadowMapCasterStaticVS.cso", shadowStaticVertexShader.ReleaseAndGetAddressOf(), shadowInputLayout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));

	// ------- 定数バッファ ------
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(ShadowConstants);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	hr = device->CreateBuffer(&bufferDesc, nullptr, shadowConstant.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	bufferDesc.ByteWidth = sizeof(ShadowCasterConstants);
	hr = device->CreateBuffer(&bufferDesc, nullptr, shadowCasterConstant.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

Shadow::~Shadow()
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	// 次のシーンが影の使用をしない場合のために nullptr でテクスチャ更新
	ID3D11ShaderResourceView* srvs[SHADOWMAP_COUNT];
	for (int i = 0; i < SHADOWMAP_COUNT; ++i)
	{
		srvs[i] = nullptr;
	}
	dc->PSSetShaderResources(_shadowTexture, SHADOWMAP_COUNT, srvs);
}

void Shadow::Clear(float r, float g, float b, float a, float depth)
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	float color[4] = { r,g,b,a };
	for (auto& depthStencilView : depthStencilViews)
	{
		dc->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, 0);
	}
}

void Shadow::Activate(int index)
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	viewportCount = D3D10_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	dc->RSGetViewports(&viewportCount, cachedViewports);
	dc->OMGetRenderTargets(1, cachedRenderTargetView.ReleaseAndGetAddressOf(), cachedDepthStencilView.ReleaseAndGetAddressOf());
	dc->RSSetViewports(1, &viewport);
	dc->OMSetRenderTargets(0, nullptr, depthStencilViews[index].Get());

	///////////////////////////////////////////////////////////////
	// この区切りの中は一度の設定のみでいいが見やすさ的にここに書いている(変えれたら変えたい)

	// シャドウマップ分割エリア定義
	float splitAreaTable[] =
	{
		Camera::Instance().GetNearZ(),
		30.0f,							
		300.0f,
		500.0f,
		Camera::Instance().GetFarZ()
	};

	// カメラのパラメータ取得
	DirectX::XMVECTOR cameraFront = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&Camera::Instance().GetFront()));
	DirectX::XMVECTOR cameraRight = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&Camera::Instance().GetRight()));
	DirectX::XMVECTOR cameraUp = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&Camera::Instance().GetUp()));
	DirectX::XMVECTOR cameraPos = DirectX::XMLoadFloat3(&Camera::Instance().GetEye());

	// 平行光源からカメラ位置を作成し、そこから原点の位置を見るように視線行列を生成
	DirectX::XMFLOAT3 dir = LightManager::Instance().GetLight(0)->GetDirection();
	DirectX::XMVECTOR LightPosition = DirectX::XMLoadFloat3(&dir);
	LightPosition = DirectX::XMVectorScale(LightPosition, -250.0f);
	DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(
		LightPosition,
		DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	);

	// シャドウマップに描画したい範囲の射影行列を生成
	DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicLH(10000, 10000, 0.1f, 1000.0f);
	DirectX::XMMATRIX viewProjection = V * P;

	///////////////////////////////////////////////////////////////

	float nearDepth = splitAreaTable[index + 0];
	float farDepth = splitAreaTable[index + 1];

	// エリアを内包する視推台の8頂点を算出する
	DirectX::XMVECTOR vertex[8];
	{
		// エリア近平面の中心から上面までの距離を求める
		float nearY = tanf(Camera::Instance().GetFovY() / 2.0f) * nearDepth;
		// エリア近平面の中心から右面までの距離を求める
		float nearX = nearY * Camera::Instance().GetAspect();
		// エリア遠平面の中心から上面までの距離を求める
		float farY = tanf(Camera::Instance().GetFovY() / 2.0f) * farDepth;
		// エリア遠平面の中心から右面までの距離を求める
		float farX = farY * Camera::Instance().GetAspect();

		// エリア近平面の中心座標を求める
		DirectX::XMVECTOR nearPos = cameraPos + cameraFront * nearDepth;
		// エリア遠平面の中心座標を求める
		DirectX::XMVECTOR farPos = cameraPos + cameraFront * farDepth;

		// 8頂点を求める
		{
			// 近平面の右上
			vertex[0] = nearPos + cameraUp * nearY + cameraRight * nearX;
			// 近平面の左上
			vertex[1] = nearPos + cameraUp * nearY - cameraRight * nearX;
			// 近平面の右下
			vertex[2] = nearPos - cameraUp * nearY + cameraRight * nearX;
			// 近平面の左下
			vertex[3] = nearPos - cameraUp * nearY - cameraRight * nearX;
			// 遠平面の右上
			vertex[4] = farPos + cameraUp * farY + cameraRight * farX;
			// 遠平面の左上
			vertex[5] = farPos + cameraUp * farY - cameraRight * farX;
			// 遠平面の右下
			vertex[6] = farPos - cameraUp * farY + cameraRight * farX;
			// 遠平面の左下
			vertex[7] = farPos - cameraUp * farY - cameraRight * farX;
		}
	}
	// 8点を LightViewProjection 空間に変換して、最大値、最小値を求める
	DirectX::XMFLOAT3 vertexMin(FLT_MAX, FLT_MAX, FLT_MAX);
	DirectX::XMFLOAT3 vertexMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	for (auto& v : vertex)
	{
		DirectX::XMFLOAT3 p;
		DirectX::XMStoreFloat3(&p, DirectX::XMVector3TransformCoord(v, viewProjection));

		vertexMin.x = min(p.x, vertexMin.x);
		vertexMin.y = min(p.y, vertexMin.y);
		vertexMin.z = min(p.z, vertexMin.z);
		vertexMax.x = max(p.x, vertexMax.x);
		vertexMax.y = max(p.y, vertexMax.y);
		vertexMax.z = max(p.z, vertexMax.z);
	}

	// クロップ行列を求める
	float xScale = 2.0f / (vertexMax.x - vertexMin.x);
	float yScale = 2.0f / (vertexMax.y - vertexMin.y);
	float xOffset = -0.5f * (vertexMax.x + vertexMin.x) * xScale;
	float yOffset = -0.5f * (vertexMax.y + vertexMin.y) * yScale;
	DirectX::XMFLOAT4X4 clopMatrix;
	clopMatrix._11 = xScale;
	clopMatrix._12 = 0;
	clopMatrix._13 = 0;
	clopMatrix._14 = 0;
	clopMatrix._21 = 0;
	clopMatrix._22 = yScale;
	clopMatrix._23 = 0;
	clopMatrix._24 = 0;
	clopMatrix._31 = 0;
	clopMatrix._32 = 0;
	clopMatrix._33 = 1;
	clopMatrix._34 = 0;
	clopMatrix._41 = xOffset;
	clopMatrix._42 = yOffset;
	clopMatrix._43 = 0;
	clopMatrix._44 = 1;
	DirectX::XMMATRIX ClopMatrix = DirectX::XMLoadFloat4x4(&clopMatrix);
	DirectX::XMStoreFloat4x4(&shadowConstants.lightViewProjections[index], viewProjection * ClopMatrix);


	DirectX::XMStoreFloat4x4(&shadowCasterConstants.lightViewProjection, viewProjection * ClopMatrix);
	dc->UpdateSubresource(shadowCasterConstant.Get(), 0, 0, &shadowCasterConstants, 0, 0);
	dc->VSSetConstantBuffers(_shadowConstant, 1, shadowCasterConstant.GetAddressOf());
}

void Shadow::DeActivate()
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	dc->RSSetViewports(viewportCount, cachedViewports);
	dc->OMSetRenderTargets(1, cachedRenderTargetView.GetAddressOf(), cachedDepthStencilView.Get());
}

// シャドウマップ描画開始
void Shadow::UpdateShadowCasterBegin()
{
	// 必要なポインタ取得
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();
	// シェーダー設定
	dc->IASetInputLayout(shadowInputLayout.Get());
	dc->VSSetShader(shadowVertexShader.Get(), nullptr, 0);
	dc->PSSetShader(nullptr, nullptr, 0);
	// state 設定
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);	// デプスシャドウなので深度テストと深度書きこみをONにしておく
	gfx->SetBlend(BLEND_STATE::NONE);						// blend 不要
}

void Shadow::SetAnimatedShader()
{
	// 必要なポインタ取得
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();
	dc->VSSetShader(shadowVertexShader.Get(), nullptr, 0);
}

// staticオブジェクトシェーダー設定
void Shadow::SetStaticShader()
{
	// 必要なポインタ取得
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();
	dc->VSSetShader(shadowStaticVertexShader.Get(), nullptr, 0);
}

// テクスチャの設定
void Shadow::SetShadowTextureAndConstants()
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	// テクスチャ更新
	ID3D11ShaderResourceView* srvs[SHADOWMAP_COUNT];
	for (int i = 0; i < SHADOWMAP_COUNT; ++i)
	{
		(&shadowConstants.shadowBias.x)[i] = shadowBias[i];
		srvs[i] = shaderResourceViews[i].Get();
	}
	dc->PSSetShaderResources(_shadowTexture, SHADOWMAP_COUNT, srvs);

	// 定数バッファ更新
	dc->UpdateSubresource(shadowConstant.Get(), 0, 0, &shadowConstants, 0, 0);
	dc->VSSetConstantBuffers(_shadowConstant, 1, shadowConstant.GetAddressOf());
	dc->PSSetConstantBuffers(_shadowConstant, 1, shadowConstant.GetAddressOf());
}

void Shadow::DrawDebugGui()
{
	ImGui::Begin("Shadow");
	{
		ImGui::ColorEdit3("Color", &shadowConstants.shadowColor.x);
		ImGui::SliderFloat4("Bias", &shadowConstants.shadowBias.x, 0.0f, 1.0f);
		for (auto& shaderResourceView : shaderResourceViews)
		{
			ImGui::Image(shaderResourceView.Get(), { 200, 200 });
		}
		
	}
	ImGui::End();
}



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

	// ------- �萔�o�b�t�@ ------
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

	// ���̃V�[�����e�̎g�p�����Ȃ��ꍇ�̂��߂� nullptr �Ńe�N�X�`���X�V
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
	// ���̋�؂�̒��͈�x�̐ݒ�݂̂ł��������₷���I�ɂ����ɏ����Ă���(�ς��ꂽ��ς�����)

	// �V���h�E�}�b�v�����G���A��`
	float splitAreaTable[] =
	{
		Camera::Instance().GetNearZ(),
		30.0f,							
		300.0f,
		500.0f,
		Camera::Instance().GetFarZ()
	};

	// �J�����̃p�����[�^�擾
	DirectX::XMVECTOR cameraFront = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&Camera::Instance().GetFront()));
	DirectX::XMVECTOR cameraRight = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&Camera::Instance().GetRight()));
	DirectX::XMVECTOR cameraUp = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&Camera::Instance().GetUp()));
	DirectX::XMVECTOR cameraPos = DirectX::XMLoadFloat3(&Camera::Instance().GetEye());

	// ���s��������J�����ʒu���쐬���A�������猴�_�̈ʒu������悤�Ɏ����s��𐶐�
	DirectX::XMFLOAT3 dir = LightManager::Instance().GetLight(0)->GetDirection();
	DirectX::XMVECTOR LightPosition = DirectX::XMLoadFloat3(&dir);
	LightPosition = DirectX::XMVectorScale(LightPosition, -250.0f);
	DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(
		LightPosition,
		DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	);

	// �V���h�E�}�b�v�ɕ`�悵�����͈͂̎ˉe�s��𐶐�
	DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicLH(10000, 10000, 0.1f, 1000.0f);
	DirectX::XMMATRIX viewProjection = V * P;

	///////////////////////////////////////////////////////////////

	float nearDepth = splitAreaTable[index + 0];
	float farDepth = splitAreaTable[index + 1];

	// �G���A�����鎋�����8���_���Z�o����
	DirectX::XMVECTOR vertex[8];
	{
		// �G���A�ߕ��ʂ̒��S�����ʂ܂ł̋��������߂�
		float nearY = tanf(Camera::Instance().GetFovY() / 2.0f) * nearDepth;
		// �G���A�ߕ��ʂ̒��S����E�ʂ܂ł̋��������߂�
		float nearX = nearY * Camera::Instance().GetAspect();
		// �G���A�����ʂ̒��S�����ʂ܂ł̋��������߂�
		float farY = tanf(Camera::Instance().GetFovY() / 2.0f) * farDepth;
		// �G���A�����ʂ̒��S����E�ʂ܂ł̋��������߂�
		float farX = farY * Camera::Instance().GetAspect();

		// �G���A�ߕ��ʂ̒��S���W�����߂�
		DirectX::XMVECTOR nearPos = cameraPos + cameraFront * nearDepth;
		// �G���A�����ʂ̒��S���W�����߂�
		DirectX::XMVECTOR farPos = cameraPos + cameraFront * farDepth;

		// 8���_�����߂�
		{
			// �ߕ��ʂ̉E��
			vertex[0] = nearPos + cameraUp * nearY + cameraRight * nearX;
			// �ߕ��ʂ̍���
			vertex[1] = nearPos + cameraUp * nearY - cameraRight * nearX;
			// �ߕ��ʂ̉E��
			vertex[2] = nearPos - cameraUp * nearY + cameraRight * nearX;
			// �ߕ��ʂ̍���
			vertex[3] = nearPos - cameraUp * nearY - cameraRight * nearX;
			// �����ʂ̉E��
			vertex[4] = farPos + cameraUp * farY + cameraRight * farX;
			// �����ʂ̍���
			vertex[5] = farPos + cameraUp * farY - cameraRight * farX;
			// �����ʂ̉E��
			vertex[6] = farPos - cameraUp * farY + cameraRight * farX;
			// �����ʂ̍���
			vertex[7] = farPos - cameraUp * farY - cameraRight * farX;
		}
	}
	// 8�_�� LightViewProjection ��Ԃɕϊ����āA�ő�l�A�ŏ��l�����߂�
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

	// �N���b�v�s������߂�
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

// �V���h�E�}�b�v�`��J�n
void Shadow::UpdateShadowCasterBegin()
{
	// �K�v�ȃ|�C���^�擾
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();
	// �V�F�[�_�[�ݒ�
	dc->IASetInputLayout(shadowInputLayout.Get());
	dc->VSSetShader(shadowVertexShader.Get(), nullptr, 0);
	dc->PSSetShader(nullptr, nullptr, 0);
	// state �ݒ�
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);	// �f�v�X�V���h�E�Ȃ̂Ő[�x�e�X�g�Ɛ[�x�������݂�ON�ɂ��Ă���
	gfx->SetBlend(BLEND_STATE::NONE);						// blend �s�v
}

void Shadow::SetAnimatedShader()
{
	// �K�v�ȃ|�C���^�擾
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();
	dc->VSSetShader(shadowVertexShader.Get(), nullptr, 0);
}

// static�I�u�W�F�N�g�V�F�[�_�[�ݒ�
void Shadow::SetStaticShader()
{
	// �K�v�ȃ|�C���^�擾
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();
	dc->VSSetShader(shadowStaticVertexShader.Get(), nullptr, 0);
}

// �e�N�X�`���̐ݒ�
void Shadow::SetShadowTextureAndConstants()
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	// �e�N�X�`���X�V
	ID3D11ShaderResourceView* srvs[SHADOWMAP_COUNT];
	for (int i = 0; i < SHADOWMAP_COUNT; ++i)
	{
		(&shadowConstants.shadowBias.x)[i] = shadowBias[i];
		srvs[i] = shaderResourceViews[i].Get();
	}
	dc->PSSetShaderResources(_shadowTexture, SHADOWMAP_COUNT, srvs);

	// �萔�o�b�t�@�X�V
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



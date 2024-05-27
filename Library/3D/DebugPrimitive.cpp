#include "DebugPrimitive.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Shader.h"
#include "../ErrorLogger.h"

// ������
void DebugPrimitive::Initialize()
{
	// --- Graphics �擾 ---
	Graphics* gfx = &Graphics::Instance();
	ID3D11Device* device = gfx->GetDevice();

	HRESULT hr = S_OK;

	//--- ���_�V�F�[�_�[�I�u�W�F�N�g�Ɠ��̓��C�A�E�g�I�u�W�F�N�g�̐��� ---
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	CreateVsFromCso("./Data/Shader/GeometricPrimitive_VS.cso", vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));

	//--- �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐��� ---
	CreatePsFromCso("./Data/Shader/GeometricPrimitive_PS.cso", pixelShader.GetAddressOf());

	//--- �萔�o�b�t�@�I�u�W�F�N�g�̐��� ---
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(Constants);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	//--- �����b�V���쐬 ---
	CreateSphereMesh(1.0f, 16, 16);

	//--- �~�����b�V���쐬 ---
	CreateCylinderMesh(1.0f, 1.0f, 0.0f, 1.0f, 16, 1);
}

// �`����s
void DebugPrimitive::Render()
{
	// --- Graphics �擾 ---
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	// �V�F�[�_�[�ݒ�
	dc->IASetInputLayout(inputLayout.Get());

	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader.Get(), nullptr, 0);

	// �v���~�e�B�u�ݒ�
	UINT stride = sizeof(DirectX::XMFLOAT3);
	UINT offset = 0;
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// ���`��
	dc->IASetVertexBuffers(0, 1, sphereVertexBuffer.GetAddressOf(), &stride, &offset);
	for (const Sphere& sphere : spheres)
	{
		// ���[���h�s��쐬
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(sphere.radius, sphere.radius, sphere.radius);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(sphere.center.x, sphere.center.y, sphere.center.z);
		DirectX::XMMATRIX W = S * T;

		DirectX::XMFLOAT4X4 w;
		DirectX::XMStoreFloat4x4(&w, W);

		// �萔�o�b�t�@�X�V
		Constants data = { w, sphere.color };
		dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
		dc->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

		dc->Draw(sphereVertexCount, 0);
	}
	spheres.clear();

	// �~���`��
	dc->IASetVertexBuffers(0, 1, cylinderVertexBuffer.GetAddressOf(), &stride, &offset);
	for (const Cylinder& cylinder : cylinders)
	{
		// ���[���h�s��쐬
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(cylinder.radius, cylinder.height, cylinder.radius);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(cylinder.position.x, cylinder.position.y, cylinder.position.z);
		DirectX::XMMATRIX W = S * T;

		DirectX::XMFLOAT4X4 w;
		DirectX::XMStoreFloat4x4(&w, W);

		// �萔�o�b�t�@�X�V
		Constants data = { w, cylinder.color };
		dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
		dc->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

		dc->Draw(cylinderVertexCount, 0);
	}
	cylinders.clear();
}

// ���ǉ�
void DebugPrimitive::AddSphere(const DirectX::XMFLOAT3& center, float radius, const DirectX::XMFLOAT4& color)
{
	Sphere sphere;
	sphere.center = center;
	sphere.radius = radius;
	sphere.color = color;
	spheres.emplace_back(sphere);
}

// �~���ǉ�
void DebugPrimitive::AddCylinder(const DirectX::XMFLOAT3& position, float radius, float height, const DirectX::XMFLOAT4& color)
{
	Cylinder cylinder;
	cylinder.position = position;
	cylinder.radius = radius;
	cylinder.height = height;
	cylinder.color = color;
	cylinders.emplace_back(cylinder);
}

// �����b�V���쐬
void DebugPrimitive::CreateSphereMesh(float radius, int slices, int stacks)
{
	// --- Graphics �擾 ---
	Graphics* gfx = &Graphics::Instance();
	ID3D11Device* device = gfx->GetDevice();

	// --- ���_�f�[�^�쐬 ---
	sphereVertexCount = stacks * slices * 2 + slices * stacks * 2;
	std::unique_ptr<DirectX::XMFLOAT3[]> vertices = std::make_unique<DirectX::XMFLOAT3[]>(sphereVertexCount);

	float phiStep = DirectX::XM_PI / stacks;
	float thetaStep = DirectX::XM_2PI / slices;

	DirectX::XMFLOAT3* p = vertices.get();

	for (int i = 0; i < stacks; ++i)
	{
		float phi = i * phiStep;
		float y = radius * cosf(phi);
		float r = radius * sinf(phi);

		for (int j = 0; j < slices; ++j)
		{
			float theta = j * thetaStep;
			p->x = r * sinf(theta);
			p->y = y;
			p->z = r * cosf(theta);
			p++;

			theta += thetaStep;

			p->x = r * sinf(theta);
			p->y = y;
			p->z = r * cosf(theta);
			p++;
		}
	}

	thetaStep = DirectX::XM_2PI / stacks;
	for (int i = 0; i < slices; ++i)
	{
		DirectX::XMMATRIX M = DirectX::XMMatrixRotationY(i * thetaStep);
		for (int j = 0; j < stacks; ++j)
		{
			float theta = j * thetaStep;
			DirectX::XMVECTOR V1 = DirectX::XMVectorSet(radius * sinf(theta), radius * cosf(theta), 0.0f, 1.0f);
			DirectX::XMVECTOR P1 = DirectX::XMVector3TransformCoord(V1, M);
			DirectX::XMStoreFloat3(p++, P1);

			int n = (j + 1) % stacks;
			theta += thetaStep;

			DirectX::XMVECTOR V2 = DirectX::XMVectorSet(radius * sinf(theta), radius * cosf(theta), 0.0f, 1.0f);
			DirectX::XMVECTOR P2 = DirectX::XMVector3TransformCoord(V2, M);
			DirectX::XMStoreFloat3(p++, P2);
		}
	}

	// --- ���_�o�b�t�@�쐬 ---
	D3D11_BUFFER_DESC bufferDesc = {};
	D3D11_SUBRESOURCE_DATA subresourceData = {};

	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * sphereVertexCount);
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;	// D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	subresourceData.pSysMem = vertices.get();
	subresourceData.SysMemPitch = 0;
	subresourceData.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, sphereVertexBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

// �~�����b�V���쐬
void DebugPrimitive::CreateCylinderMesh(float radius1, float radius2, float start, float height, int slices, int stacks)
{
	// --- Graphics �擾 ---
	Graphics* gfx = &Graphics::Instance();
	ID3D11Device* device = gfx->GetDevice();


	// --- ���_�f�[�^�쐬 ---
	cylinderVertexCount = 2 * slices * (stacks + 1) + 2 * slices;
	std::unique_ptr<DirectX::XMFLOAT3[]> vertices = std::make_unique<DirectX::XMFLOAT3[]>(cylinderVertexCount);

	DirectX::XMFLOAT3* p = vertices.get();

	float stackHeight = height / stacks;
	float radiusStep = (radius2 - radius1) / stacks;

	// vertices of ring
	float dTheta = DirectX::XM_2PI / slices;

	for (int i = 0; i < slices; ++i)
	{
		int n = (i + 1) % slices;

		float c1 = cosf(i * dTheta);
		float s1 = sinf(i * dTheta);

		float c2 = cosf(n * dTheta);
		float s2 = sinf(n * dTheta);

		for (int j = 0; j <= stacks; ++j)
		{
			float y = start + j * stackHeight;
			float r = radius1 + j * radiusStep;

			p->x = r * c1;
			p->y = y;
			p->z = r * s1;
			p++;

			p->x = r * c2;
			p->y = y;
			p->z = r * s2;
			p++;
		}

		p->x = radius1 * c1;
		p->y = start;
		p->z = radius1 * s1;
		p++;

		p->x = radius2 * c1;
		p->y = start + height;
		p->z = radius2 * s1;
		p++;
	}

	// ���_�o�b�t�@
	{
		D3D11_BUFFER_DESC desc = {};
		D3D11_SUBRESOURCE_DATA subresourceData = {};

		desc.ByteWidth = static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * cylinderVertexCount);
		desc.Usage = D3D11_USAGE_IMMUTABLE;	// D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		subresourceData.pSysMem = vertices.get();
		subresourceData.SysMemPitch = 0;
		subresourceData.SysMemSlicePitch = 0;

		HRESULT hr = device->CreateBuffer(&desc, &subresourceData, cylinderVertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}
}

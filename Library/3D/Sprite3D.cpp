#include <sstream>
#include "Sprite3D.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Shader.h"
#include "../ErrorLogger.h"

// ��]
inline void  Rotate(float& x, float& y, float cx, float cy, float cos, float sin)
{
	x -= cx;
	y -= cy;

	float tx{ x }, ty{ y };
	x = cos * tx + -sin * ty;
	y = sin * tx + cos * ty;

	x += cx;
	y += cy;
};

Sprite3D::Sprite3D(const wchar_t* filename)
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11Device* device = gfx->GetDevice();

	//--- < ���_���̃Z�b�g > ---
	Vertex vertices[] =
	{
		{{ -1.0f,  1.0f, 0 }, { 1, 1, 1, 1 }, { 0, 0 }},	// Top Left			- [0]
		{{  1.0f,  1.0f, 0 }, { 1, 0, 0, 1 }, { 1, 0 }},	// Top Right		- [1]
		{{ -1.0f, -1.0f, 0 }, { 0, 1, 0, 1 }, { 0, 1 }},	// Bottom Left		- [2]
		{{  1.0f, -1.0f, 0 }, { 0, 0, 1, 1 }, { 1, 1 }},	// Bottom Right		- [3]
	};

	DWORD indices[] =
	{
		0, 1, 2,
		3, 2, 1
	};

	//--- < ���_�o�b�t�@�I�u�W�F�N�g�̐��� > ---
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subresourceData{};
	subresourceData.pSysMem = vertices;
	subresourceData.SysMemPitch = 0;
	subresourceData.SysMemSlicePitch = 0;
	HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, vertexBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	//--- < �C���f�b�N�X�o�b�t�@�I�u�W�F�N�g�̐��� > ---
	D3D11_BUFFER_DESC indexBufferDesc{};
	indexBufferDesc.ByteWidth = sizeof(vertices);
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexBufferData;
	indexBufferData.pSysMem = indices;
	hr = device->CreateBuffer(&indexBufferDesc, &indexBufferData, indexBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));


	//--- < ���_�V�F�[�_�[�I�u�W�F�N�g�Ɠ��̓��C�A�E�g�I�u�W�F�N�g�̐��� > ---
	const char* csoName{ "./Data/Shader/Sprite3D_VS.cso" };

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	CreateVsFromCso(csoName, vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), inputElementDesc, _countof(inputElementDesc));

	//--- < �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐��� > ---
	csoName = { "./Data/Shader/Sprite3D_PS.cso" };
	CreatePsFromCso(csoName, pixelShader.GetAddressOf());

	//--- < �摜�t�@�C���̃��[�h��shaderResourceView�̐����ƃe�N�X�`�����̎擾 > ---
	LoadTextureFromFile(filename, shaderResourceView.GetAddressOf(), &texture2dDesc);

	//--- �萔�o�b�t�@�I�u�W�F�N�g�̐��� ---
	D3D11_BUFFER_DESC constantBufferDesc{};
	constantBufferDesc.ByteWidth = sizeof(Constants);
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = device->CreateBuffer(&constantBufferDesc, nullptr, constantBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

}

Sprite3D::~Sprite3D()
{
}

void Sprite3D::Render(const DirectX::XMFLOAT4X4& world, DirectX::XMFLOAT4 color)
{
	Render(world, color, 0, 0, texture2dDesc.Width, texture2dDesc.Height);
}

void Sprite3D::Render(const DirectX::XMFLOAT4X4& world, DirectX::XMFLOAT4 color, float sx, float sy, float sw, float sh)
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	float dw = sw;
	float dh = sh;

	float w = 1;
	float h = 1;

	// �����̂ق����傫���ꍇ
	if (dw > dh)
	{
		w = 1;
		h = dh / dw;
	}
	// �c���̂ق����傫���ꍇ
	else
	{
		h = 1;
		w = dw / dh;
	}

	float x0 = (0 - w * 0.5f) * 2;
	float y0 = (0 - h * 0.5f) * -2;
	float x1 = (0 + w * 0.5f) * 2;
	float y1 = (0 - h * 0.5f) * -2;
	float x2 = (0 - w * 0.5f) * 2;
	float y2 = (0 + h * 0.5f) * -2;
	float x3 = (0 + w * 0.5f) * 2;
	float y3 = (0 + h * 0.5f) * -2;
	

	//--- < �e�N�Z�����W�n����UV���W�n�ւ̕ϊ� > ---
	float u0 = sx / texture2dDesc.Width;
	float v0 = sy / texture2dDesc.Height;
	float u1 = (sx + sw) / texture2dDesc.Width;
	float v1 = (sy + sh) / texture2dDesc.Height;

	//--- < �v�Z���ʂŒ��_�o�b�t�@�I�u�W�F�N�g���X�V���� > ---
	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mappedSubresource{};
	hr = dc->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	Vertex* vertices{ reinterpret_cast<Vertex*>(mappedSubresource.pData) };
	if (vertices != nullptr)
	{
		vertices[0].position = { x0 ,y0, 0 };
		vertices[1].position = { x1 ,y1, 0 };
		vertices[2].position = { x2 ,y2, 0 };
		vertices[3].position = { x3 ,y3, 0 };
		vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = color;

		vertices[0].texcoord = { u0,v0 };
		vertices[1].texcoord = { u1,v0 };
		vertices[2].texcoord = { u0,v1 };
		vertices[3].texcoord = { u1,v1 };
	}
	dc->Unmap(vertexBuffer.Get(), 0);

	//--- < ���X�^���C�U�̃o�C���h > ---
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_CULL_NONE);
	
	//--- < ���_�o�b�t�@�[�̃o�C���h > ---
	UINT stride{ sizeof(Vertex) };
	UINT offset{ 0 };
	dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	dc->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	//--- < �v���~�e�B�u�^�C�v����Ƀf�[�^�����Ɋւ�����̃o�C���h > ---
	dc->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//--- < ���̓��C�A�E�g�I�u�W�F�N�g�̃o�C���h > ---
	dc->IASetInputLayout(inputLayout.Get());

	//--- < �V�F�[�_�[�̃o�C���h > ---
	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader.Get(), nullptr, 0);

	Constants data = { world };
	dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
	dc->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	//--- < �V�F�[�_�[���\�[�X�̃o�C���h > ---
	dc->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());

	//--- < �v���~�e�B�u�̕`�� > ---
	dc->DrawIndexed(6, 0, 0);
}

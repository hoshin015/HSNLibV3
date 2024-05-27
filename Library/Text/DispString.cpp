#include <iostream>
#include <tchar.h>
#include "DispString.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Shader.h"
#include "../ErrorLogger.h"

//
//  �Q�l �F https://blog.jumtana.com/2007/09/vc-unicode.html#google_vignette
//  
// �����񂪂��ׂĔ��p���m�F����
BOOL IsAllHankaku(const TCHAR* pTarget, int nLen, int index)
{
	// �����̎�ʂ��擾����
	WORD* pType = new WORD[nLen];
	GetStringTypeEx(
		LOCALE_SYSTEM_DEFAULT    // ���j�R�[�h�Ńv���W�F�N�g�ł́A������������      
		, CT_CTYPE3                // �S�p�A���p�A�Ђ炪�ȓ��𔻒肷��^�C�v
		, pTarget                  // ����ΏۂƂȂ镶���z��
		, nLen                     // ����ΏۂƂȂ镶���z��̃T�C�Y
		, pType                    // ���茋�ʂ̔z��
	);
	// ���ʂ����ׂĔ��p�Ȃ� TRUE ��������
	BOOL isAllHankaku = TRUE;
	if (!(pType[index] & C3_HALFWIDTH)) {
		isAllHankaku = FALSE;
	}

	delete[] pType;
	return isAllHankaku;
}


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

// �R���X�g���N�^
DispString::DispString()
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11Device* device = gfx->GetDevice();

	//--- < ���_���̃Z�b�g > ---
	vertex vertices[] =
	{
		{{ -0.5f,  0.5f, 0 }, { 1, 1, 1, 1 }, { 0, 0 }},
		{{  0.5f,  0.5f, 0 }, { 1, 0, 0, 1 }, { 1, 0 }},
		{{ -0.5f, -0.5f, 0 }, { 0, 1, 0, 1 }, { 0, 1 }},
		{{  0.5f, -0.5f, 0 }, { 0, 0, 1, 1 }, { 1, 1 }},
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

	//--- < ���_�V�F�[�_�[�I�u�W�F�N�g�Ɠ��̓��C�A�E�g�I�u�W�F�N�g�̐��� > ---
	const char* csoName{ "./Data/Shader/FontTexture_VS.cso" };

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	CreateVsFromCso(csoName, vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), inputElementDesc, _countof(inputElementDesc));

	//--- < �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐��� > ---
	csoName = { "./Data/Shader/FontTexture_PS.cso" };
	CreatePsFromCso(csoName, pixelShader.GetAddressOf());


	// ���P�[���ݒ�
	std::wcin.imbue(std::locale("japanese"));
	std::wcout.imbue(std::locale("japanese"));
}

// �f�X�g���N�^
DispString::~DispString()
{
    auto iter = dictionary.begin();
    while (iter != dictionary.end())
    {
        delete iter->second;
        iter = dictionary.erase(iter);
    }
    dictionary.clear();
}

void DispString::Draw(const TCHAR* str, DirectX::XMFLOAT2 position, float size, TEXT_ALIGN align, DirectX::XMFLOAT4 color, bool outline, DirectX::XMFLOAT4 outlineColor, float outlineOffset)
{
    int num = _tcslen(str);

	float width = 0;
	float height = size;

	// --- �S�p���p���� ---
	std::wstring szTarget = std::wstring(str);

	for (int i = 0; i < num; ++i)
	{
		if (auto iter = dictionary.find((UINT)((TCHAR*)str[i])); iter != end(dictionary))
		{
			//exist
		}
		else
		{
			//not exists
			FontTexture* font = new FontTexture();
			font->Create((TCHAR*)&str[i], size);
			dictionary.emplace((UINT)((TCHAR*)str[i]), font);
		}

		// �S�p���p�`�F�b�N
		BOOL isAllHan = IsAllHankaku(szTarget.c_str(), szTarget.length(), i);
		width += (isAllHan) ? (size * 0.5f) : (size);
	}	

	float bufferW = getBufferW(align, width);		//�A���C�������g�ɂ�鉡�������l
	float bufferH = getBufferH(align, height);		//�A���C�������g�ɂ��c�������l

	width = 0;
	for (int i = 0; i < num; ++i)
	{
		// ��������e�N�X�`���`�揈��
		shaderResourceView = dictionary[(UINT)str[i]]->GetShaderResourceView();
		texture2dDesc = dictionary[(UINT)str[i]]->GetTexture2dDesc();

		// �S�p���p�`�F�b�N
		BOOL isAllHan = IsAllHankaku(szTarget.c_str(), szTarget.length(), i);
		float addWidth = (isAllHan) ? (size * 0.5f) : (size);

		// �A�E�g���C���`��
		if (outline)
		{
			Render(position.x + width + bufferW - outlineOffset, position.y + bufferH - outlineOffset, addWidth, size, outlineColor.x, outlineColor.y, outlineColor.z, outlineColor.w, 0);
			Render(position.x + width + bufferW + outlineOffset, position.y + bufferH - outlineOffset, addWidth, size, outlineColor.x, outlineColor.y, outlineColor.z, outlineColor.w, 0);
			Render(position.x + width + bufferW - outlineOffset, position.y + bufferH + outlineOffset, addWidth, size, outlineColor.x, outlineColor.y, outlineColor.z, outlineColor.w, 0);
			Render(position.x + width + bufferW + outlineOffset, position.y + bufferH + outlineOffset, addWidth, size, outlineColor.x, outlineColor.y, outlineColor.z, outlineColor.w, 0);
		}
		
		
		Render(position.x + width + bufferW, position.y + bufferH, addWidth, size, color.x, color.y, color.z, color.w, 0);
		width += addWidth;
	}
}

void DispString::Render(float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle)
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	//--- < �X�N���[��(�r���[�|�[�g)�̃T�C�Y���擾 > ---
	D3D11_VIEWPORT viewport{};
	UINT numViewports{ 1 };
	dc->RSGetViewports(&numViewports, &viewport);

	//--- < ��`�̊e���_�̈ʒu(�X�N���[�����W�n)���v�Z���� > ---

	// left top
	float x0{ dx };
	float y0{ dy };
	// right top
	float x1{ dx + dw };
	float y1{ dy };
	// left bottom
	float x2{ dx };
	float y2{ dy + dh };
	// right bottom
	float x3{ dx + dw };
	float y3{ dy + dh };

	//--- < ���_��]���� > ---

	// ��]�̒��S����`�̒��S�_�ɂ����ꍇ
	float cx = dx + dw * 0.5f;
	float cy = dy + dh * 0.5f;

	float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
	float sin{ sinf(DirectX::XMConvertToRadians(angle)) };

	Rotate(x0, y0, cx, cy, cos, sin);
	Rotate(x1, y1, cx, cy, cos, sin);
	Rotate(x2, y2, cx, cy, cos, sin);
	Rotate(x3, y3, cx, cy, cos, sin);


	//--- < �X�N���[�����W�n����NDC�֍��W�ϊ� > ---
	x0 = 2.0f * x0 / viewport.Width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / viewport.Height;
	x1 = 2.0f * x1 / viewport.Width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / viewport.Height;
	x2 = 2.0f * x2 / viewport.Width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / viewport.Height;
	x3 = 2.0f * x3 / viewport.Width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / viewport.Height;

	//--- < �v�Z���ʂŒ��_�o�b�t�@�I�u�W�F�N�g���X�V���� > ---
	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mappedSubresource{};
	hr = dc->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	vertex* vertices{ reinterpret_cast<vertex*>(mappedSubresource.pData) };
	if (vertices != nullptr)
	{
		vertices[0].position = { x0 ,y0, 0 };
		vertices[1].position = { x1 ,y1, 0 };
		vertices[2].position = { x2 ,y2, 0 };
		vertices[3].position = { x3 ,y3, 0 };
		vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r,g,b,a };

		vertices[0].texcoord = { 0, 0 };
		vertices[1].texcoord = { 1, 0 };
		vertices[2].texcoord = { 0, 1 };
		vertices[3].texcoord = { 1, 1 };
	}
	dc->Unmap(vertexBuffer.Get(), 0);

	//--- < ���_�o�b�t�@�[�̃o�C���h > ---
	UINT stride{ sizeof(vertex) };
	UINT offset{ 0 };
	dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	//--- < �v���~�e�B�u�^�C�v����Ƀf�[�^�����Ɋւ�����̃o�C���h > ---
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//--- < ���̓��C�A�E�g�I�u�W�F�N�g�̃o�C���h > ---
	dc->IASetInputLayout(inputLayout.Get());

	//--- < �V�F�[�_�[�̃o�C���h > ---
	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader.Get(), nullptr, 0);

	//--- < �V�F�[�_�[���\�[�X�̃o�C���h > ---
	dc->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());

	//--- < �v���~�e�B�u�̕`�� > ---
	dc->Draw(4, 0);
}

//���������p
float DispString::getBufferW(TEXT_ALIGN align, float width)
{
	switch (align)
	{
	case TEXT_ALIGN::UPPER_LEFT:
	case TEXT_ALIGN::MIDDLE_LEFT:
	case TEXT_ALIGN::LOWER_LEFT:
		return 0.0f;
		break;
	case TEXT_ALIGN::UPPER_MIDDLE:
	case TEXT_ALIGN::MIDDLE:
	case TEXT_ALIGN::LOWER_MIDDLE:
		return -width * 0.5f;
		break;
	case TEXT_ALIGN::UPPER_RIGHT:
	case TEXT_ALIGN::MIDDLE_RIGHT:
	case TEXT_ALIGN::LOWER_RIGHT:
		return -width;
	}
	return 0.0f;
}

//�c�������p
float DispString::getBufferH(TEXT_ALIGN align, float height)
{
	switch (align)
	{
	case TEXT_ALIGN::UPPER_LEFT:
	case TEXT_ALIGN::UPPER_MIDDLE:
	case TEXT_ALIGN::UPPER_RIGHT:
		return 0.0f;
		break;
	case TEXT_ALIGN::MIDDLE_LEFT:
	case TEXT_ALIGN::MIDDLE:
	case TEXT_ALIGN::MIDDLE_RIGHT:
		return -height * 0.5f;
		break;
	case TEXT_ALIGN::LOWER_LEFT:
	case TEXT_ALIGN::LOWER_MIDDLE:
	case TEXT_ALIGN::LOWER_RIGHT:
		return -height;
	}
	return 0.0f;
}



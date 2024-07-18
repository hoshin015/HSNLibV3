#include <filesystem>
#include <DDSTextureLoader.h>
#include <DirectXTex.h>
#include "Texture.h"
#include "../Graphics/Graphics.h"
#include "../ErrorLogger.h"

HRESULT LoadTextureFromFile(const wchar_t* filename, ID3D11ShaderResourceView** shaderResourceView, D3D11_TEXTURE2D_DESC* texture2dDesc)
{
	// Graphics �擾
	Graphics* gfx = &Graphics::Instance();
	ID3D11Device* device = gfx->GetDevice();

	HRESULT hr = S_OK;

	///////// �e�N�X�`���ǂݍ��� /////////

	//--- < �摜�t�@�C���̃��[�h��shaderResourceView�̐��� > ---
	ComPtr<ID3D11Resource> resource;

	auto it = resources.find(filename);
	if (it != resources.end())
	{
		*shaderResourceView = it->second.Get();
		(*shaderResourceView)->AddRef();
		(*shaderResourceView)->GetResource(resource.GetAddressOf());
	}
	else
	{
		// dds �t�@�C����
		std::filesystem::path ddsFilename(filename);
		ddsFilename.replace_extension("dds");
		// tga �t�@�C����
		std::filesystem::path tgaFilename(filename);
		tgaFilename.replace_extension("tga");
		if (std::filesystem::exists(ddsFilename.c_str()))
		{
			hr = CreateDDSTextureFromFile(device, ddsFilename.c_str(), resource.GetAddressOf(), shaderResourceView);
			_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
		}
		else if (std::filesystem::exists(tgaFilename.c_str()))
		{
			using namespace DirectX;

			TexMetadata meta;
			GetMetadataFromTGAFile(filename, meta);

			std::unique_ptr<ScratchImage> image(new ScratchImage);
			hr = LoadFromTGAFile(filename, &meta, *image);
			_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
			hr = CreateShaderResourceView(device, image->GetImages(), image->GetImageCount(), meta, shaderResourceView);
			_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
		}
		else
		{
			hr = CreateWICTextureFromFile(device, filename, resource.GetAddressOf(), shaderResourceView);
			_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
		}
		resources.insert(make_pair(filename, *shaderResourceView));
	}

	//--- < �e�N�X�`�����̎擾 > ---
	if (texture2dDesc)
	{
		ComPtr<ID3D11Texture2D> texture2d;
		(*shaderResourceView)->GetResource(&resource);
		hr = resource.Get()->QueryInterface(__uuidof(ID3D11Texture2D), (LPVOID*)&texture2d);

		//hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
		texture2d->GetDesc(texture2dDesc);
	}

	return hr;
}

HRESULT LoadTextureFromFile(const wchar_t* filename, ID3D11ShaderResourceView** shaderResourceView, D3D11_TEXTURE2D_DESC* texture2dDesc, ID3D11Texture2D** texture)
{
	// Graphics �擾
	Graphics* gfx = &Graphics::Instance();
	ID3D11Device* device = gfx->GetDevice();

	HRESULT hr = S_OK;

	///////// �e�N�X�`���ǂݍ��� /////////

	//--- < �摜�t�@�C���̃��[�h��shaderResourceView�̐��� > ---
	ComPtr<ID3D11Resource> resource;

	auto it = resources.find(filename);
	if (it != resources.end())
	{
		*shaderResourceView = it->second.Get();
		(*shaderResourceView)->AddRef();
		(*shaderResourceView)->GetResource(resource.GetAddressOf());
	}
	else
	{
		// dds �t�@�C����
		std::filesystem::path ddsFilename(filename);
		ddsFilename.replace_extension("dds");
		// dds �t�@�C����
		std::filesystem::path tgaFilename(filename);
		tgaFilename.replace_extension("tga");
		// hdr �t�@�C����
		std::filesystem::path hdrFilename(filename);
		tgaFilename.replace_extension("hdr");

		if (std::filesystem::exists(ddsFilename.c_str()))
		{
			hr = CreateDDSTextureFromFile(device, ddsFilename.c_str(), resource.GetAddressOf(), shaderResourceView);
			_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
		}
		else if (std::filesystem::exists(tgaFilename.c_str()))
		{
			using namespace DirectX;

			TexMetadata meta;
			GetMetadataFromTGAFile(filename, meta);

			std::unique_ptr<ScratchImage> image(new ScratchImage);
			hr = LoadFromTGAFile(filename, &meta, *image);
			_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
			hr = CreateShaderResourceView(device, image->GetImages(), image->GetImageCount(), meta, shaderResourceView);
			_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
		}
		else if (std::filesystem::exists(hdrFilename.c_str()))
		{
			using namespace DirectX;

			TexMetadata meta;
			GetMetadataFromHDRFile(filename, meta);

			std::unique_ptr<ScratchImage> image(new ScratchImage);
			hr = LoadFromHDRFile(filename, &meta, *image);
			_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
			hr = CreateShaderResourceView(device, image->GetImages(), image->GetImageCount(), meta, shaderResourceView);
			_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
		}
		else
		{
			hr = CreateWICTextureFromFile(device, filename, resource.GetAddressOf(), shaderResourceView);
			_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
		}
		resources.insert(make_pair(filename, *shaderResourceView));
	}
	

	

	//--- < �e�N�X�`�����̎擾 > ---
	ComPtr<ID3D11Texture2D> texture2d;
	(*shaderResourceView)->GetResource(&resource);
	hr = resource->QueryInterface(__uuidof(ID3D11Texture2D), (LPVOID*)&texture2d);

	///hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	texture2d->GetDesc(texture2dDesc);


	hr = resource->QueryInterface<ID3D11Texture2D>(texture);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));


	return hr;
}

HRESULT LoadTextureFromFile(const wchar_t* filename, ID3D11ShaderResourceView** shaderResourceView, DirectX::XMFLOAT2& size)
{
	// Graphics �擾
	Graphics* gfx = &Graphics::Instance();
	ID3D11Device* device = gfx->GetDevice();

	HRESULT hr = S_OK;

	///////// �e�N�X�`���ǂݍ��� /////////

	//--- < �摜�t�@�C���̃��[�h��shaderResourceView�̐��� > ---
	ComPtr<ID3D11Resource> resource;

	auto it = resources.find(filename);
	if (it != resources.end())
	{
		*shaderResourceView = it->second.Get();
		(*shaderResourceView)->AddRef();
		(*shaderResourceView)->GetResource(resource.GetAddressOf());
	}
	else
	{
		// dds �t�@�C����
		std::filesystem::path ddsFilename(filename);
		ddsFilename.replace_extension("dds");
		// tga �t�@�C����
		std::filesystem::path tgaFilename(filename);
		tgaFilename.replace_extension("tga");
		if (std::filesystem::exists(ddsFilename.c_str()))
		{
			hr = CreateDDSTextureFromFile(device, ddsFilename.c_str(), resource.GetAddressOf(), shaderResourceView);
			_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
		}
		else if (std::filesystem::exists(tgaFilename.c_str()))
		{
			using namespace DirectX;

			TexMetadata meta;
			GetMetadataFromTGAFile(filename, meta);

			std::unique_ptr<ScratchImage> image(new ScratchImage);
			hr = LoadFromTGAFile(filename, &meta, *image);
			_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
			hr = CreateShaderResourceView(device, image->GetImages(), image->GetImageCount(), meta, shaderResourceView);
			_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
		}
		else
		{
			hr = CreateWICTextureFromFile(device, filename, resource.GetAddressOf(), shaderResourceView);
			_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
		}
		resources.insert(make_pair(filename, *shaderResourceView));
	}


	//--- < �e�N�X�`�����̎擾 > ---
	ComPtr<ID3D11Texture2D> texture2d;
	D3D11_TEXTURE2D_DESC texture2dDesc;
	(*shaderResourceView)->GetResource(&resource);
	hr = resource->QueryInterface(__uuidof(ID3D11Texture2D), (LPVOID*)&texture2d);

	///hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	texture2d->GetDesc(&texture2dDesc);

	size = { static_cast<float>(texture2dDesc.Width), static_cast<float>(texture2dDesc.Height) };

	return hr;
}

void ReleaseAllTextures()
{
	resources.clear();
}

#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <map>
#include <string>

// �����e�N�X�`�����쐬����N���X

//--------------------------------------------------------------
//  FontTexture
//--------------------------------------------------------------
class FontTexture
{
private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	D3D11_TEXTURE2D_DESC texture2dDesc;
public:
	FontTexture();
	~FontTexture();
	void Create(TCHAR* c, int fontSize);	// �����e�N�X�`���̍쐬
	ID3D11ShaderResourceView* GetShaderResourceView() { return shaderResourceView.Get(); }
	D3D11_TEXTURE2D_DESC GetTexture2dDesc() { return texture2dDesc; }
};
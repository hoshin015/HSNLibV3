#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include "../2D/Sprite.h"


class SkyMap
{
public:
	SkyMap(const wchar_t* filename);
	~SkyMap();

	void Render();

private:
	std::unique_ptr<Sprite> sprSky;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseIemSrv;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> specularPmermSrv;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> lutGgxSrv;
};
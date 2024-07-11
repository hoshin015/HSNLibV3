#include "SkyMap.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Shader.h"
#include "../ErrorLogger.h"
#include "../Graphics/Texture.h"
#include "Camera.h"
#include "../RegisterNum.h"

SkyMap::SkyMap(const wchar_t* filename)
{
	Graphics*            gfx    = &Graphics::Instance();
	lock_guard lock(gfx->GetMutex());

	ID3D11Device*        device = gfx->GetDevice();
	ID3D11DeviceContext* dc     = gfx->GetDeviceContext();

	sprSky = std::make_unique<Sprite>("Data/Texture/skymap/country_club_4k/specular_pmrem.dds",
	                                  "./Data/Shader/SkyMap_PS.cso", "./Data/Shader/SkyMap_VS.cso", false);
	sprSky->SetSize({1280, 720});


	//--- < 画像ファイルのロードとshaderResourceViewの生成とテクスチャ情報の取得 > ---
	D3D11_TEXTURE2D_DESC texture2dDesc;
	LoadTextureFromFile(L"Data/Texture/skymap/country_club_4k/diffuse_iem.dds", diffuseIemSrv.GetAddressOf(),
	                    &texture2dDesc);
	LoadTextureFromFile(L"Data/Texture/skymap/country_club_4k/specular_pmrem.dds", specularPmermSrv.GetAddressOf(),
	                    &texture2dDesc);
	LoadTextureFromFile(L"Data/Texture/skymap/lut_ggx.dds", lutGgxSrv.GetAddressOf(), &texture2dDesc);

	dc->PSSetShaderResources(_IEM, 1, diffuseIemSrv.GetAddressOf());
	dc->PSSetShaderResources(_PMREM, 1, specularPmermSrv.GetAddressOf());
	dc->PSSetShaderResources(_LutGgx, 1, lutGgxSrv.GetAddressOf());
}

SkyMap::~SkyMap()
{
}

void SkyMap::Render()
{
	Graphics* gfx = &Graphics::Instance();
	gfx->SetBlend(BLEND_STATE::ALPHA);
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_CULL_NONE);

	sprSky->Render();
}

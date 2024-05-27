#include "SkyMap.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Shader.h"
#include "../ErrorLogger.h"
#include "../Graphics/Texture.h"
#include "Camera.h"
#include <vector>

SkyMap::SkyMap(const wchar_t* filename)
{
    Graphics* gfx = &Graphics::Instance();
    ID3D11Device* device = gfx->GetDevice();
   
    //--- < ���_�V�F�[�_�[�I�u�W�F�N�g�̐��� > ---
    const char* csoName{ "./Data/Shader/SkyMap_VS.cso" };
    CreateVsFromCso(csoName, vertexShader.GetAddressOf(), NULL, NULL, 0);

    //--- < �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐��� > ---
    csoName = { "./Data/Shader/SkyMap_PS.cso" };
    CreatePsFromCso(csoName, pixelShader.GetAddressOf());

    //--- < �摜�t�@�C���̃��[�h��shaderResourceView�̐����ƃe�N�X�`�����̎擾 > ---
    D3D11_TEXTURE2D_DESC texture2dDesc;
    LoadTextureFromFile(filename, shaderResourceView.GetAddressOf(), &texture2dDesc);
   
    //--- �萔�o�b�t�@�I�u�W�F�N�g�̐��� ---
    D3D11_BUFFER_DESC constantBufferDesc{};
    constantBufferDesc.ByteWidth = sizeof(Constants);
    constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    HRESULT hr = device->CreateBuffer(&constantBufferDesc, nullptr, constantBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

SkyMap::~SkyMap()
{
}

void SkyMap::Render()
{
    // --- Graphics �擾 ---
    Graphics* gfx = &Graphics::Instance();
    ID3D11DeviceContext* dc = gfx->GetDeviceContext();

    XMMATRIX viewProjectionM = XMLoadFloat4x4(&Camera::Instance().GetView()) * XMLoadFloat4x4(&Camera::Instance().GetProjection());
    DirectX::XMFLOAT4X4 viewProjection;
    DirectX::XMStoreFloat4x4(&viewProjection, viewProjectionM);

    Constants data;
    DirectX::XMStoreFloat4x4(&data.inverseViewProjection, DirectX::XMMatrixInverse(NULL, DirectX::XMLoadFloat4x4(&viewProjection)));
    dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
    dc->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
    dc->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

    //--- < ���X�^���C�U�̃o�C���h > ---
    gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_CULL_NONE);
    gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_OFF_ZW_OFF);

    dc->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    dc->IASetInputLayout(NULL);

    dc->VSSetShader(vertexShader.Get(), 0, 0);
    dc->PSSetShader(pixelShader.Get(), 0, 0);

    //--- < �V�F�[�_�[���\�[�X�̃o�C���h > ---
    dc->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());

    dc->Draw(4, 0);
}


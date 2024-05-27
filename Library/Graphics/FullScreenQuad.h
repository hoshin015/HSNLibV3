#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <cstdint>

class FullScreenQuad
{
public:
	FullScreenQuad();
	virtual ~FullScreenQuad() = default;

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> embeddedVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> embeddedPixelShader;

public:
	void blit(ID3D11ShaderResourceView** shaderResourceView, uint32_t startSlot, uint32_t numViews, ID3D11PixelShader* replacedPixelShader = nullptr, ID3D11VertexShader* replacedVertexShader = nullptr);
	void blit(ID3D11ShaderResourceView** shaderResourceView, ID3D11ShaderResourceView** bloomFilterViews, uint32_t startSlot, uint32_t numViews, ID3D11PixelShader* replacedPixelShader = nullptr, ID3D11VertexShader* replacedVertexShader = nullptr);
};
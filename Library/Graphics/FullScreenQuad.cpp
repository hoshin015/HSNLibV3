#include "FullScreenQuad.h"
#include "Shader.h"
#include "Graphics.h"
#include "../ErrorLogger.h"

FullScreenQuad::FullScreenQuad()
{
	CreateVsFromCso("Data/Shader/FullScreenQuad_VS.cso", embeddedVertexShader.ReleaseAndGetAddressOf(), nullptr, nullptr, 0);
	CreatePsFromCso("Data/Shader/FullScreenQuad_PS.cso", embeddedPixelShader.ReleaseAndGetAddressOf());
}

void FullScreenQuad::blit(ID3D11ShaderResourceView** shaderResourceView, uint32_t startSlot, uint32_t numViews, ID3D11PixelShader* replacedPixelShader, ID3D11VertexShader* replacedVertexShader)
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	dc->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	dc->IASetInputLayout(nullptr);

	replacedVertexShader ? dc->VSSetShader(replacedVertexShader, 0, 0) : dc->VSSetShader(embeddedVertexShader.Get(), 0, 0);
	replacedPixelShader ? dc->PSSetShader(replacedPixelShader, 0, 0) : dc->PSSetShader(embeddedPixelShader.Get(), 0, 0);

	dc->PSSetShaderResources(startSlot, numViews, shaderResourceView);
	dc->VSSetShaderResources(startSlot, numViews, shaderResourceView);
	dc->Draw(4, 0);
}

void FullScreenQuad::blit(ID3D11ShaderResourceView** shaderResourceView, ID3D11ShaderResourceView** bloomFilterViews, uint32_t startSlot, uint32_t numViews, ID3D11PixelShader* replacedPixelShader, ID3D11VertexShader* replacedVertexShader)
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	dc->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	dc->IASetInputLayout(nullptr);

	replacedVertexShader ? dc->VSSetShader(replacedVertexShader, 0, 0) : dc->VSSetShader(embeddedVertexShader.Get(), 0, 0);
	replacedPixelShader ? dc->PSSetShader(replacedPixelShader, 0, 0) : dc->PSSetShader(embeddedPixelShader.Get(), 0, 0);

	dc->PSSetShaderResources(startSlot, numViews, shaderResourceView);
	dc->VSSetShaderResources(startSlot, numViews, shaderResourceView);


	dc->Draw(4, 0);
}



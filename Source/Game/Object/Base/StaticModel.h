#pragma once
// --- Library ---
#include "../../Library/RegisterNum.h"
// --- Game ---
#include "BaseModel.h"

class StaticModel : public BaseModel
{
public:
	StaticModel(const char* filename);
	~StaticModel() = default;

	// モデル情報生成
	void CreateComObject();

	// 描画
	void Render(int instancing, DirectX::XMFLOAT4X4* instancingTransform ,float* emissivePowers, DirectX::XMFLOAT4* colors, DirectX::XMFLOAT4* uvScrolls, bool isShadow);

	struct Constants
	{
		DirectX::XMFLOAT4 materialColorKd;
		DirectX::XMFLOAT4 materialColorKs;
		DirectX::XMFLOAT4 materialColorKa;
		int instancingCount = 0;
		DirectX::XMFLOAT3 pad;
		DirectX::XMFLOAT4X4 transforms[MAX_INSTANCE];
		DirectX::XMFLOAT4 emissivePowers[MAX_INSTANCE];
		DirectX::XMFLOAT4 colors[MAX_INSTANCE];
		DirectX::XMFLOAT4 uvScrolls[MAX_INSTANCE];
	};

private:
	Constants data;

	Microsoft::WRL::ComPtr<ID3D11InputLayout>	inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		constantBuffer;


	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11VertexShader>> vertexShaderMap;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>>  pixelShaderMap;
};
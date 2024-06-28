#pragma once
#include <unordered_map>
// --- Library ---
#include "../../Library/RegisterNum.h"
// --- Game ---
#include "BaseModel.h"

class AnimatedModel : public BaseModel
{
public:
	AnimatedModel(const char* filename);
	~AnimatedModel() = default;

	// 描画
	void Render(DirectX::XMFLOAT4X4 world, ModelResource::KeyFrame* keyFrame, bool isShadow);

	// オブジェクトの生成(あとで変える気がする)
	void CreateComObject();

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11VertexShader>> vertexShaderMap;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>>  pixelShaderMap;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>                                   inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>                                        constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>                                        uvScrollConstantBuffer;

	struct Constants
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4   materialColorKd;
		DirectX::XMFLOAT4   materialColorKs;
		DirectX::XMFLOAT4   materialColorKa;
		DirectX::XMFLOAT4X4 boneTransforms[ModelResource::MAX_BONES] = {
			{
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1
			}
		};
		float emissivePower = 1.0f;
		float metalnessPower = 1.0f;
		float roughnessPower = -1.0f;
		float pad;
	};
	Constants data;

	// uvスクロール用定数バッファ
	struct UvScrollConstant
	{
		DirectX::XMFLOAT2 uvScrollValue = {0,0};
		DirectX::XMFLOAT2 pad;
	};
	UvScrollConstant uvScrollConstant;

private:
};

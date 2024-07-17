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

	// �`��
	void Render(DirectX::XMFLOAT4X4 world, ModelResource::KeyFrame* keyFrame, bool isShadow);
	void Render(DirectX::XMFLOAT4X4 world, ModelResource::KeyFrame* keyFrame, ID3D11PixelShader* ps);

	// �I�u�W�F�N�g�̐���(���Ƃŕς���C������)
	void CreateComObject();

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11VertexShader>> vertexShaderMap;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>>  pixelShaderMap;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>                                   inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>                                        constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>                                        uvScrollConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>                                        dissolveConstantBuffer;

	struct Constants
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4   materialColorKd = {1,1,1,1};
		DirectX::XMFLOAT4   materialColorKs = {1,1,1,1};
		DirectX::XMFLOAT4   materialColorKa = {1,1,1,1};
		DirectX::XMFLOAT4X4 boneTransforms[ModelResource::MAX_BONES] = {
			{
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1
			}
		};
		float emissivePower  = 1.0f;
		float metalnessPower = 1.0f;
		float roughnessPower = -1.0f;
		float pad;
	};

	Constants data;

	// uv�X�N���[���p�萔�o�b�t�@
	struct UvScrollConstant
	{
		DirectX::XMFLOAT2 uvScrollValue = {0, 0};
		DirectX::XMFLOAT2 pad;
	};

	UvScrollConstant uvScrollConstant;

	// �f�B�]���u�p�萔�o�b�t�@
	struct DissolveConstant
	{
		float             dissolveThreshold = 0.0f; // �f�B�]���u��
		float             edgeThreshold     = 1.0f;  // �G�b�W��臒l
		DirectX::XMFLOAT2 pad;
		DirectX::XMFLOAT4 edgeColor = {0.0, 0.0, 1.0, 1.0}; // �G�b�W�̐F
	};

	DissolveConstant dissolveConstant;


	// ���ʉ��̃t���O
private:
};

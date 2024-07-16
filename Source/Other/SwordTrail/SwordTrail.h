#pragma once
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include "../../../Library/Resource/Sprite/SpriteResource.h"

class SwordTrail
{
public:
	SwordTrail();
	~SwordTrail(){}

	void Update();

	void Render();

	void SetSwordPos(const DirectX::XMFLOAT3 headPos, const DirectX::XMFLOAT3 tailPos);

	void CreateBuffer();

	void Clear();


	struct PosBuffer
	{
		DirectX::XMFLOAT3 head;
		DirectX::XMFLOAT3 tail;
	};
	void CreateCurveVertex(std::vector<PosBuffer>& posBuffer);


private:

	std::vector<PosBuffer> posBuffers;

	std::shared_ptr<SpriteResource> spriteResource;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;

	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};
	std::vector<Vertex> vertex;

	float split = 3.0f;
};

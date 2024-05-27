#pragma once
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>

class DebugPrimitive
{
private:
	DebugPrimitive() {};
	~DebugPrimitive() {};

public:
	static DebugPrimitive& Instance()
	{
		static DebugPrimitive instance;
		return instance;
	}

	// ������
	void Initialize();

	// �`����s
	void Render();

	// ���ǉ�
	void AddSphere(const DirectX::XMFLOAT3& center, float radius, const DirectX::XMFLOAT4& color);

	// �~���ǉ�
	void AddCylinder(const DirectX::XMFLOAT3& position, float radius, float height, const DirectX::XMFLOAT4& color);

private:
	// �����b�V���쐬
	void CreateSphereMesh(float radius, int slices, int stacks);

	// �~�����b�V���쐬
	void CreateCylinderMesh(float radius1, float radius2, float start, float height, int slices, int stacks);

private:
	struct Sphere
	{
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT3	center;
		float				radius;
	};

	struct Cylinder
	{
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT3	position;
		float				radius;
		float				height;
	};
	struct Constants
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4	materialColor;
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> sphereVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> cylinderVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	std::vector<Sphere>		spheres;
	std::vector<Cylinder>	cylinders;

	UINT	sphereVertexCount = 0;
	UINT	cylinderVertexCount = 0;
};
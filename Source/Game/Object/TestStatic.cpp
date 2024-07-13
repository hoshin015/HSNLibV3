#include "TestStatic.h"

void TestStatic::Update()
{
	// p¨s—ñXV
	UpdateTransform();

	int bufferPos = 5;
	for (int i = 0; i < instanceNum; i++)
	{
		DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&transform);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(-(i%10)* bufferPos -10, 0, -(i/10)* bufferPos -10);
		DirectX::XMStoreFloat4x4(&m[i], W * T);
	}
}

void TestStatic::Render(bool isShadow)
{
	float e = 2;
	DirectX::XMFLOAT4 c = { 1,1,1,1 };
	DirectX::XMFLOAT4 uv = { 0.0f,0.0f,0.0f,0.0f};
	model->Render(instanceNum, m, &e, &c, &uv, isShadow);
}

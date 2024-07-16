#include "SwordTrail.h"
#include "../../../Library/Graphics/Graphics.h"
#include "../../Library/ErrorLogger.h"
#include "../../../Library/Graphics/Shader.h"
#include "../../../Library/RegisterNum.h"
#include "../../../Library/3D/ResourceManager.h"
#include "../../../Library/Math/Vector.h"

SwordTrail::SwordTrail()
{
	//--- < 頂点シェーダーオブジェクトと入力レイアウトオブジェクトの生成 > ---
	const char* csoName;
	csoName = { "./Data/Shader/Sprite3D_VS.cso" };

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	CreateVsFromCso(csoName, vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), inputElementDesc, _countof(inputElementDesc));

	//--- < ピクセルシェーダーオブジェクトの生成 > ---
	csoName = { "./Data/Shader/Sprite3D_PS.cso" };
	CreatePsFromCso(csoName, pixelShader.GetAddressOf());

	// リソースの取得
	spriteResource = ResourceManager::Instance().LoadSpriteResource("Data/Texture/Flow.png");
}

void SwordTrail::Update()
{
	//CreateCurveVertex(posBuffers);

	CreateBuffer();
}

void SwordTrail::Render()
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	if (vertex.empty()) return;

	//--- < 頂点バッファーのバインド > ---
	UINT stride{ sizeof(Vertex) };
	UINT offset{ 0 };
	dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	//--- < プリミティブタイプおよにデータ順序に関する情報のバインド > ---
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//--- < 入力レイアウトオブジェクトのバインド > ---
	dc->IASetInputLayout(inputLayout.Get());

	//--- < シェーダーのバインド > ---
	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader.Get(), nullptr, 0);

	//--- < シェーダーリソースのバインド > ---
	dc->PSSetShaderResources(_spriteTexture, 1, spriteResource->GetSrvAddres());

	//--- < プリミティブの描画 > ---
	dc->Draw(posBuffers.size(), 0);
}

void SwordTrail::SetSwordPos(const DirectX::XMFLOAT3 headPos, const DirectX::XMFLOAT3 tailPos)
{
	PosBuffer posBuffer;
	posBuffer.head = headPos;
	posBuffer.tail = tailPos;
	posBuffers.insert(posBuffers.begin(), posBuffer);	// 先頭にいれる
}

void SwordTrail::CreateBuffer()
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11Device* device = gfx->GetDevice();

	if (posBuffers.empty()) return;

	float amount = 1.0f / (posBuffers.size() - 1);
	float v = 0;
	vertex.clear();
	vertex.resize(posBuffers.size() * 2);
	for(int i = 0, j = 0; i < vertex.size() && j < posBuffers.size(); i+=2, ++j)
	{
		vertex[i].position = posBuffers[j].head;
		vertex[i].color = { 1,1,1,1 };
		vertex[i].texcoord = { 1.0f, v };
		vertex[i + 1].position = posBuffers[j].tail;
		vertex[i+1].color = { 1,1,1,1 };
		vertex[i].texcoord = { 0.0f, v };
		v += amount;
	}

	//--- < 頂点バッファオブジェクトの生成 > ---
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(Vertex) * std::size(vertex);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subresourceData{};
	subresourceData.pSysMem = vertex.data();
	subresourceData.SysMemPitch = 0;
	subresourceData.SysMemSlicePitch = 0;
	HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, vertexBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

void SwordTrail::CreateCurveVertex(std::vector<PosBuffer>& posBuffer)
{
	if (posBuffer.size() < 3 || split < 1) return;
	std::vector<PosBuffer> newPosBuffer;
	newPosBuffer.reserve(posBuffer.size() + (posBuffer.size() - 1) * split);
	const float amount = 1.0f / (split + 1);

	PosBuffer newPos;
	newPosBuffer.emplace_back(posBuffer.front());
	for(int i = 0; i < posBuffer.size() - 1; ++i)
	{
		using namespace DirectX;

		float ratio = amount;
		XMVECTOR p0Head = (i == 0) ? (XMLoadFloat3(&posBuffer[1].head) + XMLoadFloat3(&posBuffer[2].head)) * 0.5f : XMLoadFloat3(&posBuffer[i - 1].head);
		XMVECTOR p1Head = XMLoadFloat3(&posBuffer[i].head);
		XMVECTOR p2Head = XMLoadFloat3(&posBuffer[i + 1].head);
		XMVECTOR p3Head = (i == posBuffer.size() - 2) ? (p0Head + p2Head) * 0.5f : XMLoadFloat3(&posBuffer[i +2].head);

		XMVECTOR p0Tail = (i == 0) ? (XMLoadFloat3(&posBuffer[1].tail) + XMLoadFloat3(&posBuffer[2].tail)) * 0.5f : XMLoadFloat3(&posBuffer[i - 1].tail);
		XMVECTOR p1Tail = XMLoadFloat3(&posBuffer[i].tail);
		XMVECTOR p2Tail = XMLoadFloat3(&posBuffer[i + 1].head);
		XMVECTOR p3Tail = (i == posBuffer.size() - 2) ? (p0Tail + p2Tail) * 0.5f : XMLoadFloat3(&posBuffer[i + 2].tail);

		for(int j = 0; j < static_cast<int>(split - 1); ++j)
		{
			newPos = PosBuffer();

			XMStoreFloat3(&newPos.head, XMVectorCatmullRom(p0Head, p1Head, p2Head, p3Head, ratio));
			XMStoreFloat3(&newPos.tail, XMVectorCatmullRom(p0Tail, p1Tail, p2Tail, p3Tail, ratio));

			newPosBuffer.emplace_back(newPos);
			ratio += amount;
		}
		newPosBuffer.emplace_back(posBuffer[i + 1]);
	}
	posBuffer = newPosBuffer;
}

void SwordTrail::Clear()
{
	posBuffers.clear();
	vertex.clear();
}

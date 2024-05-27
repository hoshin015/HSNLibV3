#include <sstream>
#include "Sprite.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Shader.h"
#include "../ErrorLogger.h"
#include "../RegisterNum.h"
#include "../Timer.h"
#include "../3D/ResourceManager.h"


// 回転
inline void  Rotate(float& x, float& y, float cx, float cy, float cos, float sin)
{
	x -= cx;
	y -= cy;

	float tx{ x }, ty{ y };
	x = cos * tx + -sin * ty;
	y = sin * tx + cos * ty;

	x += cx;
	y += cy;
};

Sprite::Sprite(const char* filename, const char* pixelShaderPath, bool posZ1)
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11Device* device = gfx->GetDevice();

	//--- < 頂点情報のセット > ---
	Vertex vertices[] =
	{
		{{ -0.5f,  0.5f, 0 }, { 1, 1, 1, 1 }, { 0, 0 }},
		{{  0.5f,  0.5f, 0 }, { 1, 0, 0, 1 }, { 1, 0 }},
		{{ -0.5f, -0.5f, 0 }, { 0, 1, 0, 1 }, { 0, 1 }},
		{{  0.5f, -0.5f, 0 }, { 0, 0, 1, 1 }, { 1, 1 }},
	};

	//--- < 頂点バッファオブジェクトの生成 > ---
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subresourceData{};
	subresourceData.pSysMem = vertices;
	subresourceData.SysMemPitch = 0;
	subresourceData.SysMemSlicePitch = 0;
	HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, vertexBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	//--- < 頂点シェーダーオブジェクトと入力レイアウトオブジェクトの生成 > ---
	const char* csoName{ "./Data/Shader/SpriteVS.cso" };

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	CreateVsFromCso(csoName, vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), inputElementDesc, _countof(inputElementDesc));

	//--- < ピクセルシェーダーオブジェクトの生成 > ---
	if (pixelShaderPath == nullptr) pixelShaderPath = "./Data/Shader/SpritePS.cso";
	csoName = { pixelShaderPath };
	CreatePsFromCso(csoName, pixelShader.GetAddressOf());

	// リソースの取得
	spriteResource = ResourceManager::Instance().LoadSpriteResource(filename);

	size = spriteResource->GetSize();	// とりあえず画像全体のサイズをいれておく

	this->posZ1 = posZ1;
}

Sprite::~Sprite()
{
}

//------------------------------------------------------------
//	アニメーション更新関数
//------------------------------------------------------------
void Sprite::UpdateAnimation()
{
	// アニメーション用にサイズ変換(毎フレームする必要はない。アニメーションを変更するタイミングで一度だけでいい、あとで変更)
	SetSize({
			static_cast<float>(GetSpriteResource()->GetAnimations().at(GetCurrentAnimationIndex()).frameWidth),
			static_cast<float>(GetSpriteResource()->GetAnimations().at(GetCurrentAnimationIndex()).frameHeight)
	});
	SetPivotPoint({
		static_cast<float>(GetSpriteResource()->GetAnimations().at(GetCurrentAnimationIndex()).xPivotPoint),
		static_cast<float>(GetSpriteResource()->GetAnimations().at(GetCurrentAnimationIndex()).yPivotPoint)
	});



	animationTime += Timer::Instance().DeltaTime();

	// 現在のアニメーション取得
	SpriteResource::Animation& anim = spriteResource->GetAnimations().at(currentAnimationIndex);

	// 現在のフレーム取得
	int currentFrame = (animationTime / anim.secondsLength) * anim.frameNum - 1;
	
	if (animationTime >= anim.secondsLength)
	{
		if (animationLoop)
		{
			//currentFrame = 0;
			animationTime = 0;
		}
		else
		{
			animationTime = anim.secondsLength;
		}
	}
	
	texPos.x = (currentFrame % anim.framePerRow) * anim.frameWidth + (anim.xCellOffset * anim.frameWidth) + anim.xPixelOffset;
	texPos.y = (currentFrame / anim.framePerRow) * anim.frameHeight + (anim.yCellOffset * anim.frameHeight) + anim.yPixelOffset;
}

void Sprite::Render()
{
	if (!isRender) return;

	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	//--- < スクリーン(ビューポート)のサイズを取得 > ---
	D3D11_VIEWPORT viewport{};
	UINT numViewports{ 1 };
	dc->RSGetViewports(&numViewports, &viewport);

	// スケール処理
	DirectX::XMFLOAT2 scalingSize = { size.x * scale.x, size.y * scale.y};
	DirectX::XMFLOAT2 scalingPivot = { pivotPoint.x * scale.x, pivotPoint.y * scale.y };

	//--- < 矩形の各頂点の位置(スクリーン座標系)を計算する > ---

	// left top
	float x0{ position.x };
	float y0{ position.y };
	// right top
	float x1{ position.x + scalingSize.x };
	float y1{ position.y };
	// left bottom
	float x2{ position.x };
	float y2{ position.y + scalingSize.y };
	// right bottom
	float x3{ position.x + scalingSize.x };
	float y3{ position.y + scalingSize.y };

	//--- < 頂点回転処理 > ---

	// 回転の中心を矩形の中心点にした場合
	//float cx = position.x + scalingSize.x * 0.5f;
	//float cy = position.y + scalingSize.y * 0.5f;
	//
	//float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
	//float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
	//
	//Rotate(x0, y0, cx, cy, cos, sin);
	//Rotate(x1, y1, cx, cy, cos, sin);
	//Rotate(x2, y2, cx, cy, cos, sin);
	//Rotate(x3, y3, cx, cy, cos, sin);

	// pivotPoint を中心に回転
	float cx = position.x + scalingSize.x * (scalingPivot.x / scalingSize.x);
	float cy = position.y + scalingSize.y * (scalingPivot.y / scalingSize.y);
	
	float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
	float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
	
	Rotate(x0, y0, cx, cy, cos, sin);
	Rotate(x1, y1, cx, cy, cos, sin);
	Rotate(x2, y2, cx, cy, cos, sin);
	Rotate(x3, y3, cx, cy, cos, sin);

	// 回転後にpivotPos に対応させて座標移動
	x0 -= scalingPivot.x;
	x1 -= scalingPivot.x;
	x2 -= scalingPivot.x;
	x3 -= scalingPivot.x;
	y0 -= scalingPivot.y;
	y1 -= scalingPivot.y;
	y2 -= scalingPivot.y;
	y3 -= scalingPivot.y;


	//--- < スクリーン座標系からNDCへ座標変換 > ---
	x0 = 2.0f * x0 / viewport.Width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / viewport.Height;
	x1 = 2.0f * x1 / viewport.Width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / viewport.Height;
	x2 = 2.0f * x2 / viewport.Width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / viewport.Height;
	x3 = 2.0f * x3 / viewport.Width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / viewport.Height;


	//--- < テクセル座標系からUV座標系への変換 > ---
	float u0{ texPos.x / spriteResource->GetSize().x };
	float v0{ texPos.y / spriteResource->GetSize().y };
	float u1{ (texPos.x + size.x) / spriteResource->GetSize().x };
	float v1{ (texPos.y + size.y) / spriteResource->GetSize().y };

	//--- < 計算結果で頂点バッファオブジェクトを更新する > ---
	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mappedSubresource{};
	hr = dc->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	Vertex* vertices{ reinterpret_cast<Vertex*>(mappedSubresource.pData) };
	if (vertices != nullptr)
	{
		float zP = (posZ1) ? 1: 0;

		vertices[0].position = { x0 ,y0, zP };
		vertices[1].position = { x1 ,y1, zP };
		vertices[2].position = { x2 ,y2, zP };
		vertices[3].position = { x3 ,y3, zP };
		vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { color.x, color.y, color.z, color.w };

		vertices[0].texcoord = { u0,v0 };
		vertices[1].texcoord = { u1,v0 };
		vertices[2].texcoord = { u0,v1 };
		vertices[3].texcoord = { u1,v1 };
	}
	dc->Unmap(vertexBuffer.Get(), 0);

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
	dc->Draw(4, 0);
}

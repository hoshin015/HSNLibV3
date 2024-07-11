#include <stdlib.h>
#include "Math.h"

#include "../Framework.h"
#include "../3D/Camera.h"
#include "../Graphics/Graphics.h"

//--------------------------------------------
//	範囲指定ランダム値関数 float型
//--------------------------------------------
// min		: 最小値
// max		: 最大値
float Math::RandomRange(float min, float max)
{
	// 0.0～1.0の間のランダム値
	float value = static_cast<float>(rand()) / RAND_MAX;

	// min～maxまでのランダム値に変換
	return min + (max - min) * value;
}

//--------------------------------------------
//	範囲指定ランダム値関数 int型
//--------------------------------------------
// min		: 最小値
// max		: 最大値
float Math::IRandomRange(int min, int max)
{
	return rand() % ((max + 1) - min) + min;
}

// ========================= 座標変換 =========================

//--------------------------------------------
//	ワールド座標からスクリーン座標への変換
//--------------------------------------------
// worldPosition	: 変換したいワールド座標
DirectX::XMFLOAT2 Math::WorldToScreenPos(DirectX::XMFLOAT3 worldPosition)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

	DirectX::XMVECTOR WorldPosition = DirectX::XMLoadFloat3(&worldPosition);

	// ビューポート
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	// 変換行列
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&Camera::Instance().GetView());
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&Camera::Instance().GetProjection());
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	// ワールド座標からスクリーン座標への変換
	DirectX::XMVECTOR ScreenPosition = DirectX::XMVector3Project(
		WorldPosition,
		viewport.TopLeftX,
		viewport.TopLeftY,
		viewport.Width,
		viewport.Height,
		viewport.MinDepth,
		viewport.MaxDepth,
		Projection,
		View,
		World
	);

	// スクリーン座標
	DirectX::XMFLOAT2 screenPosition;
	DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);

	return screenPosition;
}

DirectX::XMFLOAT2 Math::WorldToScreenPos(DirectX::XMFLOAT3 worldPosition, CameraBase* camera)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

	DirectX::XMVECTOR WorldPosition = DirectX::XMLoadFloat3(&worldPosition);

	// ビューポート
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	// 変換行列
	DirectX::XMFLOAT4X4 V;
	DirectX::XMStoreFloat4x4(&V, camera->GetView());
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&V);
	DirectX::XMFLOAT4X4 P;
	DirectX::XMStoreFloat4x4(&P, camera->GetProjection());
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&P);
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	// ワールド座標からスクリーン座標への変換
	DirectX::XMVECTOR ScreenPosition = DirectX::XMVector3Project(
		WorldPosition,
		viewport.TopLeftX,
		viewport.TopLeftY,
		viewport.Width,
		viewport.Height,
		viewport.MinDepth,
		viewport.MaxDepth,
		Projection,
		View,
		World
	);

	// スクリーン座標
	DirectX::XMFLOAT2 screenPosition;
	DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);

	return screenPosition;
}

//--------------------------------------------
//	スクリーン座標からndc座標への変換
//--------------------------------------------
// screenPosition	: 変換したいワールド座標
DirectX::XMFLOAT2 Math::ScreenToNdcPos(DirectX::XMFLOAT2 screenPosition)
{
	// 0 ~ 1 に変換
	DirectX::XMFLOAT2 ndc =
	{
		screenPosition.x / Framework::Instance().GetScreenWidthF(),
		screenPosition.y / Framework::Instance().GetScreenHeightF()
	};

	// -1 ~ 1 に変換
	ndc.x = ndc.x * 2.0f - 1.0f;
	ndc.y = ndc.y * 2.0f - 1.0f;

	return ndc;
}


// ========================= XMFloat3 =========================

//--------------------------------------------
//	正規化する(長さを１にする)
//--------------------------------------------
// f1		: 1点目の座標
// f2		: 2点目の座標
DirectX::XMFLOAT3 Math::XMFloat3Normalize(const DirectX::XMFLOAT3& f)
{
	DirectX::XMVECTOR F = DirectX::XMLoadFloat3(&f);
	F = DirectX::XMVector3Normalize(F);
	DirectX::XMFLOAT3 n;
	DirectX::XMStoreFloat3(&n, F);
	return n;
}

//--------------------------------------------
//	2点間の長さ返す
//--------------------------------------------
// f1		: 1点目の座標
// f2		: 2点目の座標
float Math::XMFloat3Length(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
{
	DirectX::XMVECTOR F1 = DirectX::XMLoadFloat3(&f1);
	DirectX::XMVECTOR F2 = DirectX::XMLoadFloat3(&f2);
	DirectX::XMVECTOR F3 = DirectX::XMVectorSubtract(F1, F2);
	DirectX::XMVECTOR LENGTH = DirectX::XMVector3Length(F3);
	float length;
	DirectX::XMStoreFloat(&length, LENGTH);
	return length;
}

//--------------------------------------------
//	2点間の長さの２乗返す
//--------------------------------------------
// f1		: 1点目の座標
// f2		: 2点目の座標
float Math::XMFloat3LengthSq(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
{
	DirectX::XMVECTOR F1 = DirectX::XMLoadFloat3(&f1);
	DirectX::XMVECTOR F2 = DirectX::XMLoadFloat3(&f2);
	DirectX::XMVECTOR F3 = DirectX::XMVectorSubtract(F1, F2);
	DirectX::XMVECTOR LENGTH = DirectX::XMVector3LengthSq(F3);
	float length;
	DirectX::XMStoreFloat(&length, LENGTH);
	return length;
}

//--------------------------------------------
//	内積を求める
//--------------------------------------------
// f1		: 1点目のベクトル(XMFLOAT3だが(0,0,0)を基準としたベクトルのようにあつかう)
// f2		: 2点目のベクトル(XMFLOAT3だが(0,0,0)を基準としたベクトルのようにあつかう)
float Math::XMFloat3Dot(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
{
	DirectX::XMVECTOR F1 = DirectX::XMLoadFloat3(&f1);
	DirectX::XMVECTOR F2 = DirectX::XMLoadFloat3(&f2);
	DirectX::XMVECTOR DOT = DirectX::XMVector3Dot(F1, F2);
	float dot;
	DirectX::XMStoreFloat(&dot, DOT);
	return dot;
}

//--------------------------------------------
//	外積を求める
//--------------------------------------------
// f1		: 1点目のベクトル(XMFLOAT3だが(0,0,0)を基準としたベクトルのようにあつかう)
// f2		: 2点目のベクトル(XMFLOAT3だが(0,0,0)を基準としたベクトルのようにあつかう)
DirectX::XMFLOAT3 Math::XMFloat3Cross(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
{
	DirectX::XMVECTOR F1 = DirectX::XMLoadFloat3(&f1);
	DirectX::XMVECTOR F2 = DirectX::XMLoadFloat3(&f2);
	DirectX::XMVECTOR CROSS = DirectX::XMVector3Cross(F1, F2);
	DirectX::XMFLOAT3 cross;
	DirectX::XMStoreFloat3(&cross, CROSS);
	return cross;
}

/// 線形補間
/// @param f1 補間元の最初の値
/// @param f2 補間元の2つ目の値
/// @param t 補間制御係数 0であればf1を,1であればf2になる
/// @return 補間した値を返す
float Math::Lerp(const float& f1, const float& f2, const float& t) {
	return f1 + t * (f2 - f1);
}

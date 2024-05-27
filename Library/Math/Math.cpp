#include <stdlib.h>
#include "Math.h"

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

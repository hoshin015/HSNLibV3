#pragma once
#include "OperatorXMFloat3.h"

class Math
{
public:
	// 範囲指定ランダム値 float 型
	static float RandomRange(float min, float max);
	// 範囲指定ランダム値 int 型
	static float IRandomRange(int min, int max);

	// ========================= 座標変換 =========================

	// ワールド座標からスクリーン座標への変換
	static DirectX::XMFLOAT2 WorldToScreenPos(DirectX::XMFLOAT3 worldPosition);



	// ========================= XMFloat3 =========================
	
	// 正規化する
	static DirectX::XMFLOAT3 XMFloat3Normalize(const DirectX::XMFLOAT3& f);
	// 2点間の長さを求める
	static float XMFloat3Length(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2);
	// 2点間の長さの２乗を求める
	static float XMFloat3LengthSq(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2);
	// 内積を求める
	static float XMFloat3Dot(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2);
	// 外積を求める
	static DirectX::XMFLOAT3 XMFloat3Cross(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2);

	// ========================= float =============================

	//線形補間
	static float Lerp(const float& f1, const float& f2, const float& t);
};
#include <stdlib.h>
#include "Math.h"

//--------------------------------------------
//	�͈͎w�胉���_���l�֐� float�^
//--------------------------------------------
// min		: �ŏ��l
// max		: �ő�l
float Math::RandomRange(float min, float max)
{
	// 0.0�`1.0�̊Ԃ̃����_���l
	float value = static_cast<float>(rand()) / RAND_MAX;

	// min�`max�܂ł̃����_���l�ɕϊ�
	return min + (max - min) * value;
}

//--------------------------------------------
//	�͈͎w�胉���_���l�֐� int�^
//--------------------------------------------
// min		: �ŏ��l
// max		: �ő�l
float Math::IRandomRange(int min, int max)
{
	return rand() % ((max + 1) - min) + min;
}


// ========================= XMFloat3 =========================

//--------------------------------------------
//	���K������(�������P�ɂ���)
//--------------------------------------------
// f1		: 1�_�ڂ̍��W
// f2		: 2�_�ڂ̍��W
DirectX::XMFLOAT3 Math::XMFloat3Normalize(const DirectX::XMFLOAT3& f)
{
	DirectX::XMVECTOR F = DirectX::XMLoadFloat3(&f);
	F = DirectX::XMVector3Normalize(F);
	DirectX::XMFLOAT3 n;
	DirectX::XMStoreFloat3(&n, F);
	return n;
}

//--------------------------------------------
//	2�_�Ԃ̒����Ԃ�
//--------------------------------------------
// f1		: 1�_�ڂ̍��W
// f2		: 2�_�ڂ̍��W
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
//	2�_�Ԃ̒����̂Q��Ԃ�
//--------------------------------------------
// f1		: 1�_�ڂ̍��W
// f2		: 2�_�ڂ̍��W
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
//	���ς����߂�
//--------------------------------------------
// f1		: 1�_�ڂ̃x�N�g��(XMFLOAT3����(0,0,0)����Ƃ����x�N�g���̂悤�ɂ�����)
// f2		: 2�_�ڂ̃x�N�g��(XMFLOAT3����(0,0,0)����Ƃ����x�N�g���̂悤�ɂ�����)
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
//	�O�ς����߂�
//--------------------------------------------
// f1		: 1�_�ڂ̃x�N�g��(XMFLOAT3����(0,0,0)����Ƃ����x�N�g���̂悤�ɂ�����)
// f2		: 2�_�ڂ̃x�N�g��(XMFLOAT3����(0,0,0)����Ƃ����x�N�g���̂悤�ɂ�����)
DirectX::XMFLOAT3 Math::XMFloat3Cross(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
{
	DirectX::XMVECTOR F1 = DirectX::XMLoadFloat3(&f1);
	DirectX::XMVECTOR F2 = DirectX::XMLoadFloat3(&f2);
	DirectX::XMVECTOR CROSS = DirectX::XMVector3Cross(F1, F2);
	DirectX::XMFLOAT3 cross;
	DirectX::XMStoreFloat3(&cross, CROSS);
	return cross;
}

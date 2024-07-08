#pragma once
#include "OperatorXMFloat3.h"

class Math
{
public:
	// �͈͎w�胉���_���l float �^
	static float RandomRange(float min, float max);
	// �͈͎w�胉���_���l int �^
	static float IRandomRange(int min, int max);

	// ========================= ���W�ϊ� =========================

	// ���[���h���W����X�N���[�����W�ւ̕ϊ�
	static DirectX::XMFLOAT2 WorldToScreenPos(DirectX::XMFLOAT3 worldPosition);



	// ========================= XMFloat3 =========================
	
	// ���K������
	static DirectX::XMFLOAT3 XMFloat3Normalize(const DirectX::XMFLOAT3& f);
	// 2�_�Ԃ̒��������߂�
	static float XMFloat3Length(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2);
	// 2�_�Ԃ̒����̂Q������߂�
	static float XMFloat3LengthSq(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2);
	// ���ς����߂�
	static float XMFloat3Dot(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2);
	// �O�ς����߂�
	static DirectX::XMFLOAT3 XMFloat3Cross(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2);

	// ========================= float =============================

	//���`���
	static float Lerp(const float& f1, const float& f2, const float& t);
};
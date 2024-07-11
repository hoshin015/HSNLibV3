#pragma once
#include "OperatorXMFloat3.h"
#include "../3D/CameraBase.h"

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
	// ���[���h���W����X�N���[�����W�ւ̕ϊ�
	static DirectX::XMFLOAT2 WorldToScreenPos(DirectX::XMFLOAT3 worldPosition, CameraBase* camera);
	// �X�N���[�����W����NDC���W�ւ̕ϊ�
	static DirectX::XMFLOAT2 ScreenToNdcPos(DirectX::XMFLOAT2 screenPosition);


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
};
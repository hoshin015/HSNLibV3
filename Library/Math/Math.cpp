#include <stdlib.h>
#include "Math.h"

#include "../Framework.h"
#include "../3D/Camera.h"
#include "../Graphics/Graphics.h"

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

// ========================= ���W�ϊ� =========================

//--------------------------------------------
//	���[���h���W����X�N���[�����W�ւ̕ϊ�
//--------------------------------------------
// worldPosition	: �ϊ����������[���h���W
DirectX::XMFLOAT2 Math::WorldToScreenPos(DirectX::XMFLOAT3 worldPosition)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

	DirectX::XMVECTOR WorldPosition = DirectX::XMLoadFloat3(&worldPosition);

	// �r���[�|�[�g
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	// �ϊ��s��
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&Camera::Instance().GetView());
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&Camera::Instance().GetProjection());
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	// ���[���h���W����X�N���[�����W�ւ̕ϊ�
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

	// �X�N���[�����W
	DirectX::XMFLOAT2 screenPosition;
	DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);

	return screenPosition;
}

DirectX::XMFLOAT2 Math::WorldToScreenPos(DirectX::XMFLOAT3 worldPosition, CameraBase* camera)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

	DirectX::XMVECTOR WorldPosition = DirectX::XMLoadFloat3(&worldPosition);

	// �r���[�|�[�g
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	// �ϊ��s��
	DirectX::XMFLOAT4X4 V;
	DirectX::XMStoreFloat4x4(&V, camera->GetView());
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&V);
	DirectX::XMFLOAT4X4 P;
	DirectX::XMStoreFloat4x4(&P, camera->GetProjection());
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&P);
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	// ���[���h���W����X�N���[�����W�ւ̕ϊ�
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

	// �X�N���[�����W
	DirectX::XMFLOAT2 screenPosition;
	DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);

	return screenPosition;
}

//--------------------------------------------
//	�X�N���[�����W����ndc���W�ւ̕ϊ�
//--------------------------------------------
// screenPosition	: �ϊ����������[���h���W
DirectX::XMFLOAT2 Math::ScreenToNdcPos(DirectX::XMFLOAT2 screenPosition)
{
	// 0 ~ 1 �ɕϊ�
	DirectX::XMFLOAT2 ndc =
	{
		screenPosition.x / Framework::Instance().GetScreenWidthF(),
		screenPosition.y / Framework::Instance().GetScreenHeightF()
	};

	// -1 ~ 1 �ɕϊ�
	ndc.x = ndc.x * 2.0f - 1.0f;
	ndc.y = ndc.y * 2.0f - 1.0f;

	return ndc;
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

/// ���`���
/// @param f1 ��Ԍ��̍ŏ��̒l
/// @param f2 ��Ԍ���2�ڂ̒l
/// @param t ��Ԑ���W�� 0�ł����f1��,1�ł����f2�ɂȂ�
/// @return ��Ԃ����l��Ԃ�
float Math::Lerp(const float& f1, const float& f2, const float& t) {
	return f1 + t * (f2 - f1);
}

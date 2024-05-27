#pragma once

#include <DirectXMath.h>
#include "../Resource/Model/ModelResource.h"

// �q�b�g����
struct HitResult
{
	DirectX::XMFLOAT3	position = { 0,0,0 };	// ���C�ƃ|���S���̌�_
	DirectX::XMFLOAT3	normal = { 0,0,0 };		// �Փ˂����|���S���̖@���x�N�g��
	float				distance = 0.0f;		// ���C�̎n�_�����_�܂ł̋���
	std::string			materialName = "";		// �Փ˂����|���S���̃}�e���A���̖��O
};


// �R���W����
class Collision
{
public:
	// ���Ƌ��̌�������
	static bool IntersectSphereVsSphere(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		DirectX::XMFLOAT3& outPositionB
	);

	// ���Ƌ��̔�������
	static bool RepulsionSphereVsSphere(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		float weightA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		float weightB,
		DirectX::XMFLOAT3& outPositionA,
		DirectX::XMFLOAT3& outPositionB
	);

	// ���ƌŒ苅�̔�������
	static bool StaticRepulsionSphereVsSphere(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		DirectX::XMFLOAT3& outPositionA,
		int power
	);

	// �~���Ɖ~���̌�������
	static bool IntersectCylinderVsCylinder(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		float heightA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		float heightB,
		DirectX::XMFLOAT3& outPositionB
	);

	// ���Ɖ~���̌�������
	static bool IntersectSphereVsCylinder(
		const DirectX::XMFLOAT3& spherePosition,
		float sphereRadius,
		const DirectX::XMFLOAT3& cylinderPosition,
		float cylinderRadius,
		float cylinderHeight,
		DirectX::XMFLOAT3& outCylinderPosition
	);

	// ���C�ƃ��f���̌�������
	static bool IntersectRayVsModel(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		const ModelResource* model,
		HitResult& result
	);

	// ���C�ƃ��f���̌�������
	static bool IntersectRayVsModel(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		const ModelResource* model,
		const DirectX::XMFLOAT4X4& transform,
		HitResult& result
	);
};
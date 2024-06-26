#pragma once

#include "../../Library/3D/CameraBase.h"

// --- �v���C���[�̃J���� ---
class PlayerCamera : public CameraBase
{
public:
	void Initialize() override;

	void Update() override;
	void UpdateConstants() override;

	void DrawDebugGui() override;


private:
	float horizontalAngle = 0.0f;
	float verticalAngle = 0.0f;
	float sensitivity = 1.0f;	// ���x
	float range = 10.0f;
	bool  fixedCursor = true;
	float t;
	float positionComplementAmount;	// �ʒu�̕⊮��

	Vector3 currentPosition;
};


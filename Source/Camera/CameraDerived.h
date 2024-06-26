#pragma once

#include "../../Library/3D/CameraBase.h"

// --- プレイヤーのカメラ ---
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
	float sensitivity = 1.0f;	// 感度
	float range = 10.0f;
	bool  fixedCursor = true;
	float t;
	float positionComplementAmount;	// 位置の補完量

	Vector3 currentPosition;
};


#pragma once

#include "../../Library/3D/CameraBase.h"

// --- プレイヤーのカメラ ---
class PlayerCamera : public CameraBase
{
public:
	void Update() override;
};


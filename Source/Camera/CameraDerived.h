#pragma once

#include "../../Library/3D/CameraBase.h"

// --- プレイヤーのカメラ ---
class PlayerCamera : public CameraBase
{
public:
	void SetTimer(const float timer) { this->timer = timer; }
	void SetShakePower(const float power) { shakePower = power; }

	void Initialize() override;
	void Update() override;
	void UpdateConstants() override;
	void DrawDebugGui() override;

	void OnSetCamera();
	void OnFixedCursor(float deltaTime);	// カーソルを固定しているときに呼ばれる
	void CalcPositionFromAngle(const Vector3& position);	// 角度から位置を計算

private:
	float horizontalAngle = 0.0f;	// 水平角度
	float verticalAngle = 0.0f;		// 垂直角度
	float sensitivity = 1.0f;		// 感度
	float range;					// カメラの目標からの距離
	float height;					// プレイヤーの高さ
	bool  fixedCursor = true;		// 中央にカーソルを固定するか
	float timer = 0.0f;

	float shakePower = 100.0f;		// 画面を揺らす力

	// --- Lerp減衰用 ---
	float t = 0.3f;	// 補完量

	int state;	// ステート
};



// --- ロックオンカメラ ---
class LockOnCamera final : public CameraBase
{
public:
	void Initialize() override;
	void Update() override;
	void UpdateConstants() override;
	void DrawDebugGui() override;

private:
	int state;		// ステート

	float height;	// 高さ
	float range;	// 距離

	float t = 0.3f;	// 補完量
	Vector3 prevTarget;	// 前回の目標の位置
};



class EnemyDeadCamera final : public CameraBase
{
public:
	void Initialize() override;
	void Update() override;
	void UpdateConstants() override;
	void DrawDebugGui() override;

private:
	int state;
	float timer;
};
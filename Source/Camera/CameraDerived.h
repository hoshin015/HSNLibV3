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

	void OnSetCamera();
	void OnFixedCursor(float deltaTime);	// カーソルを固定しているときに呼ばれる
	Vector3 OnShake(const Vector3& intensity);
	void CalcPositionFromAngle(const Vector3& position);	// 角度から位置を計算

	Vector3 currentTarget;
private:
	float horizontalAngle = 0.0f;	// 水平角度
	float verticalAngle = 0.0f;		// 垂直角度
	float sensitivity = 1.0f;		// 感度
	float range;					// カメラの目標からの距離
	float height;					// プレイヤーの高さ
	bool  fixedCursor = true;		// 中央にカーソルを固定するか

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

	Vector3 OnShake(const Vector3& intensity);

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
	bool updateTimer = true;

	struct
	{
		float beginAngle;
		float endAngle;
		float range;
		float height;
		float time;
	}state1Data, state2Data, state3Data;
};



class PlayerDeadCamera final : public CameraBase
{
public:
	void Initialize() override;
	void Update() override;
	void UpdateConstants() override;
	void DrawDebugGui() override;

private:
	int state;
	float time = 10.0f;
	float timer;
	float range = 12.0f;
	float height = 5.0f;
};
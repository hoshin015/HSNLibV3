#pragma once

#include "../Math/Matrix.h"


// --- カメラの基底クラス ---
class CameraBase
{
public:
	CameraBase();
	virtual ~CameraBase() {}


	// --- ゲッター ---
	const Matrix&  GetView()	   const { return view; }
	const Matrix&  GetProjection() const { return projection; }
	const Vector3& GetPosition()   const { return position; }
	const Vector3& GetTarget()	   const { return target; }
	const Vector3& GetUp()		   const { return up; }
	const float    GetFov()		   const { return fov; }
	const float    GetNearZ()	   const { return nearZ; }
	const float    GetFarZ()	   const { return farZ; }
	const float	   GetAspect()	   const { return aspect; }
	const Vector3& GetFrontVec()   const { return frontVec; }
	const Vector3& GetUpVec()	   const { return upVec; }
	const Vector3& GetRightVec()   const { return rightVec; }

	// --- セッター ---
	void SetView(const Matrix& view) { this->view = view; }
	void SetProjection(const Matrix& projection) { this->projection = projection; }
	void SetPosition(const Vector3& position) { this->position = position; }
	void SetTarget(const Vector3& target) { this->target = target; }
	void SetUp(const Vector3& up) { this->up = up; }
	void SetFov(float fov) { this->fov = fov; }
	void SetNearZ(float nearZ) { this->nearZ = nearZ; }
	void SetFarZ(float farZ) { this->farZ = farZ; }
	void SetAspect(float aspect) { this->aspect = aspect; }

	virtual void Initialize() {};

	// --- カメラ行列の更新 ---
	virtual void Update();
	virtual void Update(
		const Vector3& position, const Vector3& target, const Vector3& up,
		const float fov, const float nearZ, const float farZ, const float aspect
	);

	// --- カメラ定数の更新 ---
	virtual void UpdateConstants();
	virtual void UpdateConstants(const Vector3& position);

	// --- デバッグ描画 ---
	virtual void DrawDebugGui() {};

protected:
	Matrix view;		// ビュー行列
	Matrix projection;	// プロジェクション行列

	// --- ビュー行列に必要な情報 ---
	Vector3 position;					  // カメラの位置
	Vector3 target = {0.0f, 0.0f, 10.0f}; // 目標
	Vector3 up = Vector3::Up_;			  // 上方向

	// --- プロジェクション行列に必要な情報 ---
	float fov = 60.0f;		// 視野角 ( 度数法 )
	float nearZ = 0.01f;	// 最近距離
	float farZ = 500.0f;	// 最遠距離
	float aspect;			// 画面比率

	// --- カメラの方向ベクトル ---
	Vector3 frontVec;	// 正面
	Vector3 upVec;		// 上
	Vector3 rightVec;	// 右
};


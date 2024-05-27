#pragma once
#include <DirectXMath.h>

// 全ての3Dオブジェクトの基底クラス
class BaseObject
{
public:
	BaseObject() = default;
	virtual ~BaseObject() = default;

	//virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Render(bool isShadow) = 0;
	//virtual void DrawDebug() = 0;

protected:
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};

public:
#pragma region ゲッター/セッター
	DirectX::XMFLOAT3	GetPos() { return position; }
	float				GetPosX() { return position.x; }
	float				GetPosY() { return position.y; }
	float				GetPosZ() { return position.z; }
	DirectX::XMFLOAT3	GetAngle() { return angle; }
	float				GetAngleX() { return angle.x; }
	float				GetAngleY() { return angle.y; }
	float				GetAngleZ() { return angle.z; }
	DirectX::XMFLOAT3	GetScale() { return scale; }
	float				GetScaleX() { return scale.x; }
	float				GetScaleY() { return scale.y; }
	float				GetScaleZ() { return scale.z; }
	DirectX::XMFLOAT4X4	GetTransform() { return transform; }

	void SetPos(DirectX::XMFLOAT3 p) { position = p; }
	void SetPosX(float x) { position.x = x; }
	void SetPosY(float y) { position.y = y; }
	void SetPosZ(float z) { position.z = z; }
	void SetAngle(DirectX::XMFLOAT3 a) { angle = a; }
	void SetAngleX(float x) { angle.x = x; }
	void SetAngleY(float y) { angle.y = y; }
	void SetAngleZ(float z) { angle.z = z; }
	void SetScale(DirectX::XMFLOAT3 s) { scale = s; }
	void SetScaleX(float x) { scale.x = x; }
	void SetScaleY(float y) { scale.y = y; }
	void SetScaleZ(float z) { scale.z = z; }
	void SetTransform(DirectX::XMFLOAT4X4 t) { transform = t; }
#pragma endregion
	

protected:
	// 姿勢行列更新
	virtual void UpdateTransform() = 0;
};
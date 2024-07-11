#pragma once

#include "../../Library/3D/CameraBase.h"

// --- �v���C���[�̃J���� ---
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
	void OnFixedCursor(float deltaTime);	// �J�[�\�����Œ肵�Ă���Ƃ��ɌĂ΂��
	void CalcPositionFromAngle(const Vector3& position);	// �p�x����ʒu���v�Z

private:
	float horizontalAngle = 0.0f;	// �����p�x
	float verticalAngle = 0.0f;		// �����p�x
	float sensitivity = 1.0f;		// ���x
	float range;					// �J�����̖ڕW����̋���
	float height;					// �v���C���[�̍���
	bool  fixedCursor = true;		// �����ɃJ�[�\�����Œ肷�邩
	float timer = 0.0f;

	float shakePower = 100.0f;		// ��ʂ�h�炷��

	// --- Lerp�����p ---
	float t = 0.3f;	// �⊮��

	int state;	// �X�e�[�g
};



// --- ���b�N�I���J���� ---
class LockOnCamera final : public CameraBase
{
public:
	void Initialize() override;
	void Update() override;
	void UpdateConstants() override;
	void DrawDebugGui() override;

private:
	int state;		// �X�e�[�g

	float height;	// ����
	float range;	// ����

	float t = 0.3f;	// �⊮��
	Vector3 prevTarget;	// �O��̖ڕW�̈ʒu
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
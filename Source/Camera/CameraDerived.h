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

	void OnSetCamera();
	void OnFixedCursor(float deltaTime);	// �J�[�\�����Œ肵�Ă���Ƃ��ɌĂ΂��
	Vector3 OnShake(const Vector3& intensity);
	void CalcPositionFromAngle(const Vector3& position);	// �p�x����ʒu���v�Z

	Vector3 currentTarget;
private:
	float horizontalAngle = 0.0f;	// �����p�x
	float verticalAngle = 0.0f;		// �����p�x
	float sensitivity = 1.0f;		// ���x
	float range;					// �J�����̖ڕW����̋���
	float height;					// �v���C���[�̍���
	bool  fixedCursor = true;		// �����ɃJ�[�\�����Œ肷�邩

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

	Vector3 OnShake(const Vector3& intensity);

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
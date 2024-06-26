#pragma once

#include "../Math/Matrix.h"


// --- �J�����̊��N���X ---
class CameraBase
{
public:
	CameraBase();
	virtual ~CameraBase() {}


	// --- �Q�b�^�[ ---
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

	// --- �Z�b�^�[ ---
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

	// --- �J�����s��̍X�V ---
	virtual void Update();
	virtual void Update(
		const Vector3& position, const Vector3& target, const Vector3& up,
		const float fov, const float nearZ, const float farZ, const float aspect
	);

	// --- �J�����萔�̍X�V ---
	virtual void UpdateConstants();
	virtual void UpdateConstants(const Vector3& position);

	// --- �f�o�b�O�`�� ---
	virtual void DrawDebugGui() {};

protected:
	Matrix view;		// �r���[�s��
	Matrix projection;	// �v���W�F�N�V�����s��

	// --- �r���[�s��ɕK�v�ȏ�� ---
	Vector3 position;					  // �J�����̈ʒu
	Vector3 target = {0.0f, 0.0f, 10.0f}; // �ڕW
	Vector3 up = Vector3::Up_;			  // �����

	// --- �v���W�F�N�V�����s��ɕK�v�ȏ�� ---
	float fov = 60.0f;		// ����p ( �x���@ )
	float nearZ = 0.01f;	// �ŋߋ���
	float farZ = 500.0f;	// �ŉ�����
	float aspect;			// ��ʔ䗦

	// --- �J�����̕����x�N�g�� ---
	Vector3 frontVec;	// ����
	Vector3 upVec;		// ��
	Vector3 rightVec;	// �E
};


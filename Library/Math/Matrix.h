#pragma once

#include "Vector.h"

// ===== �s��N���X ========================================================================================================================
class Matrix
{
public:
	// --- �R���X�g���N�^ ---
	Matrix(
		float m11 = 1.0f, float m12 = 0.0f, float m13 = 0.0f, float m14 = 0.0f,
		float m21 = 0.0f, float m22 = 1.0f, float m23 = 0.0f, float m24 = 0.0f,
		float m31 = 0.0f, float m32 = 0.0f, float m33 = 1.0f, float m34 = 0.0f,
		float m41 = 0.0f, float m42 = 0.0f, float m43 = 0.0f, float m44 = 1.0f)
	{
		_11 = m11; _12 = m12; _13 = m13; _14 = m14;
		_21 = m21; _22 = m22; _23 = m23; _24 = m24;
		_31 = m31; _32 = m32; _33 = m33; _34 = m34;
		_41 = m41; _42 = m42; _43 = m43; _44 = m44;
	}

	Matrix(const Matrix& m)
	{
		*this = m;
	}

	Matrix(const DirectX::XMFLOAT4X4& m)
	{
		mat_ = m;
	}


	operator DirectX::XMMATRIX() const
	{
		return DirectX::XMLoadFloat4x4(&mat_);
	}


	// --- ������Z�q ---
	Matrix& operator=(const Matrix& m)
	{
		mat_ = m.mat_;
		return *this;
	}

	// --- ��Z��� ---
	const Matrix& operator*=(const Matrix& m)
	{
		Multiply(*this, m);
		return *this;
	}

	// --- ��Z ---
	void Multiply(const Matrix& m0, const Matrix& m1)
	{
		DirectX::XMStoreFloat4x4(
			&mat_,
			DirectX::XMMatrixMultiply(m0, m1)
		);
	}


	// --- ���s�ړ��s��̍쐬 ---
	void MakeTranslation(const Vector3& position)
	{
		DirectX::XMStoreFloat4x4(
			&mat_,
			DirectX::XMMatrixTranslationFromVector(position)
		);
	}

	void MakeTranslation(float x, float y, float z)
	{
		MakeTranslation(Vector3(x, y, z));
	}


	// --- ��]�s��̍쐬 ( Radian ) ---
	void MakeRotation(const Vector3& rotation)
	{
		DirectX::XMStoreFloat4x4(
			&mat_,
			DirectX::XMMatrixRotationRollPitchYawFromVector(rotation)
		);
	}

	void MakeRotation(float x, float y, float z)
	{
		MakeRotation(Vector3(x, y, z));
	}

	
	// --- �N�H�[�^�j�I�������]�s����쐬 ---
	void MakeRotationFromQuaternion(const Quaternion& q)
	{
		DirectX::XMStoreFloat4x4(
			&mat_,
			DirectX::XMMatrixRotationQuaternion(q)
		);
	}


	// --- �g��s��̍쐬 ---
	void MakeScaling(const Vector3& scaling)
	{
		DirectX::XMStoreFloat4x4(
			&mat_,
			DirectX::XMMatrixScalingFromVector(scaling)
		);
	}

	void MakeScaling(float x, float y, float z)
	{
		MakeScaling(Vector3(x, y, z));
	}


	// --- �J�����s��̍쐬 ---
	void MakeLookAt(const Vector3& position, const Vector3& target, const Vector3& up = Vector3::Up_)
	{
		DirectX::XMStoreFloat4x4(
			&mat_,
			DirectX::XMMatrixLookAtLH(position, target, up)
		);
	}


	/// <summary>
	/// --- ���s���e�s��̍쐬 ---
	/// </summary>
	/// <param name="width">: ��ʂ̕�</param>
	/// <param name="height">: ��ʂ̍���</param>
	/// <param name="nearZ">: �ŋߋ���</param>
	/// <param name="farZ">: �ŉ�����</param>
	void MakeOrthographic(float width, float height, float nearZ, float farZ)
	{
		DirectX::XMStoreFloat4x4(
			&mat_,
			DirectX::XMMatrixOrthographicLH(
				width,
				height,
				nearZ,
				farZ
			)
		);
	}

	/// <summary>
	/// --- �����ϊ��s��̍쐬 ---
	/// </summary>
	/// <param name="fov">: ����p ( �x���@ )</param>
	/// <param name="aspect">: ��ʂ̔䗦</param>
	/// <param name="nearZ">: �ŋߋ���</param>
	/// <param name="farZ">: �ŉ�����</param>
	void MakePerspective(float fov, float aspect, float nearZ, float farZ)
	{
		DirectX::XMStoreFloat4x4(
			&mat_,
			DirectX::XMMatrixPerspectiveFovLH(
				DirectX::XMConvertToRadians(fov),
				aspect,
				nearZ,
				farZ
			)
		);
	}


	// --- �t�s�� ---
	void Inverse()
	{
		DirectX::XMStoreFloat4x4(&mat_, DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&mat_)));
	}

	static Matrix Inverse(const Matrix& m)
	{
		Matrix result;
		result = m;
		result.Inverse();
		return result;
	}



	static const Matrix Identify_;

	union
	{
		DirectX::XMFLOAT4X4 mat_;
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
		Vector4 v_[4];
		float m_[4][4];
	};
};


static inline Matrix operator*(const Matrix& m0, const Matrix& m1)
{
	Matrix result;
	result.Multiply(m0, m1);
	return result;
}


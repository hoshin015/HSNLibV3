#pragma once

#include <DirectXMath.h>

class Matrix;

// ===== 2�����x�N�g�� ==================================================================================================================================
class Vector2
{
public:
	// --- �R���X�g���N�^ ---
	explicit Vector2()
	{
		x = y = 0.0f;
	}

	Vector2(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	Vector2(const DirectX::XMFLOAT2& v)
	{
		this->vec_ = v;
	}


	// --- ��� ---
	Vector2& operator=(const Vector2& vec)
	{
		this->vec_ = vec.vec_;
		return *this;
	}

	// --- ���� + ---
	Vector2 operator+() const
	{
		return Vector2(x, y);
	}

	// --- ���� - ---
	Vector2 operator-() const
	{
		return Vector2(-x, -y);
	}
	
	// --- ���Z ---
	Vector2 operator+(const Vector2& vec) const
	{
		Vector2 result;
		DirectX::XMStoreFloat2(&result.vec_, DirectX::XMVectorAdd(DirectX::XMLoadFloat2(&this->vec_), DirectX::XMLoadFloat2(&vec.vec_)));
		return result;
	}

	// --- ���Z��� ---
	Vector2& operator+=(const Vector2& vec)
	{
		DirectX::XMStoreFloat2(&this->vec_, DirectX::XMVectorAdd(DirectX::XMLoadFloat2(&this->vec_), DirectX::XMLoadFloat2(&vec.vec_)));
		return *this;
	}

	// --- ���Z ---
	Vector2 operator-(const Vector2& vec) const
	{
		Vector2 result;
		DirectX::XMStoreFloat2(&result.vec_, DirectX::XMVectorSubtract(DirectX::XMLoadFloat2(&this->vec_), DirectX::XMLoadFloat2(&vec.vec_)));
		return result;
	}

	// --- ���Z��� ---
	Vector2& operator-=(const Vector2& vec)
	{
		DirectX::XMStoreFloat2(&this->vec_, DirectX::XMVectorSubtract(DirectX::XMLoadFloat2(&this->vec_), DirectX::XMLoadFloat2(&vec.vec_)));
		return *this;
	}

	// --- ��Z ---
	Vector2 operator*(float s) const
	{
		Vector2 result;
		DirectX::XMStoreFloat2(&result.vec_, DirectX::XMVectorScale(DirectX::XMLoadFloat2(&this->vec_), s));
		return result;
	}

	// --- ��Z ( ���� ) ---
	float operator*(const Vector2& vec) const
	{
		float result;
		DirectX::XMStoreFloat(&result, DirectX::XMVector2Dot(DirectX::XMLoadFloat2(&this->vec_), DirectX::XMLoadFloat2(&vec.vec_)));
		return result;
	}

	// --- ��Z��� ---
	Vector2& operator*=(float s)
	{
		DirectX::XMStoreFloat2(&this->vec_, DirectX::XMVectorScale(DirectX::XMLoadFloat2(&this->vec_), s));
		return *this;
	}

	// --- ��Z��� ( ���� ) ---
	float operator*=(const Vector2& vec)
	{
		float result;
		DirectX::XMStoreFloat(&result, DirectX::XMVector2Dot(DirectX::XMLoadFloat2(&this->vec_), DirectX::XMLoadFloat2(&vec.vec_)));
		return result;
	}

	// --- ���Z ---
	Vector2 operator/(const Vector2& vec) const
	{
		Vector2 result;
		DirectX::XMStoreFloat2(&result.vec_, DirectX::XMVectorDivide(DirectX::XMLoadFloat2(&this->vec_), DirectX::XMLoadFloat2(&vec.vec_)));
		return result;
	}

	Vector2 operator/(float d) const
	{
		Vector2 result;
		result = *this * (1.0f / d);
		return result;
	}

	// --- ���Z��� ---
	Vector2& operator/=(const Vector2& vec)
	{
		DirectX::XMStoreFloat2(&this->vec_, DirectX::XMVectorDivide(DirectX::XMLoadFloat2(&this->vec_), DirectX::XMLoadFloat2(&vec.vec_)));
		return *this;
	}

	Vector2& operator/=(float d)
	{
		*this = *this * (1.0f / d);
		return *this;
	}


	// --- ���K�� ---
	void Normalize()
	{
		DirectX::XMStoreFloat2(
			&vec_, DirectX::XMVector2Normalize(DirectX::XMLoadFloat2(&vec_))
		);
	}

	// --- �x�N�g���̒������擾 ---
	float Length() const
	{
		float result{};
		DirectX::XMStoreFloat(&result, DirectX::XMVector2Length(DirectX::XMLoadFloat2(&this->vec_)));
		return result;
	}

	// --- �x�N�g���̒����̓����擾 ---
	float LengthSq() const
	{
		float result{};
		DirectX::XMStoreFloat(&result, DirectX::XMVector2LengthSq(DirectX::XMLoadFloat2(&this->vec_)));
		return result;
	}


	static const Vector2 Zero_;	// �[���x�N�g��


	union
	{
		DirectX::XMFLOAT2 vec_;
		struct { float x, y; };
		float v[2];
	};
};


// ===== 3�����x�N�g�� ==================================================================================================================================
class Vector3
{
public:
	// --- �R���X�g���N�^ ---
	explicit Vector3()
	{
		x = y = z = 0.0f;
	}

	Vector3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vector3(const DirectX::XMFLOAT3& v)
	{
		this->vec_ = v;
	}


	operator DirectX::XMVECTOR() const
	{
		return DirectX::XMLoadFloat3(&vec_);
	}


	// --- ��� ---
	Vector3& operator=(const Vector3& vec)
	{
		this->vec_ = vec.vec_;
		return *this;
	}

	// --- ���� + ---
	Vector3 operator+() const
	{
		return Vector3(x, y, z);
	}

	// --- ���� - ---
	Vector3 operator-() const
	{
		return Vector3(-x, -y, -z);
	}

	// --- ���Z ---
	Vector3 operator+(const Vector3& vec) const
	{
		Vector3 result;
		DirectX::XMStoreFloat3(&result.vec_, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&this->vec_), DirectX::XMLoadFloat3(&vec.vec_)));
		return result;
	}

	// --- ���Z��� ---
	Vector3& operator+=(const Vector3& vec)
	{
		DirectX::XMStoreFloat3(&this->vec_, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&this->vec_), DirectX::XMLoadFloat3(&vec.vec_)));
		return *this;
	}

	// --- ���Z ---
	Vector3 operator-(const Vector3& vec) const
	{
		Vector3 result;
		DirectX::XMStoreFloat3(&result.vec_, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&this->vec_), DirectX::XMLoadFloat3(&vec.vec_)));
		return result;
	}

	// --- ���Z��� ---
	Vector3& operator-=(const Vector3& vec)
	{
		DirectX::XMStoreFloat3(&this->vec_, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&this->vec_), DirectX::XMLoadFloat3(&vec.vec_)));
		return *this;
	}

	// --- ��Z ---
	Vector3 operator*(float s) const
	{
		Vector3 result;
		DirectX::XMStoreFloat3(&result.vec_, DirectX::XMVectorScale(DirectX::XMLoadFloat3(&this->vec_), s));
		return result;
	}

	Vector3 operator*(const Vector3& vec) const
	{
		Vector3 result;
		result = { x * vec.x, y * vec_.y, z * vec.z };
		return result;
	}

	// --- ��Z��� ---
	Vector3& operator*=(float s)
	{
		DirectX::XMStoreFloat3(&this->vec_, DirectX::XMVectorScale(DirectX::XMLoadFloat3(&this->vec_), s));
		return *this;
	}

	Vector3& operator*=(const Vector3& vec)
	{
		*this =  *this * vec;
		return *this;
	}

	// --- ���Z ---
	Vector3 operator/(const Vector3& vec) const
	{
		Vector3 result;
		DirectX::XMStoreFloat3(&result.vec_, DirectX::XMVectorDivide(DirectX::XMLoadFloat3(&this->vec_), DirectX::XMLoadFloat3(&vec.vec_)));
		return result;
	}

	Vector3 operator/(float d) const
	{
		Vector3 result;
		result = *this * (1.0f / d);
		return result;
	}

	// --- ���Z��� ---
	Vector3& operator/=(const Vector3& vec)
	{
		DirectX::XMStoreFloat3(&this->vec_, DirectX::XMVectorDivide(DirectX::XMLoadFloat3(&this->vec_), DirectX::XMLoadFloat3(&vec.vec_)));
		return *this;
	}

	Vector3& operator/=(float d)
	{
		*this = *this * (1.0f / d);
		return *this;
	}


	Vector2 xy() const
	{
		return Vector2(x, y);
	}

	Vector2 xz() const
	{
		return Vector2(x, z);
	}

	Vector2 yz() const
	{
		return Vector2(y, z);
	}


	// --- ���K�� ---
	void Normalize()
	{
		DirectX::XMStoreFloat3(&this->vec_, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&this->vec_)));
	}

	static Vector3 Normalize(const Vector3& vec)
	{
		Vector3 result;
		result = vec;
		result.Normalize();
		return result;
	}


	// --- ���� ---
	float Dot(const Vector3& vec) const
	{
		float result{};
		DirectX::XMStoreFloat(&result, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&this->vec_), DirectX::XMLoadFloat3(&vec.vec_)));
		return result;
	}

	// --- �O�� ---
	Vector3 Cross(const Vector3& vec) const
	{
		Vector3 result;
		DirectX::XMStoreFloat3(&result.vec_, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&this->vec_), DirectX::XMLoadFloat3(&vec.vec_)));
		return result;
	}


	// --- �x�N�g���̒������擾 ---
	float Length() const
	{
		float result{};
		DirectX::XMStoreFloat(&result, DirectX::XMVector3Length(DirectX::XMLoadFloat3(&this->vec_)));
		return result;
	}

	// --- �x�N�g���̒����̓����擾 ---
	float LengthSq() const
	{
		float result{};
		DirectX::XMStoreFloat(&result, DirectX::XMVector3LengthSq(DirectX::XMLoadFloat3(&this->vec_)));
		return result;
	}


	// --- �ʓx�@�ւ̕ϊ� ---
	Vector3 ToRadian()
	{
		Vector3 result;
		result.x = DirectX::XMConvertToRadians(x);
		result.y = DirectX::XMConvertToRadians(y);
		result.z = DirectX::XMConvertToRadians(z);
		return result;
	}

	// --- �x���@�ւ̕ϊ� ---
	Vector3 ToDegree()
	{
		Vector3 result;
		result.x = DirectX::XMConvertToDegrees(x);
		result.y = DirectX::XMConvertToDegrees(y);
		result.z = DirectX::XMConvertToDegrees(z);
		return result;
	}


	void TransformCoord(
		const Vector3& vec,
		const Matrix& mat
	);

	void TransformNormal(
		const Vector3& normal,
		const Matrix& mat
	);


	// --- ���`�⊮ ---
	static Vector3 Lerp(const Vector3& src, const Vector3& dst, float t)
	{
		Vector3 result;
		DirectX::XMStoreFloat3(&result.vec_, DirectX::XMVectorLerp(src, dst, t));
		return result;
	}

	static const Vector3 Zero_;		// �[���x�N�g��
	static const Vector3 Unit_;		// �P�ʃx�N�g��
	static const Vector3 Front_;	// ���ʃx�N�g��
	static const Vector3 Back_;		// ���x�N�g��
	static const Vector3 Left_;		// ���x�N�g��
	static const Vector3 Right_;	// �E�x�N�g��
	static const Vector3 Up_;		// ��x�N�g��
	static const Vector3 Down_;		// ���x�N�g��
	static const Vector3 AxisX_;	// X���x�N�g��
	static const Vector3 AxisY_;	// Y���x�N�g��
	static const Vector3 AxisZ_;	// Z���x�N�g��


	union
	{
		DirectX::XMFLOAT3 vec_;
		struct { float x, y, z; };
		float v_[3];
	};
};


// ===== 4�����x�N�g�� ==================================================================================================================================
class Vector4
{
public:
	// --- �R���X�g���N�^ ---
	explicit Vector4()
	{
		x = y = z = w = 0.0f;
	}

	Vector4(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	Vector4(const DirectX::XMFLOAT4& v)
	{
		this->vec_ = v;
	}


	operator DirectX::XMVECTOR() const
	{
		return DirectX::XMLoadFloat4(&vec_);
	}


	// --- ��� ---
	Vector4& operator=(const Vector4& vec)
	{
		this->vec_ = vec.vec_;
		return *this;
	}

	// --- ���� + ---
	Vector4 operator+() const
	{
		return Vector4(x, y, z, w);
	}

	// --- ���� - ---
	Vector4 operator-() const
	{
		return Vector4(-x, -y, -z, -w);
	}

	// --- ���Z ---
	Vector4 operator+(const Vector4& vec) const
	{
		Vector4 result;
		DirectX::XMStoreFloat4(&result.vec_, DirectX::XMVectorAdd(DirectX::XMLoadFloat4(&this->vec_), DirectX::XMLoadFloat4(&vec.vec_)));
		return result;
	}

	// --- ���Z��� ---
	Vector4& operator+=(const Vector4& vec)
	{
		DirectX::XMStoreFloat4(&this->vec_, DirectX::XMVectorAdd(DirectX::XMLoadFloat4(&this->vec_), DirectX::XMLoadFloat4(&vec.vec_)));
		return *this;
	}

	// --- ���Z ---
	Vector4 operator-(const Vector4& vec) const
	{
		Vector4 result;
		DirectX::XMStoreFloat4(&result.vec_, DirectX::XMVectorSubtract(DirectX::XMLoadFloat4(&this->vec_), DirectX::XMLoadFloat4(&vec.vec_)));
		return result;
	}

	// --- ���Z��� ---
	Vector4& operator-=(const Vector4& vec)
	{
		DirectX::XMStoreFloat4(&this->vec_, DirectX::XMVectorSubtract(DirectX::XMLoadFloat4(&this->vec_), DirectX::XMLoadFloat4(&vec.vec_)));
		return *this;
	}

	// --- ��Z ---
	Vector4 operator*(float s) const
	{
		Vector4 result;
		DirectX::XMStoreFloat4(&result.vec_, DirectX::XMVectorScale(DirectX::XMLoadFloat4(&this->vec_), s));
		return result;
	}

	// --- ��Z��� ---
	Vector4& operator*=(float s)
	{
		DirectX::XMStoreFloat4(&this->vec_, DirectX::XMVectorScale(DirectX::XMLoadFloat4(&this->vec_), s));
		return *this;
	}

	// --- ���Z ---
	Vector4 operator/(const Vector4& vec) const
	{
		Vector4 result;
		DirectX::XMStoreFloat4(&result.vec_, DirectX::XMVectorDivide(DirectX::XMLoadFloat4(&this->vec_), DirectX::XMLoadFloat4(&vec.vec_)));
		return result;
	}

	Vector4 operator/(float d) const
	{
		Vector4 result;
		result = *this * (1.0f / d);
		return result;
	}

	// --- ���Z��� ---
	Vector4& operator/=(const Vector4& vec)
	{
		DirectX::XMStoreFloat4(&this->vec_, DirectX::XMVectorDivide(DirectX::XMLoadFloat4(&this->vec_), DirectX::XMLoadFloat4(&vec.vec_)));
		return *this;
	}

	Vector4& operator/=(float d)
	{
		*this = *this * (1.0f / d);
		return *this;
	}


	Vector3 rgb() const
	{
		return Vector3(r, g, b);
	}

	Vector3 xyz() const
	{
		return Vector3(x, y, z);
	}


	// --- ���K�� ---
	void Normalize()
	{
		DirectX::XMStoreFloat4(&this->vec_, DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&this->vec_)));
	}
	

	// --- ���� ---
	float Dot(const Vector4& vec) const
	{
		float result{};
		DirectX::XMStoreFloat(&result, DirectX::XMVector4Dot(DirectX::XMLoadFloat4(&this->vec_), DirectX::XMLoadFloat4(&vec.vec_)));
		return result;
	}


	// --- �x�N�g���̒������擾 ---
	float Length() const
	{
		float result{};
		DirectX::XMStoreFloat(&result, DirectX::XMVector4Length(DirectX::XMLoadFloat4(&this->vec_)));
		return result;
	}

	// --- �x�N�g���̒����̓����擾 ---
	float LengthSq() const
	{
		float result{};
		DirectX::XMStoreFloat(&result, DirectX::XMVector4LengthSq(DirectX::XMLoadFloat4(&this->vec_)));
		return result;
	}


	static const Vector4 White_;
	static const Vector4 Black_;
	static const Vector4 Gray_;


	union
	{
		DirectX::XMFLOAT4 vec_;
		struct { float x, y, z, w; };
		struct { float r, g, b, a; };
		float v_[4];
	};
};


// ===== �N�H�[�^�j�I�� ==================================================================================================================================
class Quaternion final : public Vector4
{
public:
	// --- �R���X�g���N�^ ---
	Quaternion()
	{
		x = y = z = 0.0f;
		w = 1.0f;
	}

	Quaternion(float x, float y, float z, float w) :
		Vector4(x, y, z, w)
	{}

	Quaternion(const Vector3& axis, float angle)
	{
		SetRotation(axis, angle);
	}


	// --- X������̉�]�N�H�[�^�j�I���̍쐬 ( Radian ) ---
	void SetRotationX(float angle)
	{
		SetRotation(Vector3::AxisX_, angle);
	}

	// --- X������̉�]�N�H�[�^�j�I���̍쐬 ( Degree ) ---
	void SetRotationDegX(float angle)
	{
		SetRotationDeg(Vector3::AxisX_, angle);
	}


	// --- Y������̉�]�N�H�[�^�j�I���̍쐬 ( Radian ) ---
	void SetRotationY(float angle)
	{
		SetRotation(Vector3::AxisY_, angle);
	}

	// --- Y������̉�]�N�H�[�^�j�I���̍쐬 ( Degree ) ---
	void SetRotationDegY(float angle)
	{
		SetRotationDeg(Vector3::AxisY_, angle);
	}


	// --- Z������̉�]�N�H�[�^�j�I���̍쐬 ( Radian ) ---
	void SetRotationZ(float angle)
	{
		SetRotation(Vector3::AxisZ_, angle);
	}

	// --- Z������̉�]�N�H�[�^�j�I���̍쐬 ( Degree ) ---
	void SetRotationDegZ(float angle)
	{
		SetRotationDeg(Vector3::AxisZ_, angle);
	}


	// --- XYZ������̉�]�N�H�[�^�j�I���̍쐬 ( Radian ) ---
	void SetRotationFromVector(const Vector3& rotation)
	{
		Quaternion rot;
		rot.SetRotationX(rotation.x);
		*this *= rot;
		rot.SetRotationY(rotation.y);
		*this *= rot;
		rot.SetRotationZ(rotation.z);
		*this *= rot;
	}

	// --- XYZ������̉�]�N�H�[�^�j�I���̍쐬 ( Radian ) ---
	void SetRotationDegFromVector(Vector3& rotation)
	{
		SetRotationFromVector(rotation.ToRadian());
	}


	// --- �C�ӎ�����̉�]�N�H�[�^�j�I���̍쐬 ( Radian ) ---
	void SetRotation(const Vector3& axis, float angle)
	{
		float sin{};
		float halfAngle = angle * 0.5f;
		sin = sinf(halfAngle);
		w = cosf(halfAngle);
		x = axis.x * sin;
		y = axis.y * sin;
		z = axis.z * sin;
	}

	// --- �C�ӎ�����̉�]�N�H�[�^�j�I���̍쐬 ( Degree ) ---
	void SetRotationDeg(const Vector3& axis, float angle)
	{
		SetRotation(axis, DirectX::XMConvertToRadians(angle));
	}

	// --- �s�񂩂�N�H�[�^�j�I�����쐬 ---
	void SetRotation(const Matrix& m);


	// --- �N�H�[�^�j�I�����m�̏�Z ---
	void Multiply(const Quaternion& rot)
	{
		float pw, px, py, pz;
		float qw, qx, qy, qz;

		qw = w; qx = x; qy = y; qz = z;
		pw = rot.w; px = rot.x; py = rot.y; pz = rot.z;

		w = pw * qw - px * qx - py * qy - pz * qz;
		x = pw * px + px * qw + py * qz - pz * qy;
		y = pw * qy - px * qz + py * qw + pz * qx;
		z = pw * qz + px * qy - py * qx + pz * qw;
	}

	void Multiply(const Quaternion& rot0, const Quaternion& rot1)
	{
		float pw, px, py, pz;
		float qw, qx, qy, qz;

		qw = rot0.w; qx = rot0.x; qy = rot0.y; qz = rot0.z;
		pw = rot1.w; px = rot1.x; py = rot1.y; pz = rot1.z;

		w = pw * qw - px * qx - py * qy - pz * qz;
		x = pw * qx + px * qw + py * qz - pz * qy;
		y = pw * qy - px * qz + py * qw + pz * qx;
		z = pw * qz + px * qy - py * qx + pz * qw;
	}


	// --- �����Z���Z�q ---
	const Quaternion& operator*=(const Quaternion& rot)
	{
		Multiply(rot, *this);
		return *this;
	}

	// --- ��Z���Z�q ---
	Quaternion operator*(const Quaternion& rot)
	{
		Quaternion result;
		result.Multiply(rot, *this);
		return result;
	}


	// --- ���`�⊮ ---
	static Quaternion Lerp(const Quaternion& src, const Quaternion& dst, float t)
	{
		Quaternion result;
		DirectX::XMStoreFloat4(&result.vec_, DirectX::XMQuaternionSlerp(src, dst, t));
		return result;
	}

	static const Quaternion Identity_;
};
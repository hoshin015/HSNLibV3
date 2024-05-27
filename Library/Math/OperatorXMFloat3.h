#pragma once
#include <DirectXMath.h>

#define USE_VECTOR 0

#if USE_VECTOR

#pragma region XMFloat3 - XMFloat3 

// --- éZèpââéZéq ---

// â¡éZ
inline DirectX::XMFLOAT3& operator+(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
{
	DirectX::XMVECTOR F1 = DirectX::XMLoadFloat3(&f1);
	DirectX::XMVECTOR F2 = DirectX::XMLoadFloat3(&f2);
	DirectX::XMVECTOR F3 = DirectX::XMVectorAdd(F1, F2);
	DirectX::XMFLOAT3 f3;
	DirectX::XMStoreFloat3(&f3, F3);
	return f3;
}
// å∏éZ
inline DirectX::XMFLOAT3& operator-(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
{
	DirectX::XMVECTOR F1 = DirectX::XMLoadFloat3(&f1);
	DirectX::XMVECTOR F2 = DirectX::XMLoadFloat3(&f2);
	DirectX::XMVECTOR F3 = DirectX::XMVectorSubtract(F1, F2);
	DirectX::XMFLOAT3 f3;
	DirectX::XMStoreFloat3(&f3, F3);
	return f3;
}
// èÊéZ
inline DirectX::XMFLOAT3& operator*(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
{
	DirectX::XMVECTOR F1 = DirectX::XMLoadFloat3(&f1);
	DirectX::XMVECTOR F2 = DirectX::XMLoadFloat3(&f2);
	DirectX::XMVECTOR F3 = DirectX::XMVectorMultiply(F1, F2);
	DirectX::XMFLOAT3 f3;
	DirectX::XMStoreFloat3(&f3, F3);
	return f3;
}
// èôéZ
inline DirectX::XMFLOAT3& operator/(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
{
	DirectX::XMVECTOR F1 = DirectX::XMLoadFloat3(&f1);
	DirectX::XMVECTOR F2 = DirectX::XMLoadFloat3(&f2);
	DirectX::XMVECTOR F3 = DirectX::XMVectorDivide(F1, F2);
	DirectX::XMFLOAT3 f3;
	DirectX::XMStoreFloat3(&f3, F3);
	return f3;
}

// --- ï°çáë„ì¸ââéZéq ---

// â¡éZ
inline DirectX::XMFLOAT3& operator+=(DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
{
	DirectX::XMVECTOR F1 = DirectX::XMLoadFloat3(&f1);
	DirectX::XMVECTOR F2 = DirectX::XMLoadFloat3(&f2);
	F1 = DirectX::XMVectorAdd(F1, F2);
	DirectX::XMStoreFloat3(&f1, F1);
	return f1;
}
// å∏éZ
inline DirectX::XMFLOAT3& operator-=(DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
{
	DirectX::XMVECTOR F1 = DirectX::XMLoadFloat3(&f1);
	DirectX::XMVECTOR F2 = DirectX::XMLoadFloat3(&f2);
	F1 = DirectX::XMVectorSubtract(F1, F2);
	DirectX::XMStoreFloat3(&f1, F1);
	return f1;
}
// èÊéZ
inline DirectX::XMFLOAT3& operator/=(DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
{
	DirectX::XMVECTOR F1 = DirectX::XMLoadFloat3(&f1);
	DirectX::XMVECTOR F2 = DirectX::XMLoadFloat3(&f2);
	F1 = DirectX::XMVectorDivide(F1, F2);
	DirectX::XMStoreFloat3(&f1, F1);
	return f1;
}
// èôéZ
inline DirectX::XMFLOAT3& operator*=(DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
{
	DirectX::XMVECTOR F1 = DirectX::XMLoadFloat3(&f1);
	DirectX::XMVECTOR F2 = DirectX::XMLoadFloat3(&f2);
	F1 = DirectX::XMVectorMultiply(F1, F2);
	DirectX::XMStoreFloat3(&f1, F1);
	return f1;
}

// --- î‰ärââéZéq ---
inline bool operator==(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
{
	bool result = (
		f1.x == f2.x &&
		f1.y == f2.y &&
		f1.z == f2.z
	);

	return result;
}

inline bool operator!=(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
{
	bool result = (
		f1.x != f2.x ||
		f1.y != f2.y ||
		f1.z != f2.z
		);

	return result;
}

#pragma endregion

#pragma region XMFloat3 - float

// --- éZèpââéZéq ---

// â¡éZ
inline DirectX::XMFLOAT3& operator+(const DirectX::XMFLOAT3& f1, const float f2)
{
	DirectX::XMVECTOR F1 = DirectX::XMLoadFloat3(&f1);
	DirectX::XMVECTOR F2 = DirectX::XMVectorReplicate(f2);
	DirectX::XMVECTOR F3 = DirectX::XMVectorAdd(F1, F2);
	DirectX::XMFLOAT3 f3;
	DirectX::XMStoreFloat3(&f3, F3);
	return f3;
}

// å∏éZ
inline DirectX::XMFLOAT3& operator-(const DirectX::XMFLOAT3& f1, const float f2)
{
	DirectX::XMVECTOR F1 = DirectX::XMLoadFloat3(&f1);
	DirectX::XMVECTOR F2 = DirectX::XMVectorReplicate(f2);
	DirectX::XMVECTOR F3 = DirectX::XMVectorSubtract(F1, F2);
	DirectX::XMFLOAT3 f3;
	DirectX::XMStoreFloat3(&f3, F3);
	return f3;
}

// èÊéZ
inline DirectX::XMFLOAT3& operator*(const DirectX::XMFLOAT3& f1, const float f2)
{
	DirectX::XMVECTOR F1 = DirectX::XMLoadFloat3(&f1);
	DirectX::XMVECTOR F2 = DirectX::XMVectorReplicate(f2);
	DirectX::XMVECTOR F3 = DirectX::XMVectorMultiply(F1, F2);
	DirectX::XMFLOAT3 f3;
	DirectX::XMStoreFloat3(&f3, F3);
	return f3;
}

// èôéZ
inline DirectX::XMFLOAT3& operator/(const DirectX::XMFLOAT3& f1, const float f2)
{
	DirectX::XMVECTOR F1 = DirectX::XMLoadFloat3(&f1);
	DirectX::XMVECTOR F2 = DirectX::XMVectorReplicate(f2);
	DirectX::XMVECTOR F3 = DirectX::XMVectorDivide(F1, F2);
	DirectX::XMFLOAT3 f3;
	DirectX::XMStoreFloat3(&f3, F3);
	return f3;
}

// --- ï°çáë„ì¸ââéZéq ---

// â¡éZ
inline DirectX::XMFLOAT3& operator+=(DirectX::XMFLOAT3& f1, const float f2)
{
	DirectX::XMVECTOR F1 = DirectX::XMLoadFloat3(&f1);
	DirectX::XMVECTOR F2 = DirectX::XMVectorReplicate(f2);
	F1 = DirectX::XMVectorAdd(F1, F2);
	DirectX::XMStoreFloat3(&f1, F1);
	return f1;
}
// å∏éZ
inline DirectX::XMFLOAT3& operator-=(DirectX::XMFLOAT3& f1, const float f2)
{
	DirectX::XMVECTOR F1 = DirectX::XMLoadFloat3(&f1);
	DirectX::XMVECTOR F2 = DirectX::XMVectorReplicate(f2);
	F1 = DirectX::XMVectorSubtract(F1, F2);
	DirectX::XMStoreFloat3(&f1, F1);
	return f1;
}
// èÊéZ
inline DirectX::XMFLOAT3& operator/=(DirectX::XMFLOAT3& f1, const float f2)
{
	DirectX::XMVECTOR F1 = DirectX::XMLoadFloat3(&f1);
	DirectX::XMVECTOR F2 = DirectX::XMVectorReplicate(f2);
	F1 = DirectX::XMVectorMultiply(F1, F2);
	DirectX::XMStoreFloat3(&f1, F1);
	return f1;
}
// èôéZ
inline DirectX::XMFLOAT3& operator*=(DirectX::XMFLOAT3& f1, const float f2)
{
	DirectX::XMVECTOR F1 = DirectX::XMLoadFloat3(&f1);
	DirectX::XMVECTOR F2 = DirectX::XMVectorReplicate(f2);
	F1 = DirectX::XMVectorDivide(F1, F2);
	DirectX::XMStoreFloat3(&f1, F1);
	return f1;
}

#pragma endregion


#else 

#pragma region XMFloat3 - XMFloat3 

// --- éZèpââéZéq ---

// â¡éZ
inline DirectX::XMFLOAT3& operator+(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
{
	DirectX::XMFLOAT3 result;
	result.x = f1.x + f2.x;
	result.y = f1.y + f2.y;
	result.z = f1.z + f2.z;

	return result;
}
// å∏éZ
inline DirectX::XMFLOAT3& operator-(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
{
	DirectX::XMFLOAT3 result;
	result.x = f1.x - f2.x;
	result.y = f1.y - f2.y;
	result.z = f1.z - f2.z;

	return result;
}
// èÊéZ
inline DirectX::XMFLOAT3& operator*(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
{
	DirectX::XMFLOAT3 result;
	result.x = f1.x * f2.x;
	result.y = f1.y * f2.y;
	result.z = f1.z * f2.z;

	return result;
}
// èôéZ
inline DirectX::XMFLOAT3& operator/(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
{
	DirectX::XMFLOAT3 result;
	result.x = f1.x / f2.x;
	result.y = f1.y / f2.y;
	result.z = f1.z / f2.z;

	return result;
}

// --- ï°çáë„ì¸ââéZéq ---

// â¡éZ
inline DirectX::XMFLOAT3& operator+=(DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
{
	f1.x = f1.x + f2.x;
	f1.y = f1.y + f2.y;
	f1.z = f1.z + f2.z;
	return f1;
}
// å∏éZ
inline DirectX::XMFLOAT3& operator-=(DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
{
	f1.x = f1.x - f2.x;
	f1.y = f1.y - f2.y;
	f1.z = f1.z - f2.z;
	return f1;
}
// èÊéZ
inline DirectX::XMFLOAT3& operator/=(DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
{
	f1.x = f1.x * f2.x;
	f1.y = f1.y * f2.y;
	f1.z = f1.z * f2.z;
	return f1;
}
// èôéZ
inline DirectX::XMFLOAT3& operator*=(DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
{
	f1.x = f1.x / f2.x;
	f1.y = f1.y / f2.y;
	f1.z = f1.z / f2.z;
	return f1;
}

// --- î‰ärââéZéq ---
inline bool operator==(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
{
	bool result = (
		f1.x == f2.x &&
		f1.y == f2.y &&
		f1.z == f2.z
		);

	return result;
}

inline bool operator!=(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
{
	bool result = (
		f1.x != f2.x ||
		f1.y != f2.y ||
		f1.z != f2.z
		);

	return result;
}

#pragma endregion

#pragma region XMFloat3 - float

// --- éZèpââéZéq ---

// â¡éZ
inline DirectX::XMFLOAT3& operator+(const DirectX::XMFLOAT3& f1, const float f2)
{
	DirectX::XMFLOAT3 result;
	result.x = f1.x + f2;
	result.y = f1.y + f2;
	result.z = f1.z + f2;

	return result;
}

// å∏éZ
inline DirectX::XMFLOAT3& operator-(const DirectX::XMFLOAT3& f1, const float f2)
{
	DirectX::XMFLOAT3 result;
	result.x = f1.x - f2;
	result.y = f1.y - f2;
	result.z = f1.z - f2;

	return result;
}

// èÊéZ
inline DirectX::XMFLOAT3& operator*(const DirectX::XMFLOAT3& f1, const float f2)
{
	DirectX::XMFLOAT3 result;
	result.x = f1.x * f2;
	result.y = f1.y * f2;
	result.z = f1.z * f2;

	return result;
}

// èôéZ
inline DirectX::XMFLOAT3& operator/(const DirectX::XMFLOAT3& f1, const float f2)
{
	DirectX::XMFLOAT3 result;
	result.x = f1.x / f2;
	result.y = f1.y / f2;
	result.z = f1.z / f2;

	return result;
}

// --- ï°çáë„ì¸ââéZéq ---

// â¡éZ
inline DirectX::XMFLOAT3& operator+=(DirectX::XMFLOAT3& f1, const float f2)
{
	f1.x = f1.x + f2;
	f1.y = f1.y + f2;
	f1.z = f1.z + f2;

	return f1;
}
// å∏éZ
inline DirectX::XMFLOAT3& operator-=(DirectX::XMFLOAT3& f1, const float f2)
{
	f1.x = f1.x - f2;
	f1.y = f1.y - f2;
	f1.z = f1.z - f2;

	return f1;
}
// èÊéZ
inline DirectX::XMFLOAT3& operator/=(DirectX::XMFLOAT3& f1, const float f2)
{
	f1.x = f1.x / f2;
	f1.y = f1.y / f2;
	f1.z = f1.z / f2;

	return f1;
}
// èôéZ
inline DirectX::XMFLOAT3& operator*=(DirectX::XMFLOAT3& f1, const float f2)
{
	f1.x = f1.x * f2;
	f1.y = f1.y * f2;
	f1.z = f1.z * f2;

	return f1;
}

#pragma endregion

#endif
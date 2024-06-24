#include "../RegisterNum.h"

struct VS_OUT
{
	uint vertexId : VERTEXID;
};

struct GS_OUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
};

struct Particle
{
	float3 position;
	float3 velocity;
	float3 startVelocity;
	float  friction;
	float  gravity;
	float  angle;
	float  addAngle;
	float2 scale;
	float2 startScale;
	float4 color;
	float  lifeTime;
	float  lifeTimer;
	bool   isActive;
	float  kind;
	float  billboardType;
	float  textureType;
};

// タイマーの情報
cbuffer TimerConstant : register(_timerConstant)
{
	float  deltaTime;
	float  nowTime;
	float2 pad;
};

// カメラの情報
cbuffer CameraConstant : register(_cameraConstant)
{
	row_major float4x4 viewProjection;
	float4             cameraPosition;
}

// エミッター, パーティクル情報
cbuffer EmitterConstant : register(_emitterConstant)
{
	float2 particleSizeMin;
	float2 particleSizeMax;
	float4 particleColorMin;
	float4 particleColorMax;
	float  particleSpeedMin;
	float  particleSpeedMax;
	float  particleLifeTimeMin;
	float  particleLifeTimeMax;
	float  particleKind;
	float  particleFrictionMin;
	float  particleFrictionMax;
	float  particleAngleMin;
	float  particleAngleMax;
	float  particleAddAngle;
	float  particleGravity;
	float  particleBillboardType;
	float  particleTextureType;
	float3 emitterPad1;
	float3 emitterPosition;
	float  emitCount;
};


//--------------------------------------------
//	範囲指定ランダム値関数 float型
//--------------------------------------------
// n		: シード値
float rand(float n)
{
	return frac(sin(n) * 43758.5453123);
}

//--------------------------------------------
//	範囲指定ランダム値関数 float型
//--------------------------------------------
// p		: シード値
float random(float2 p)
{
	const float2 r = float2(23.1406926327792690, 2.6651441426902251);
	return frac(cos(fmod(123456789.0, 1e-7 + 256.0 * dot(p, r))));
}

//--------------------------------------------
//	範囲指定ランダム値関数 float型
//--------------------------------------------
// min		: 最小値
// max		: 最大値
float RandomRange(float min, float max)
{
	float f1 = rand(deltaTime);

	// 0.0～1.0の間のランダム値
	float value = rand(f1);

	// min～maxまでのランダム値に変換
	return min + (max - min) * value;
}

//--------------------------------------------
//	範囲指定ランダム値関数 float型
//--------------------------------------------
// min		: 最小値
// max		: 最大値
// seed		: シード値
float RandomRange(float min, float max, float seed)
{
	// 0.0～1.0の間のランダム値
	float value = rand(seed);

	// min～maxまでのランダム値に変換
	return min + (max - min) * value;
}

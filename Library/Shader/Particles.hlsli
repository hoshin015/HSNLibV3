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

// �^�C�}�[�̏��
cbuffer TimerConstant : register(_timerConstant)
{
	float  deltaTime;
	float  nowTime;
	float2 pad;
};

// �J�����̏��
cbuffer CameraConstant : register(_cameraConstant)
{
	row_major float4x4 viewProjection;
	float4             cameraPosition;
}

// �G�~�b�^�[, �p�[�e�B�N�����
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
//	�͈͎w�胉���_���l�֐� float�^
//--------------------------------------------
// n		: �V�[�h�l
float rand(float n)
{
	return frac(sin(n) * 43758.5453123);
}

//--------------------------------------------
//	�͈͎w�胉���_���l�֐� float�^
//--------------------------------------------
// p		: �V�[�h�l
float random(float2 p)
{
	const float2 r = float2(23.1406926327792690, 2.6651441426902251);
	return frac(cos(fmod(123456789.0, 1e-7 + 256.0 * dot(p, r))));
}

//--------------------------------------------
//	�͈͎w�胉���_���l�֐� float�^
//--------------------------------------------
// min		: �ŏ��l
// max		: �ő�l
float RandomRange(float min, float max)
{
	float f1 = rand(deltaTime);

	// 0.0�`1.0�̊Ԃ̃����_���l
	float value = rand(f1);

	// min�`max�܂ł̃����_���l�ɕϊ�
	return min + (max - min) * value;
}

//--------------------------------------------
//	�͈͎w�胉���_���l�֐� float�^
//--------------------------------------------
// min		: �ŏ��l
// max		: �ő�l
// seed		: �V�[�h�l
float RandomRange(float min, float max, float seed)
{
	// 0.0�`1.0�̊Ԃ̃����_���l
	float value = rand(seed);

	// min�`max�܂ł̃����_���l�ɕϊ�
	return min + (max - min) * value;
}

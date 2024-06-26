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
	float  particleAngleMin;
	float  particleAngleMax;
	float  particleAddAngle;
	float  particleGravity;
	float  particleBillboardType;
	float  particleTextureType;
	float  emitterPad1;
	float3 emitterPosition;
	float  emitCount;
};


//--------------------------------------------
//	�����_���l�֐� float�^
//--------------------------------------------
// n		: �V�[�h�l
float rand(inout float seed)
{
	seed *= deltaTime;
	seed = frac(sin(seed) * 43758.5453123);
	return seed;
}

//--------------------------------------------
//	�X�|�[���֐�
//--------------------------------------------
void spawn(uint id, inout Particle p)
{
	float _id  = float(id);
	float seed = _id;

	// ����
	p.isActive = true;

	// �J���[
	p.color.x = lerp(particleColorMin.x, particleColorMax.x, rand(seed));
	p.color.y = lerp(particleColorMin.y, particleColorMax.y, rand(seed));
	p.color.z = lerp(particleColorMin.z, particleColorMax.z, rand(seed));
	p.color.w = lerp(particleColorMin.w, particleColorMax.w, rand(seed));

	// �X�P�[��
	p.scale.x = lerp(particleSizeMin.x, particleSizeMax.x, rand(seed));
	p.scale.y = lerp(particleSizeMin.y, particleSizeMax.y, rand(seed));


	p.lifeTime      = lerp(particleLifeTimeMin, particleLifeTimeMax, rand(seed));
	p.lifeTimer     = p.lifeTime;
	p.angle         = lerp(particleAngleMin, particleAngleMax, rand(seed));
	p.addAngle      = particleAddAngle;
	p.gravity       = particleGravity;
	p.kind          = particleKind;
	p.billboardType = particleBillboardType;
	p.textureType   = particleTextureType;

	p.startVelocity = p.velocity;
	p.startScale    = p.scale;

	// �ʂ̐ݒ�
	switch (particleKind)
	{
	case pk_Dust:
		{
			float3 pos = emitterPosition;

			float3 pos2 = float3(0, 0, 0);

			pos2.x = rand(seed) * 2 - 1;
			pos2.y = rand(seed) * 2 - 1;
			pos2.z = rand(seed) * 2 - 1;

			pos2 = normalize(pos2) * pow(rand(seed), 1.0 / 3.0);

			p.position.x = pos.x + pos2.x * 30;
			p.position.y = pos.y + 5 * rand(seed);
			p.position.z = pos.z + pos2.z * 30;


			float r = rand(seed) * 360;

			float particleSpeed = lerp(particleSpeedMin.x, particleSpeedMax.x, rand(seed));
			p.velocity.y        = -1 * particleSpeed;
			// x ����
			particleSpeed = lerp(particleSpeedMin.x, particleSpeedMax.x, rand(seed));
			p.velocity.x  = cos(r) * particleSpeed;

			// z ����
			particleSpeed = lerp(particleSpeedMin.x, particleSpeedMax.x, rand(seed));
			p.velocity.z  = -sin(r) * particleSpeed;


			p.textureType = 3;
			if (rand(seed) > 0.66)
				p.textureType = 4;
			if (rand(seed) > 0.66)
				p.textureType = 5;
		}

		break;
	case 1:
		{
			p.position = emitterPosition;

			float particleSpeed = lerp(particleSpeedMin.x, particleSpeedMax.x, rand(seed));
			if (rand(seed) > 0.5)
				particleSpeed *= -1;
			p.velocity.x = seed * particleSpeed;
			if (rand(seed) > 0.5)
				particleSpeed *= -1;
			p.velocity.y = seed * particleSpeed;
			if (rand(seed) > 0.5)
				particleSpeed *= -1;
			p.velocity.z = seed * particleSpeed;
		}
		break;
	case 2:
	case 3:
		{
			p.position = emitterPosition;
			p.velocity = float3(0, 0, 0);
		}
		break;
	case 4:
		{
			float3 pos = emitterPosition;

			float3 pos2 = float3(0, 0, 0);

			pos2.x = rand(seed) * 2 - 1;
			pos2.y = rand(seed) * 2 - 1;
			pos2.z = rand(seed) * 2 - 1;

			pos2 = normalize(pos2) * pow(rand(seed), 1.0 / 3.0);


			p.position.x = pos.x + pos2.x * 1.5;
			p.position.y = 0;
			p.position.z = pos.z + pos2.z * 1.5;


			float particleSpeed = lerp(particleSpeedMin.x, particleSpeedMax.x, rand(seed));

			p.velocity.x = 0;
			p.velocity.y = 1 * particleSpeed;
			p.velocity.z = 0;
		}
		break;
	case 5:
		{
			p.position   = emitterPosition;
			float y      = rand(seed) * 10;
			p.position.y = y;

			float particleSpeed = lerp(particleSpeedMin.x, particleSpeedMax.x, rand(seed));
			p.velocity.y        = 1 * particleSpeed;

			if (rand(seed) > 0.5)
				particleSpeed *= -1;
			p.velocity.x = rand(seed) * particleSpeed;


			if (rand(seed) > 0.5)
				particleSpeed *= -1;
			p.velocity.z = rand(seed) * particleSpeed;
		}
		break;
	case 6:
		{
			p.position = emitterPosition;

			p.velocity.y = 0.0;

			float particleSpeed = lerp(particleSpeedMin.x, particleSpeedMax.x, rand(seed));

			float r = rand(seed) * 360;

			// x ����
			p.velocity.x = cos(r) * particleSpeed;

			// z ����
			p.velocity.z = -sin(r) * particleSpeed;
		}
		break;
	}
}

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
	float  speed;
	float3 velocity;
	float3 startVelocity;
	float  gravity;
	float  angle;
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
	float  particleGravity;
	float  particleBillboardType;
	float  particleTextureType;
	float3 emitterPosition;
	float  emitCount;
};


//--------------------------------------------
//	�����_���l�֐� float�^
//--------------------------------------------
// n		: �V�[�h�l
float random(float seed)
{
	seed *= deltaTime;
	seed = frac(sin(seed) * 43758.5453123);
	return seed;
}


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
	float seed = _id * nowTime;

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

	p.speed = lerp(particleSpeedMin.x, particleSpeedMax.x, rand(seed));

	p.lifeTime      = lerp(particleLifeTimeMin, particleLifeTimeMax, rand(seed));
	p.lifeTimer     = p.lifeTime;
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

			p.position.x = pos.x + pos2.x * 60;
			p.position.y = pos.y + 40 * rand(seed);
			p.position.z = pos.z + pos2.z * 60;


			float r = (rand(seed) * 360) * 3.14 / 180;

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
	case pk_PlayerAttackSpark:
		{
			p.position = emitterPosition;

			float particleSpeed = lerp(particleSpeedMin.x, particleSpeedMax.x, rand(seed));
			if (rand(seed) > 0.5)
				particleSpeed *= -1;
            p.velocity.x = rand(seed) * particleSpeed * 2;
			if (rand(seed) > 0.5)
				particleSpeed *= -1;
            p.velocity.y = rand(seed) * particleSpeed;
			if (rand(seed) > 0.5)
				particleSpeed *= -1;
            p.velocity.z = rand(seed) * particleSpeed * 2;
            }
		break;
    case pk_PlayerAttackHit:
	    {
            p.position = emitterPosition;
            p.velocity = float3(0, 0, 0);
            p.angle = (rand(seed) * 360);
	    }
        break;
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
	case pk_smoke:
		{
			p.position = emitterPosition;

			p.velocity.y = 0.0;

			float particleSpeed = lerp(particleSpeedMin.x, particleSpeedMax.x, rand(seed));

			float r = (rand(seed) * 360) * 3.14 / 180;

			// x ����
			p.velocity.x = cos(r) * particleSpeed;

			// z ����
			p.velocity.z = -sin(r) * particleSpeed;

			p.angle = 0;
		}
		break;
	case pk_simpleFire:
		{
			float3 pos = emitterPosition;

            p.scale.x = lerp(particleSizeMin.x, particleSizeMax.x, rand(seed));
			p.scale.y = p.scale.x;

			float3 pos2 = float3(0, 0, 0);

			p.angle = rand(seed) * 360;

			pos2.x = rand(seed) * 2 - 1;
			pos2.y = rand(seed) * 2 - 1;
			pos2.z = rand(seed) * 2 - 1;

			pos2 = normalize(pos2) * pow(rand(seed), 1.0 / 3.0);

			p.position.x = pos.x + pos2.x * 1;
			p.position.y = pos.y + 1 * rand(seed);
			p.position.z = pos.z + pos2.z * 1;

			p.textureType = 7;
			if (rand(seed) > 0.5)
				p.textureType = 8;
		}
		break;
	case pk_novaBurst:
		{
			p.position   = emitterPosition;
			p.position.y = emitterPosition.y + rand(seed) * 1.5;


			p.velocity.y = rand(seed) * 8.0;

			float particleSpeed = lerp(particleSpeedMin.x, particleSpeedMax.x, rand(seed));

			float r = (rand(seed) * 360) * 3.14 / 180;

			// x ����
			p.velocity.x = cos(r) * particleSpeed;

			// z ����
			p.velocity.z = -sin(r) * particleSpeed;

			p.textureType = 3;
			if (rand(seed) > 0.66)
				p.textureType = 4;
			if (rand(seed) > 0.66)
				p.textureType = 5;
		}
		break;
	case pk_novaStartFire:
		{
			p.position = emitterPosition;

			p.velocity.y = 0.0;

			float particleSpeed = lerp(particleSpeedMin.x, particleSpeedMax.x, rand(seed));

			float r = (rand(seed) * 360) * 3.14 / 180;

			// x ����
			p.velocity.x = cos(r) * particleSpeed;

			// z ����
			p.velocity.z = -sin(r) * particleSpeed;

			p.angle = 0;
		}
		break;
	case pk_fireBreath:
		{
			p.position = emitterPosition;
			p.position.y += 3;

			p.angle = rand(seed) * 360;


			float particleSpeed = lerp(particleSpeedMin.x, particleSpeedMax.x, rand(seed));

			float r = rand(seed) / 3.14 * 0.1;

			// x ����
			p.velocity.x = 0;

			// y ����
			p.velocity.y = 0;

			// z ����
			p.velocity.z = 1 * particleSpeed;
		}
		break;
	case pk_beamCylinder:
		{
			p.position = emitterPosition;
			float r    = (rand(seed) * 360) * 3.14 / 180;


			// x ����
			p.position.x = p.position.x + cos(r) * 0.8f;

			// y ����
			p.position.y = p.position.y - sin(r) * 0.8f;


			float particleSpeed = lerp(particleSpeedMin.x, particleSpeedMax.x, rand(seed));

			// x ����
			p.velocity.x = 0;

			// y ����
			p.velocity.y = 0;

			// z ����
			p.velocity.z = 1 * particleSpeed;
		}
		break;
	case pk_titleSelect:
		{
			p.scale.y = p.scale.x;

			p.position   = emitterPosition;
            float r = (rand(seed) * 360) * 3.14 / 180;
            float length = rand(seed) * 10;
			
			// x ����
			p.position.x = p.position.x + cos(r) * length;
			// y ����
			p.position.y = p.position.y - sin(r) * length;
		}
		break;
	}
}

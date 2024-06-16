#include "Particles.hlsli"

RWStructuredBuffer<Particle>  particleBuffer : register(u0);
ConsumeStructuredBuffer<uint> particlePool : register(u1);


[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	// 未使用リストの末尾から取り出す
	uint id = particlePool.Consume();

	const float noise_scale = 1.0;
	float       f0          = rand(id * noise_scale * emitCount * deltaTime);
	float       f1          = rand(f0 * noise_scale);
	float       f2          = rand(f1 * noise_scale);
	float       f3          = rand(f2 * noise_scale);
	float       f4          = rand(f3 * noise_scale);
	float       f5          = rand(f4 * noise_scale);
	float       f6          = rand(f5 * noise_scale);
	float       f7          = rand(f6 * noise_scale);
	float       f8          = rand(f7 * noise_scale);
	float       seed        = random(float2(f0, deltaTime));


	Particle p = particleBuffer[id];

	// 共通
	p.isActive = true;

	p.color.x = RandomRange(particleColorMin.x, particleColorMax.x, f0);
	p.color.y = RandomRange(particleColorMin.y, particleColorMax.y, f1);
	p.color.z = RandomRange(particleColorMin.z, particleColorMax.z, f2);
	p.color.w = RandomRange(particleColorMin.w, particleColorMax.w, f3);

	p.scale.x = RandomRange(particleSizeMin.x, particleSizeMax.x, f4);
	p.scale.y = RandomRange(particleSizeMin.y, particleSizeMax.y, f5);


	p.lifeTime      = RandomRange(particleLifeTimeMin, particleLifeTimeMax, f6);
	p.lifeTimer     = p.lifeTime;
	p.friction      = RandomRange(particleFrictionMin, particleFrictionMax, f7);
	p.angle         = RandomRange(particleAngleMin, particleAngleMax, f8);
	p.addAngle      = particleAddAngle;
	p.gravity       = particleGravity;
	p.kind          = particleKind;
	p.billboardType = particleBillboardType;
	p.textureType   = particleTextureType;

	p.startVelocity = p.velocity;
	p.startScale    = p.scale;

	// 個別の設定
	switch (particleKind)
	{
	case 0:
		{
			float3 pos = emitterPosition;

			float3 pos2 = float3(0, 0, 0);
			while (1)
			{
				f4     = rand(seed);
				f5     = rand(f4);
				f6     = rand(f5);
				f7     = rand(f6);
				pos2.x = rand(f5) * 2 - 1;
				pos2.y = rand(f5) * 2 - 1;
				pos2.z = rand(f7) * 2 - 1;

				if (dot(pos2, pos2) < 1.0f)
				{
					break;
				}
				seed += 1.0f;
			}

			p.position.x = pos.x + pos2.x * 30;
			p.position.y = pos.y + 5 * rand(f6);
			p.position.z = pos.z + pos2.z * 30;


			float particleSpeed = RandomRange(particleSpeedMin.x, particleSpeedMax.x, seed);

			p.velocity.x = 0;
			p.velocity.y = -1 * particleSpeed;
			p.velocity.z = 0;
		}

		break;
	case 1:
		{
			p.position = emitterPosition;

			float particleSpeed = RandomRange(particleSpeedMin.x, particleSpeedMax.x, seed);
			seed                = random(float2(seed, deltaTime));
			if (f0 > 0.5) particleSpeed *= -1;
			p.velocity.x = seed * particleSpeed;
			seed         = random(float2(seed, deltaTime));
			if (f1 > 0.5)
				particleSpeed *= -1;
			p.velocity.y = seed * particleSpeed;
			seed         = random(float2(seed, deltaTime));
			if (f2 > 0.5)
				particleSpeed *= -1;
			p.velocity.z = seed * particleSpeed;
			seed         = random(float2(seed, deltaTime));
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
			while (1)
			{
				f4     = rand(seed);
				f5     = rand(f4);
				f6     = rand(f5);
				f7     = rand(f6);
				pos2.x = rand(f5) * 2 - 1;
				pos2.y = rand(f5) * 2 - 1;
				pos2.z = rand(f7) * 2 - 1;

				if (dot(pos2, pos2) < 1.0f)
				{
					break;
				}
				seed += 1.0f;
			}

			p.position.x = pos.x + pos2.x * 1.5;
			p.position.y = 0;
			p.position.z = pos.z + pos2.z * 1.5;


			float particleSpeed = RandomRange(particleSpeedMin.x, particleSpeedMax.x, seed);

			p.velocity.x = 0;
			p.velocity.y = 1 * particleSpeed;
			p.velocity.z = 0;
		}
		break;
	}


	particleBuffer[id] = p;
}

#include "Particles.hlsli"

RWStructuredBuffer<Particle>  particleBuffer : register(u0);
ConsumeStructuredBuffer<uint> particlePool : register(u1);


[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	// 未使用リストの末尾から取り出す
	uint id = particlePool.Consume();

	const float noise_scale = 1.0;
	float       f0          = rand(id * noise_scale * emitCount);
	float       f1          = rand(f0 * noise_scale);
	float       f2          = rand(f1 * noise_scale);

	Particle p = particleBuffer[id];
	p.isActive = true;


	float seed = random(float2(f2, deltaTime));
	float f4   = f0;
	float f5   = rand(f4);
	float f6   = rand(f5);
	float f7   = rand(f6);

	switch (particleKind)
	{
	case 0:
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
		p.position = emitterPosition;
		p.velocity.x    = (rand(f5) * 2 - 1) * 3;
		p.velocity.z    = (rand(f6) * 2 - 1) * 3;
		p.velocity.y    = (rand(f7) * 2 - 1) * 3;
		p.billboardType = 1;
		break;
	}


	// 共通
	p.color.x = RandomRange(particleColorMin.x, particleColorMax.x, seed);
	seed      = rand(seed);
	p.color.y = RandomRange(particleColorMin.y, particleColorMax.y, seed);
	seed      = rand(seed);
	p.color.z = RandomRange(particleColorMin.z, particleColorMax.z, seed);
	seed      = rand(seed);
	p.color.w = RandomRange(particleColorMin.w, particleColorMax.w, seed);
	seed      = rand(seed);

	p.scale.x = RandomRange(particleSizeMin.x, particleSizeMax.x, seed);
	seed      = rand(seed);
	p.scale.y = RandomRange(particleSizeMin.y, particleSizeMax.y, seed);
	seed      = rand(seed);


	p.lifeTime      = RandomRange(particleLifeTimeMin, particleLifeTimeMax, seed);
	p.lifeTimer     = p.lifeTime;
	seed            = rand(seed);
	p.friction      = RandomRange(particleFrictionMin, particleFrictionMax, seed);
	p.addAngle      = particleAddAngle;
	p.gravity       = particleGravity;
	p.kind          = particleKind;
	p.billboardType = particleBillboardType;

	p.startVelocity = p.velocity;
	p.startScale    = p.scale;


	particleBuffer[id] = p;
}

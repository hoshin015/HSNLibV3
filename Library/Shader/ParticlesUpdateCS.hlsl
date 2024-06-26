#include "Particles.hlsli"
#include "../RegisterNum.h"

RWStructuredBuffer<Particle> particleBuffer : register(u0);
AppendStructuredBuffer<uint> deadList : register(u1);

[numthreads(THREAD_NUM_X, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint id = DTid.x;

	Particle p = particleBuffer[id];

	// Žg—p’†‚È‚çˆ—‚ð‚·‚é
	if (p.isActive)
	{
		switch (p.kind)
		{
		case 0:
			{
				p.position += p.velocity * deltaTime;

				// x
				while (p.position.x > 30)
				{
					p.position.x -= 30;
				}
				while (p.position.x < -30)
				{
					p.position.x += 30;
				}

				// y
				while (p.position.y < -2)
				{
					p.position.y += 10;
				}

				while (p.position.y > 10)
				{
					p.position.y -= 10;
				}

				// x
				while (p.position.z > 30)
				{
					p.position.z -= 30;
				}
				while (p.position.z < -30)
				{
					p.position.z += 30;
				}
			}
			break;
		case 1:
			{
				p.velocity -= (p.velocity * 0.3f * deltaTime);
				p.velocity.y += -p.gravity * deltaTime;
				p.position += p.velocity * deltaTime;

				p.color.a = (p.lifeTimer / p.lifeTime);

				p.scale = p.startScale * (p.lifeTimer / p.lifeTime);

				p.lifeTimer -= deltaTime;
			}
			break;
		case 2:
			{
				p.color.a = (p.lifeTimer / p.lifeTime);

				p.scale = p.startScale * (p.lifeTimer / p.lifeTime);
				p.lifeTimer -= deltaTime;
			}
			break;
		case 3:
			{
				p.scale = p.startScale * (1 - (p.lifeTimer / p.lifeTime));
				p.lifeTimer -= deltaTime;
			}
			break;
		case 4:
			{
				//p.velocity.x += (RandomRange(0, 10) - 5) * deltaTime;
				//p.velocity.z += (RandomRange(0, 10) - 5) * deltaTime;
				p.position += p.velocity * deltaTime;
				p.color.a = (p.lifeTimer / p.lifeTime);
				p.scale   = p.startScale * (p.lifeTimer / p.lifeTime);
				p.lifeTimer -= deltaTime;
			}
			break;
		case 5:
			{
				p.velocity -= (p.velocity * 0.3f * deltaTime);
				p.velocity.y += -p.gravity * deltaTime;
				p.position += p.velocity * deltaTime;

				p.color.a = (p.lifeTimer / p.lifeTime);

				p.scale = p.startScale * (p.lifeTimer / p.lifeTime);

				p.lifeTimer -= deltaTime;
			}
			break;
		case 6:
			{
				p.position += p.velocity * deltaTime;

				if ((p.lifeTimer / p.lifeTime) > 0.2)
				{
					p.color.a = 1.0f - (p.lifeTimer / p.lifeTime);
				}
				else
				{
					p.color.a -= 1 * deltaTime;
				}

				//p.scale = p.startScale * (p.lifeTimer / p.lifeTime);

				p.lifeTimer -= deltaTime;
			}
			break;
		default:
			break;
		}

		if (p.lifeTimer <= 0)
		{
			p.velocity = float3(0, 0, 0);
			p.isActive = false;
			p.scale    = 0;
			deadList.Append(id);
		}
		particleBuffer[id] = p;
	}
}

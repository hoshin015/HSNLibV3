#include "Particles.hlsli"
#include "../RegisterNum.h"

RWStructuredBuffer<Particle> particleBuffer : register(u0);
AppendStructuredBuffer<uint> deadList : register(u1);

SamplerState samplerStates[_samplerNum] : register(s0);
Texture2D    perlinNoiseTexture : register(_perlinNoiseTexture);

[numthreads(THREAD_NUM_X, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint id = DTid.x;

	Particle p = particleBuffer[id];

	// 使用中なら処理をする
	if (p.isActive)
	{
		switch (p.kind)
		{
		case pk_Dust:
			{
				// TODO: perlinNoise で滑らかに動かす
#if 0
				// パーリンノイズのテクスチャから値を取得するためのUV座標
				float2 uv = frac(float2(p.position.x, p.position.z) * 0.01 + float2(random(id) * 0.01, random(id) * 0.01));
				// IDで調整して正規化

				// パーリンノイズの値を取得
				float noiseX = perlinNoiseTexture.SampleLevel(samplerStates[_pointSampler], uv, 0).r;
				float noiseY = perlinNoiseTexture.SampleLevel(samplerStates[_pointSampler], uv + float2(0.1, 0.1), 0).r;
				float noiseZ = perlinNoiseTexture.SampleLevel(samplerStates[_pointSampler], uv + float2(0.2, 0.2), 0).r;
				float noiseA = perlinNoiseTexture.SampleLevel(samplerStates[_pointSampler], uv + float2(0.3, 0.3), 0).r;

				// パーティクルの速度を更新
				p.velocity.x += (noiseX - 0.5) * p.speed; // ノイズ値を中心に移動
				p.velocity.y += (noiseY - 0.5) * p.speed;
				p.velocity.z += (noiseZ - 0.5) * p.speed;

				p.angle += (noiseA - 0.5) * 0.1;

				// パーティクルの位置を更新
				//p.position += p.velocity * 0.1; // 移動速度を調整
#endif
				p.position += p.velocity * deltaTime;

				// x
				while (p.position.x > 60)
				{
					p.position.x -= 120;
				}
				while (p.position.x < -60)
				{
					p.position.x += 120;
				}

				// y
				while (p.position.y < -2)
				{
					p.position.y += 40;
				}

				while (p.position.y > 40)
				{
					p.position.y -= 42;
				}

				// x
				while (p.position.z > 60)
				{
					p.position.z -= 120;
				}
				while (p.position.z < -60)
				{
					p.position.z += 120;
				}
			}
			break;
		case pk_PlayerAttackSpark:
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
		case pk_smoke:
			{
				p.position += p.velocity * deltaTime;

				if ((p.lifeTimer / p.lifeTime) > 0.5)
				{
					p.color.a = 1.0f - (p.lifeTimer / p.lifeTime);
				}
				else
				{
					p.color.a -= 3 * deltaTime;
				}

				//p.scale = p.startScale * (p.lifeTimer / p.lifeTime);

				p.lifeTimer -= deltaTime;
			}
			break;
		case pk_simpleFire:
			{
				p.color.a = (p.lifeTimer / p.lifeTime);

				//p.scale = p.startScale * (p.lifeTimer / p.lifeTime);
				p.lifeTimer -= deltaTime;
			}
			break;
		case pk_novaBurst:
			{
				p.position += p.velocity * deltaTime;

				if ((p.lifeTimer / p.lifeTime) < 0.9)
				{
					p.velocity.y += 8 * deltaTime;
					p.velocity.x -= (p.velocity.x * 0.66 * deltaTime);
					p.velocity.z -= (p.velocity.z * 0.66 * deltaTime);
				}
				p.lifeTimer -= deltaTime;
			}
			break;
		case pk_novaStartFire:
			{
				p.position += p.velocity * deltaTime;
				p.lifeTimer -= deltaTime;
			}
			break;
		case pk_fireBreath:
			{
				p.position += p.velocity * deltaTime;
				p.lifeTimer -= deltaTime;
			}
			break;
		case pk_beamCylinder:
			{
				p.position += p.velocity * deltaTime;
				p.lifeTimer -= deltaTime;
			}
			break;
		case pk_titleSelect:
			{
				p.color.a = (p.lifeTimer / p.lifeTime);
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

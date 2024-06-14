#include "Particles.hlsli"

RWStructuredBuffer<Particle> particleBuffer : register(u0);
AppendStructuredBuffer<uint> deadList : register(u1);

[numthreads(16, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint id = DTid.x;
    
    Particle p = particleBuffer[id];
    
    // égópíÜÇ»ÇÁèàóùÇÇ∑ÇÈ
    if(p.isActive)
    {
        if (p.kind == 0)
        {
            p.position += p.velocity * deltaTime;
            
            while (p.position.y < -2)
            {
                p.position.y += 10;
            }

            while(p.position.y > 10)
            {
                p.position.y -= 10;
            }
        }
        if(p.kind == 1)
        {
            p.velocity -= (p.velocity * 0.3f * deltaTime);
            p.velocity.y += -p.gravity * deltaTime;
            p.position += p.velocity * deltaTime;

            p.color.a = (p.lifeTimer / p.lifeTime);

            p.scale = p.startScale * (p.lifeTimer / p.lifeTime);

            p.lifeTimer -= deltaTime;
        }
        if(p.kind == 2)
        {
            p.velocity.y += -p.gravity * deltaTime;
            p.position += p.velocity * deltaTime;

            p.color.a = (p.lifeTimer / p.lifeTime);

            //p.scale = p.startScale * (length(p.velocity) / length(p.startVelocity));

            p.lifeTimer -= deltaTime;
        }
        
        if(p.lifeTimer <= 0)
        {
            p.velocity = float3(0, 0, 0);
            p.isActive = false;
            p.scale = 0;
            deadList.Append(id);
        }
        particleBuffer[id] = p;
    }
}
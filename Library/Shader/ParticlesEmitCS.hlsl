#include "Particles.hlsli"

RWStructuredBuffer<Particle> particleBuffer : register(u0);
ConsumeStructuredBuffer<uint> particlePool : register(u1);

float rand(float n)
{
    return frac(sin(n) * 43758.5453123);
}


[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{   
    // 未使用リストの末尾から取り出す
    uint id = particlePool.Consume();
 
    const float noise_scale = 1.0;
    float f0 = rand(id * noise_scale);
    float f1 = rand(f0 * noise_scale);
    float f2 = rand(f1 * noise_scale);
    
    Particle p = particleBuffer[id];
    p.isActive = true;
    
    
    float seed = f2 + deltaTime;
    float f4 = f0;
    float f5 = 0;
    float f6 = 0;
    float f7 = 0;
    f5 = rand(f4);
    f6 = rand(f5);
    f7 = rand(f6);
    
    switch (particleKind)
    {
    case 0:
        float3 pos = emitterPosition;
        
        float3 pos2 = float3(0, 0, 0);
        while (1)
        {
            f4 = rand(seed);
            f5 = rand(f4);
            f6 = rand(f5);
            f7 = rand(f6);
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
        p.position.y = pos.y + 10 * rand(f6);
        p.position.z = pos.z + pos2.z * 30;
    
    
        p.velocity.y = -5;
        break;
    case 1:
    {
         float3 pos = emitterPosition;
     
     
         float3 pos2 = float3(0, 0, 0);
         while (1)
         {
             f4 = rand(seed);
             f5 = rand(f4);
             f6 = rand(f5);
             f7 = rand(f6);
             pos2.x = rand(f5) * 2 - 1;
             pos2.y = rand(f6) * 2 - 1;
             pos2.z = rand(f7) * 2 - 1;
     
             if (dot(pos2, pos2) < 1.0f)
             {
                 break;
             }
             seed += 1.0f;
         }
     
         p.position.x = pos.x + pos2.x * 1.5;
         p.position.y = pos.y + pos2.y * 1.5;
         p.position.z = pos.z + pos2.z * 1.5;
     
     
     
         p.velocity.x = normalize(emitterPosition.x - p.position.x) * 0.1f;
         p.velocity.y = normalize(emitterPosition.y - p.position.y) * 0.1f;
         p.velocity.z = normalize(emitterPosition.z - p.position.z) * 0.1f;
     }
        break;
    case 2:
        p.position = emitterPosition;
        p.velocity.x = rand(f5) * 2 - 1;
        p.velocity.z = rand(f6) * 2 - 1;
        p.velocity.y = rand(f7) * 2 - 1;
        break;
    }
    
    
    p.color = particleColor;
    
    p.lifeTime = particleLifeTime;
    p.scale = particleSize;
    p.kind = particleKind;
    
    particleBuffer[id] = p;
}
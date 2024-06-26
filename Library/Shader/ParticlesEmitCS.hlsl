#include "Particles.hlsli"
#include "../RegisterNum.h"

RWStructuredBuffer<Particle>  particleBuffer : register(u0);
ConsumeStructuredBuffer<uint> particlePool : register(u1);


[numthreads(THREAD_NUM_X, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	// 未使用リストの末尾から取り出す
    uint id = particlePool.Consume();
    
    Particle p = particleBuffer[id];

    spawn(id, p);

    particleBuffer[id] = p;
}

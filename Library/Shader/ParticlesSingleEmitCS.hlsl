#include "Particles.hlsli"

RWStructuredBuffer<Particle>  particleBuffer : register(u0);
ConsumeStructuredBuffer<uint> particlePool : register(u1);


[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	// ���g�p���X�g�̖���������o��
    uint id = particlePool.Consume();
    
    Particle p = particleBuffer[id];

    spawn(id, p);

    particleBuffer[id] = p;
}
#include "Particles.hlsli"
#include "../RegisterNum.h"

RWStructuredBuffer<Particle> particleBuffer : register(u0);
AppendStructuredBuffer<uint> deadList : register(u1);

[numthreads(THREAD_NUM_X, 1, 1)]
void main( uint3 dTid : SV_DispatchThreadID )
{
    uint id = dTid.x;
    
    Particle p = particleBuffer[id];
    p.isActive = false;
    p.scale = float2(0, 0);
    particleBuffer[id] = p;
    deadList.Append(id);     // 未使用リスト (deadList) の末尾に追加
}
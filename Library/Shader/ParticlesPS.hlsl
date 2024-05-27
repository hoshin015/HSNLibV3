#include "Particles.hlsli"
#include "../RegisterNum.h"

SamplerState samplerStates[_samplerNum] : register(s0);

Texture2D particleTexture : register(t9);

float4 main(GS_OUT pin) : SV_TARGET
{
    float4 textureColor = particleTexture.Sample(samplerStates[_pointSampler], pin.texcoord);
    float4 finalColor = textureColor * pin.color;
    return finalColor;
    
    return pin.color;
}
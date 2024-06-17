#include "../RegisterNum.h"

SamplerState samplerStates[_samplerNum] : register(s0);

Texture2D hdrColorBufferTexture : register(_hdrColorBufferTexture);

float4 main(float4 position : SV_POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
{
    return hdrColorBufferTexture.Sample(samplerStates[_linearSampler], texcoord, 0.0f);
}
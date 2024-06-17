#include "../RegisterNum.h"
#include "GaussianBlur.hlsli"

SamplerState samplerStates[_samplerNum] : register(s0);

Texture2D hdrColorBufferTexture : register(_hdrColorBufferTexture);

float4 main(float4 position : SV_POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
{
    float4 sampledColor = hdrColorBufferTexture.Sample(samplerStates[_pointSampler], texcoord);

#if 0
	return float4(step(threshould, dot(sampledColor.rgb, float3(0.299, 0.587, 0.114))) * sampledColor.rgb * intensity, sampledColor.a);
#else	
    return float4(step(threshould, max(sampledColor.r, max(sampledColor.g, sampledColor.b))) * sampledColor.rgb, sampledColor.a);
#endif	
}
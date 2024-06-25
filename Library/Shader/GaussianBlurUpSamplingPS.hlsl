#include "../RegisterNum.h"
#include "GaussianBlur.hlsli"

SamplerState samplerStates[_samplerNum] : register(s0);

Texture2D downSampledTexture[GAUSSIAN_DOWNSAMPLING_COUNT] : register(_downSampledTexture);

float4 main(float4 position : SV_POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
{
    float3 sampledColor = 0;
	[unroll]
    for (uint downsampledIndex = 0; downsampledIndex < GAUSSIAN_DOWNSAMPLING_COUNT; ++downsampledIndex)
    {
        sampledColor += downSampledTexture[downsampledIndex].Sample(samplerStates[_linearSampler], texcoord).xyz;
    }
    return float4(sampledColor * intensity, 1);
}
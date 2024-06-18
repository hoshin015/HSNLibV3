#include "FullScreenQuad.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

SamplerState samplerStates[3] : register(s0);
Texture2D textureMap : register(t0);
Texture2D bloomMap : register(t1);

float3 reinhard_tone_mapping(float3 color)
{
    float luma = dot(color, float3(0.2126, 0.7152, 0.0722));
    float tone_mapped_luma = luma / (1. + luma);
    color *= tone_mapped_luma / luma;
    return color;
}

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = textureMap.Sample(samplerStates[POINT], pin.texcoord);

    clip(color.a);
    
    // ブルームテクスチャを加算する
    color.rgb += bloomMap.Sample(samplerStates[POINT], pin.texcoord).rgb;

    // Tone map
    //color.rgb =saturate(color.rgb);
    
    return color;

}
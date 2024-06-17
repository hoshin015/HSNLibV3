#include "../RegisterNum.h"
#include "FullScreenQuad.hlsli"


SamplerState samplerStates[_samplerNum] : register(s0);

Texture2D accumTexture : register(_accumTexture);
Texture2D revealTexture : register(_revealTexture);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 accum = accumTexture.Sample(samplerStates[_pointSampler], pin.texcoord);
    float reveal = revealTexture.Sample(samplerStates[_pointSampler], pin.texcoord).r;

    clip(1.0 - reveal - 0.01);
    
    // カラーを正規化
    float3 normalizedColor = accum.rgb / max(accum.a, 1e-5);
    
    // 最終カラーを計算
    float4 finalColor = float4(normalizedColor, 1 - reveal);

    return finalColor;
}
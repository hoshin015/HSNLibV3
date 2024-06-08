#include "FullScreenQuad.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

SamplerState samplerStates[3] : register(s0);
Texture2D accumTexture : register(t0);
Texture2D revealTexture : register(t1);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 accum = accumTexture.Sample(samplerStates[POINT], pin.texcoord);
    float reveal = revealTexture.Sample(samplerStates[POINT], pin.texcoord).r;
    
    // カラーを正規化
    float3 normalizedColor = accum.rgb / max(accum.a, 1e-5);
    
    // 最終カラーを計算
    float4 finalColor = float4(normalizedColor, 1 - reveal);

    return finalColor;
}
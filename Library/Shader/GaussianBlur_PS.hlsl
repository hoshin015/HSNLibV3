#include "GaussianBlur.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define TEXT_LINEAR 3
#define LINEAR_BORDER_BLACK 4
#define LINEAR_BORDER_WHITE 5

SamplerState samplerStates[5] : register(s0);

float4 main(GAUSSIAN_VS_OUT pin) : SV_TARGET
{
    float4 color = (float4) 0;

    // 右８テクセルサンプリング
    color = weights[0].x * textureMap.Sample(samplerStates[POINT], pin.tex0.xy);
    color += weights[0].y * textureMap.Sample(samplerStates[POINT], pin.tex1.xy);
    color += weights[0].z * textureMap.Sample(samplerStates[POINT], pin.tex2.xy);
    color += weights[0].w * textureMap.Sample(samplerStates[POINT], pin.tex3.xy);
    color += weights[1].x * textureMap.Sample(samplerStates[POINT], pin.tex4.xy);
    color += weights[1].y * textureMap.Sample(samplerStates[POINT], pin.tex5.xy);
    color += weights[1].z * textureMap.Sample(samplerStates[POINT], pin.tex6.xy);
    color += weights[1].w * textureMap.Sample(samplerStates[POINT], pin.tex7.xy);

    // 左８ピクセルサンプリング
    color += weights[0].x * textureMap.Sample(samplerStates[POINT], pin.tex0.zw);
    color += weights[0].y * textureMap.Sample(samplerStates[POINT], pin.tex1.zw);
    color += weights[0].z * textureMap.Sample(samplerStates[POINT], pin.tex2.zw);
    color += weights[0].w * textureMap.Sample(samplerStates[POINT], pin.tex3.zw);
    color += weights[1].x * textureMap.Sample(samplerStates[POINT], pin.tex4.zw);
    color += weights[1].y * textureMap.Sample(samplerStates[POINT], pin.tex5.zw);
    color += weights[1].z * textureMap.Sample(samplerStates[POINT], pin.tex6.zw);
    color += weights[1].w * textureMap.Sample(samplerStates[POINT], pin.tex7.zw);
    
    return float4(color.xyz, 1.0f);
}
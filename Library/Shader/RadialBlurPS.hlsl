#include "../RegisterNum.h"
#include "FullScreenQuad.hlsli"

cbuffer LuminanceExtractionConstant : register(_luminanceExtractionConstant)
{
    float threshould; // 高輝度抽出のための閾値
    float intensity; // ブルームの強度
    float2 lumiPad;
}

cbuffer RadialBlurConstant : register(_radialBlurConstant)
{
    float2 blurCenter;      // ブラーの中心座標
    float blurPower;        // ブラーの強度
    float samplingCount;    // サンプリング回数
}


SamplerState samplerStates[_samplerNum] : register(s0);

Texture2D textureMaps : register(_radialBlurTexture);

float4 main(VS_OUT pin) : SV_TARGET
{
    float2 direction = pin.texcoord - blurCenter;
    float3 c = float3(0, 0, 0);

    float f = 1.0 / float(samplingCount);
    for (int i = 0; i < samplingCount; i++)
    {
        c += textureMaps.Sample(samplerStates[_pointSampler], pin.texcoord - blurPower * direction * float(i)).rgb * f;
    }

    float4 color = float4(c, 1.0);
    
#if USE_LinearWorkflow
    color.rgb = pow(color.rgb, 2.2f);
#endif

    return color;
}

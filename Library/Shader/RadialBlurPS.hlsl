#include "../RegisterNum.h"
#include "FullScreenQuad.hlsli"

cbuffer RadialBlurConstant : register(_radialBlurConstant)
{
    float2 blurCenter;      // ブラーの中心座標
    float blurPower;        // ブラーの強度
    float samplingCount;    // サンプリング回数
}

//float InterleavedGradientNoise(float2 pixCoord, int frameCount)
//{
//    const float3 magic = float3(0.06711056f, 0.00583715f, 52.9829189f);
//    float2 frameMagicScale = float2(2.083f, 4.867f);
//    pixCoord += frameCount * frameMagicScale;
//    return frac(magic.z * frac(dot(pixCoord, magic.xy)));
//}

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

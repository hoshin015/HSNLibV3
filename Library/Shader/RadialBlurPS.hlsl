#include "../RegisterNum.h"
#include "FullScreenQuad.hlsli"

cbuffer LuminanceExtractionConstant : register(_luminanceExtractionConstant)
{
    float threshould; // ���P�x���o�̂��߂�臒l
    float intensity; // �u���[���̋��x
    float2 lumiPad;
}

cbuffer RadialBlurConstant : register(_radialBlurConstant)
{
    float2 blurCenter;      // �u���[�̒��S���W
    float blurPower;        // �u���[�̋��x
    float samplingCount;    // �T���v�����O��
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

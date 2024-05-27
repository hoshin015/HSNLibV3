#include "../RegisterNum.h"
#include "FullScreenQuad.hlsli"
#include "FilterFunctions.hlsli"

cbuffer LuminanceExtractionConstant : register(_luminanceExtractionConstant)
{
    float threshould;   // ���P�x���o�̂��߂�臒l
    float intensity;    // �u���[���̋��x
    float2 lumiPad;
}

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

SamplerState samplerStates[3] : register(s0);
Texture2D textureMaps    : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
    // ���ʂɕ`�悳�ꂽ�e�N�X�`��
    float4 color = textureMaps.Sample(samplerStates[POINT], pin.texcoord);
    
    // RGB > �P�x�l�ɕϊ�
    float luminance = RGB2Luminance(color.rgb);
    clip(luminance - 0.01f);
    
    // 臒l�Ƃ̍����Z�o
    float contribution = max(0, luminance - threshould);
    
    // �o�͂���F��␳����
    contribution /= luminance;
    color.rgb *= contribution * intensity;

#if USE_LinearWorkflow
    color.rgb = pow(color.rgb, 2.2f);
#endif

    return color;
}
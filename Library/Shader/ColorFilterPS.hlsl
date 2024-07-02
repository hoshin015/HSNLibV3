#include "FullScreenQuad.hlsli"
#include "FilterFunctions.hlsli"


cbuffer COLOR_FILTER : register(_colorFilterConstant)
{
    float hueShift;
    float saturation;
    float brightness;
    float colorFilterPad;
}


SamplerState samplerStates[_samplerNum] : register(s0);

Texture2D colorFilterTexture : register(_colorFilterTexture);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = colorFilterTexture.Sample(samplerStates[_linearSampler], pin.texcoord);
    
    // RGB > HSV �ɕϊ�
    color.rgb = RGB2HSV(color.rgb);
    
    // �F������
    color.r += hueShift;
    
    // �ʓx����
    color.g *= saturation;
    
    // ���x����
    color.b *= brightness;
    
    // HSV > RGB �ɕϊ�
    color.rgb = HSV2RGB(color.rgb);
    
    return color;
}
#include "FullScreenQuad.hlsli"
#include "FilterFunctions.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

SamplerState samplerStates[3] : register(s0);
Texture2D textureMaps[4] : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = textureMaps[0].Sample(samplerStates[LINEAR], pin.texcoord);
    
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
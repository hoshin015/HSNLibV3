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
    
    // RGB > HSV ‚É•ÏŠ·
    color.rgb = RGB2HSV(color.rgb);
    
    // F‘Š’²®
    color.r += hueShift;
    
    // Ê“x’²®
    color.g *= saturation;
    
    // –¾“x’²®
    color.b *= brightness;
    
    // HSV > RGB ‚É•ÏŠ·
    color.rgb = HSV2RGB(color.rgb);
    
    return color;
}
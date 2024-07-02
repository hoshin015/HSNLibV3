#include "FullScreenQuad.hlsli"
#include "FilterFunctions.hlsli"


cbuffer COLOR_FILTER : register(_colorFilterConstant)
{
    float hueShift;
    float saturation;
    float brightness;
    float contrast;
}


SamplerState samplerStates[_samplerNum] : register(s0);

Texture2D colorFilterTexture : register(_colorFilterTexture);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = colorFilterTexture.Sample(samplerStates[_linearSampler], pin.texcoord);
    
    // RGB > HSV に変換
    color.rgb = RGB2HSV(color.rgb);
    
    // 色相調整
    color.r += hueShift;
    
    // 彩度調整
    color.g *= saturation;
    
    // 明度調整
    color.b *= brightness;
    
    // HSV > RGB に変換
    color.rgb = HSV2RGB(color.rgb);

    // コントラスト処理
    float midpoint = pow(0.5, 2.2);
    color.rgb = (color.rgb - midpoint) * contrast + midpoint;
    
    return color;
}
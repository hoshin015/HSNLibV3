#include "../RegisterNum.h"
#include "FullScreenQuad.hlsli"
#include "FilterFunctions.hlsli"

cbuffer LuminanceExtractionConstant : register(_luminanceExtractionConstant)
{
    float threshould;   // 高輝度抽出のための閾値
    float intensity;    // ブルームの強度
    float2 lumiPad;
}

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

SamplerState samplerStates[3] : register(s0);
Texture2D textureMaps    : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
    // 普通に描画されたテクスチャ
    float4 color = textureMaps.Sample(samplerStates[POINT], pin.texcoord);
    
    // RGB > 輝度値に変換
    float luminance = RGB2Luminance(color.rgb);
    clip(luminance - 0.01f);
    
    // 閾値との差を算出
    float contribution = max(0, luminance - threshould);
    
    // 出力する色を補正する
    contribution /= luminance;
    color.rgb *= contribution * intensity;
    
    color.rgb = pow(color.rgb, 2.2f);
    
    return color;
}
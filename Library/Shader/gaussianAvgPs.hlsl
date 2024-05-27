#include "../RegisterNum.h"
#include "FullScreenQuad.hlsli"
 

SamplerState samplerStates[_samplerNum] : register(s0);
Texture2D textureMaps[GAUSSIAN_DOWNSAMPLING_COUNT] : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
    // 普通に描画されたテクスチャ
    float4 combineColor = textureMaps[0].Sample(samplerStates[_pointSampler], pin.texcoord);
    
    for (int i = 0; i < GAUSSIAN_DOWNSAMPLING_COUNT; i++)
    {
        combineColor += textureMaps[i].Sample(samplerStates[_pointSampler], pin.texcoord);
    }
    combineColor /= GAUSSIAN_DOWNSAMPLING_COUNT;
    combineColor.a = 1.0f;
    
    return combineColor;
}
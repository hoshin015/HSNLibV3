#include "Sprite.hlsli"
#include "ShaderFunctions.hlsli"

SamplerState samplerStates[_samplerNum] : register(s0);

Texture2D spriteTexture : register(_spriteTexture);
Texture2D dissolveTexture : register(_dissolveTexture);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = spriteTexture.Sample(samplerStates[_linearSampler], pin.texcoord);
    float alpha = color.a;

    // ƒfƒBƒ]ƒ‹ƒuˆ—
    dissolve(color, dissolveThreshold, edgeThreshold, edgeColor, dissolveTexture, samplerStates[_anisotropicSampler], pin.texcoord);

    color.rgb += (addColor.rgb * addColor.a);

    return float4(color.rgb, alpha) * pin.color;
}
#include "Sprite.hlsli"

SamplerState pointSamplerState : register(s0);
SamplerState linearSamplerState : register(s1);
SamplerState anisotropicSamplerState : register(s2);

Texture2D spriteTexture : register(_spriteTexture);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = spriteTexture.Sample(linearSamplerState, pin.texcoord);
    float alpha = color.a;
    
    return float4(color.rgb, alpha) * pin.color;
}
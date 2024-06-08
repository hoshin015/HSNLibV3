#include "Particles.hlsli"
#include "../RegisterNum.h"

SamplerState samplerStates[_samplerNum] : register(s0);

Texture2D particleTexture : register(t9);

#if USE_WBOIT
struct PSOUT
{
    float4 accum : SV_TARGET0; // カラーバッファ
    float4 reveal : SV_TARGET1; // ウェイトバッファ
};
PSOUT main(GS_OUT pin)
#else
float4 main(GS_OUT pin) : SV_TARGET
#endif
{
    float4 textureColor = particleTexture.Sample(samplerStates[_pointSampler], pin.texcoord);
    float4 finalColor = textureColor * pin.color;
#if USE_WBOIT
    float4 color = finalColor;
    float weight = max(min(1.0, max(max(color.r, color.g), color.b) * color.a), color.a) * clamp(0.03 / (1e-5 + pow(pin.position.z / 200.0, 4.0)), 1e-2, 3e3);

	PSOUT psout;
    psout.accum = float4(color.rgb * color.a, color.a) * weight;
    psout.reveal.r = color.a;
    return psout;
#else
	return finalColor;
#endif
}
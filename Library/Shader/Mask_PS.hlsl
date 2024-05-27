#include "Mask.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

SamplerState samplerStates[3] : register(s0);
Texture2D texture0 : register(t0);

Texture2D maskTexture : register(t1);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = texture0.Sample(samplerStates[LINEAR], pin.texcoord) * pin.color;
    
    float mask = maskTexture.Sample(samplerStates[LINEAR], pin.texcoord).r;
    
    // step�֐���p����mask�̒l��dissolveThreshold�̒l���r���ē��ߒl��0 or 1�ɂ���
    float alpha = step(mask, dissolveThreshold);
    
    // ���̏���
    float edgeValue = step(mask - dissolveThreshold, dissolveThreshold) * step(dissolveThreshold, mask) * step(mask, dissolveThreshold + edgeThreshold);

    color.rgb += edgeColor.rgb * edgeValue;
    alpha = saturate(alpha + edgeValue);

	// color�̓��ߒl�ɏ�Z����
    color.a *= alpha;

	// �A���t�@��0�ȉ��Ȃ炻�������`�悵�Ȃ��悤�ɂ���
    clip(color.a - 0.01f);

    return color;
}
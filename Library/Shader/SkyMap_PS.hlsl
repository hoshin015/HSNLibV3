#include "SkyMap.hlsli"

SamplerState samplerStates[_samplerNum] : register(s0);
TextureCube skymap : register(_spriteTexture);

float4 main(VS_OUT pin) : SV_TARGET
{
    // �����x�N�g��
    float3 E = normalize(pin.worldPosition.xyz - cameraPosition.xyz);

    // mipmap���g���ƕ����̂�mipmap�Ȃ��ɂȂ�䂨���ɂ��Ă���
    return skymap.SampleLevel(samplerStates[_linearSampler], E, 0) * pin.color;
}
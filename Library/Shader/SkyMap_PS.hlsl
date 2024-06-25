#include "SkyMap.hlsli"

SamplerState samplerStates[_samplerNum] : register(s0);
TextureCube skymap : register(_spriteTexture);

float4 main(VS_OUT pin) : SV_TARGET
{
    // 視線ベクトル
    float3 E = normalize(pin.worldPosition.xyz - cameraPosition.xyz);

    // mipmapを使うと崩れるのでmipmapなしになるゆおうにしておく
    return skymap.SampleLevel(samplerStates[_linearSampler], E, 0) * pin.color;
}
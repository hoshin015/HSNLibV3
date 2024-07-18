#include "FullScreenQuad.hlsli"

SamplerState samplerStates[_samplerNum]: register(s0);

Texture2D videoTexture: register(t0);

float4 main(VS_OUT pin) : SV_TARGET {
    return videoTexture.Sample(samplerStates[_linearSampler], pin.texcoord);

}

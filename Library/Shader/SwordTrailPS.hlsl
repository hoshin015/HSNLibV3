#include "FullScreenQuad.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

SamplerState samplerStates[3] : register(s0);
Texture2D textureMap : register(t0);
Texture2D swordTrailMap : register(t1);

float4 main(VS_OUT pin) : SV_TARGET
{
    float2 uv = pin.texcoord;


    float4 swordTrail = swordTrailMap.Sample(samplerStates[POINT], pin.texcoord);
    uv.x -= swordTrail.x * 0.03f;
    uv.y -= swordTrail.y * 0.03f;
    //return swordTrail;

    float4 color = textureMap.Sample(samplerStates[POINT], uv);

    //if(swordTrail.a != 0)
	//	color.b += 0.1f;

    return color;
}
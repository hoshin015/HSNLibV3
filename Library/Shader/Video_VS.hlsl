#include "FullScreenQuad.hlsli"

struct VS_IN {
    float3 pos: POSITION;
    float2 texcoord: TEXCOORD;
};

VS_OUT main(VS_IN vin) {
    // const float2 pos[4] = {
    //     { -0.7f, 0.7f },
    //     { 0.7f, 0.7f },
    //     { -0.7f, -0.7f },
    //     { 0.7f, -0.7f }
    // };
    // const float2 tex[4] = {
    //     { 0, 0 },
    //     { 1, 0 },
    //     { 0, 1 },
    //     { 1, 1 }
    // };

    VS_OUT vOut;
    vOut.position = float4(vin.pos,1);
    vOut.texcoord = vin.texcoord;

    return vOut;
}
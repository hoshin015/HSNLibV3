#include "FullScreenQuad.hlsli"

VS_OUT main(in uint vID: SV_VERTEXID) {
    const float2 pos[4] = {
        { -0.5f, 0.5f },
        { 0.5f, 0.5f },
        { -0.5f, -0.5f },
        { 0.5f, -0.5f }
    };
    const float2 tex[4] = {
        { 0, 0 },
        { 1, 0 },
        { 0, 1 },
        { 1, 1 }
    };

    VS_OUT vOut;
    vOut.position = float4(pos[vID],0,1);
    vOut.texcoord = tex[vID];

    return vOut;
}
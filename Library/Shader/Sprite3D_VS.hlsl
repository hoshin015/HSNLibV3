#include "Sprite3D.hlsli"

VS_OUT main(float3 poisition : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD)
{
    VS_OUT vout;
    // ローカル座標からワールド座標に変換しその後、スクリーン座標に変換
    vout.position = mul(float4(poisition.xyz, 1), viewProjection);
    vout.color = color;
    vout.texcoord = texcoord;
    return vout;
}
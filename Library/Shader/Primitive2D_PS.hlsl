#include "Primitive2D.hlsli"

float4 main(VS_OUT pin) : SV_TARGET
{
    return pin.color;
}
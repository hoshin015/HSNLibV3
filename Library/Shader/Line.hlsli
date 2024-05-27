#include "../RegisterNum.h"

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

cbuffer SCENE_CONSTANT_BUFFER : register(_cameraConstant)
{
    row_major float4x4 viewProjection;
    float4 lightDirection;
};
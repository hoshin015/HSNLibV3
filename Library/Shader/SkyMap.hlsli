#include "../RegisterNum.h"

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float4 worldPosition : WORLD_POSITION;
};


// ƒJƒƒ‰‚Ìî•ñ
cbuffer CameraConstant : register(_cameraConstant)
{
    row_major float4x4 viewProjection;
    float4 cameraPosition;
    row_major float4x4 inverseViewProjection;
}
#include "../RegisterNum.h"

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

// Dissolve
cbuffer DissolveConstant : register(_dissolveConstant)
{
    float dissolveThreshold; // �f�B�]���u��
    float edgeThreshold; // �΂�臒l
    float2 dissolvePad;
    float4 edgeColor; // �΂̐F
};

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
    float dissolveThreshold; // ディゾルブ量
    float edgeThreshold; // 緑の閾値
    float2 dissolvePad;
    float4 edgeColor; // 緑の色
};

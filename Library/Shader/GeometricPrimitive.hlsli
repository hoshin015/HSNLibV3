#include "../RegisterNum.h"


struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

// カメラの情報
cbuffer CameraConstant : register(_cameraConstant)
{
    row_major float4x4 viewProjection;
    float4 cameraPosition;
}

// 3d 空間に出す全てのモデルが持つ
cbuffer ObjectConstant : register(_objectConstant)
{
    row_major float4x4 world;
    float4 materialColor;
}
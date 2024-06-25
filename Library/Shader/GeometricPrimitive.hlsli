#include "../RegisterNum.h"


struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

// �J�����̏��
cbuffer CameraConstant : register(_cameraConstant)
{
    row_major float4x4 viewProjection;
    float4 cameraPosition;
    row_major float4x4 inverseViewProjection;
}

// 3d ��Ԃɏo���S�Ẵ��f��������
cbuffer ObjectConstant : register(_objectConstant)
{
    row_major float4x4 world;
    float4 materialColor;
}
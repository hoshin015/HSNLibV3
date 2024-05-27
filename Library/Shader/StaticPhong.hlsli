#include "../RegisterNum.h"
#include "light.hlsli"

struct VS_IN
{
    float4  position : POSITION;
    float4  normal : NORMAL;
    float4  tangent : TANGENT;
    float2  texcoord : TEXCOORD;
    float4  boneWeights : WEIGHTS;
    uint4   boneIndices : BONES;
    uint    instanceId : SV_INSTANCEID;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION;
    float4 worldNormal : NORMAL;
    float4 worldTangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

// BONE の最大数
static const int MAX_BONES = 256;

// カメラの情報
cbuffer CameraConstant : register(_cameraConstant)
{
    row_major float4x4 viewProjection;
    float4 cameraPosition;
}

// 3d 空間に出す全てのモデルが持つ
cbuffer ObjectConstant : register(_objectConstant)
{
    float4 materialColorKd;
    float4 materialColorKs;
    float4 materialColorKa;
    uint instanceCount;
    uint3 pad;
    row_major float4x4 worldTransforms[MAX_INSTANCE];
}

// ライトの情報
cbuffer LightConstant : register(_lightConstant)
{
    DirectionalLightData directionalLightData;
    PointLightData pointLightData[PointLightMax];
    SpotLightData spotLightData[SpotLightMax];
    int pointLightCount;
    int spotLightCount;
    float2 pad1;
    float4 ambientLightColor;
}
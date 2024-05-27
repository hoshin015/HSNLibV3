#include "../RegisterNum.h"
#include "ShadowmapFunctions.hlsli"

struct VS_IN
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 boneWeights : WEIGHTS;
    uint4 boneIndices : BONES;
};

// BONE �̍ő吔
static const int MAX_BONES = 256;

// 3d ��Ԃɏo���S�Ẵ��f��������
cbuffer ObjectConstant : register(_objectConstant)
{
    row_major float4x4 world;
    float4 materialColorKd;
    float4 materialColorKs;
    float4 materialColorKa;
    row_major float4x4 boneTransforms[MAX_BONES];
}

// �e�̏��
cbuffer ShadowConstant : register(_shadowConstant)
{
    row_major float4x4 lightViewProjection;
}
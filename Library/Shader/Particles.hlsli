#include "../RegisterNum.h"

struct VS_OUT
{
    uint vertexId : VERTEXID;
};

struct GS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

struct Particle
{
    float4 color;
    float3 position;
    float3 velocity;
    float scale;
    float depth;
    float lifeTime; 
    bool isActive; 
    float kind;
};

// タイマーの情報
cbuffer TimerConstant : register(_timerConstant)
{
    float   deltaTime;
    float3  pad;
};

// カメラの情報
cbuffer CameraConstant : register(_cameraConstant)
{
    row_major float4x4 viewProjection;
    float4 cameraPosition;
}

// エミッター, パーティクル情報
cbuffer EmitterConstant : register(_emitterConstant)
{
    float   particleSize;
    float   particleLifeTime;
    float   particleKind;
    float   emitPad1;
    float4  particleColor;
    float3  emitterPosition;
    float   emitPad2;
};
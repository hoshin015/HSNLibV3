#include "../RegisterNum.h"
#include "FullScreenQuad.hlsli"

cbuffer HeatHazeConstant : register(_heatHazeConstant)
{
    float shiftSpeed;
    float maxShift;
    float2 heatHazePad;
}

// タイマーの情報
cbuffer TimerConstant : register(_timerConstant)
{
    float deltaTime;
    float nowTime;
    float2 timePad;
};

float random(float2 pos)
{
    return frac(sin(dot(pos,float2(12.9898, 78.233))) * 43758.5453);
}

float noise(float2 pos)
{
    return random(floor(pos));
}

float valueNoise(float2 pos)
{
    float2 p = floor(pos);
    float2 f = frac(pos);

    float v00 = noise(p + float2(0.0, 0.0));
    float v10 = noise(p + float2(1.0, 0.0));
    float v01 = noise(p + float2(0.0, 1.0));
    float v11 = noise(p + float2(1.0, 1.0));

    float2 u = f * f * (3.0 - 2.0 * f);

    return lerp(lerp(v00, v10, u.x), lerp(v01, v11, u.x), u.y);
}


SamplerState samplerStates[_samplerNum] : register(s0);

Texture2D textureMaps : register(_heatHazeTexture);

float4 main(VS_OUT pin) : SV_TARGET
{
    float2 SCREEN_UV = pin.texcoord;
    float2 uv_r = SCREEN_UV + float2(nowTime * shiftSpeed, 0.0);
    float2 uv_g = SCREEN_UV + float2(-10.0, -nowTime * shiftSpeed);
    float2 shift = float2(
		sin(
			  valueNoise(uv_r * 8.0) * 0.2
			+ valueNoise(uv_r * 16.0) * 0.2
			+ valueNoise(uv_r * 32.0) * 0.2
			+ valueNoise(uv_r * 64.0) * 0.2
			+ valueNoise(uv_r * 128.0) * 0.2
		) - 0.5
	, sin(
		  valueNoise(uv_g * 8.0) * 0.2
		+ valueNoise(uv_g * 16.0) * 0.2
		+ valueNoise(uv_g * 32.0) * 0.2
		+ valueNoise(uv_g * 64.0) * 0.2
		+ valueNoise(uv_g * 128.0) * 0.2
		) - 0.5
	) * (maxShift);

    float4 color = float4(textureMaps.Sample(samplerStates[_pointSampler], SCREEN_UV + shift).rgb, 1.0f);

    return color;
}

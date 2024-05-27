#include "ShadowMapCasterStatic.hlsli"


float4 main(VS_IN vin) : SV_POSITION
{ 
    float3 p = vin.position;
    p = mul(float4(p, 1.0f), worldTransforms[vin.instanceId]).xyz;
    
    return mul(float4(p, 1.0f), lightViewProjection);
}
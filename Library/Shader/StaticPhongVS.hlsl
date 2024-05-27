#include "StaticPhong.hlsli"

VS_OUT main(VS_IN vin)
{
    float sigma = vin.tangent.w;
    
    float3 p = vin.position;
    float3 n = vin.normal;
    float3 t = vin.tangent;
    p = mul(float4(p, 1.0f), worldTransforms[vin.instanceId]).xyz;
    n = mul(float4(n, 0.0f), worldTransforms[vin.instanceId]).xyz;
    t = mul(float4(t, 0.0f), worldTransforms[vin.instanceId]).xyz;
    
    VS_OUT vout;
    vout.position = mul(float4(p, 1.0f), viewProjection);
    
    
    vout.worldPosition = mul(vin.position, worldTransforms[vin.instanceId]);
    vout.worldNormal = normalize(float4(n, 0.0f));
    vout.worldTangent = normalize(float4(t, 0.0f));
    vout.worldTangent.w = sigma;
    
    vout.texcoord = vin.texcoord;
    vout.color = materialColorKd;
    
    return vout;
}
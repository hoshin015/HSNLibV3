#include "Phong.hlsli"

#define UNITY_PI 3.14159265358979
#define UNITY_INV_PI 0.3183098861837907 // This is approximately 1 / ƒÎ
#define _DielectricF0 0.04

SamplerState samplerStates[_samplerNum] : register(s0);

Texture2D albedoTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D metallicRoughnessTexture : register(t2);
Texture2D emissiveTexture : register(t3);

Texture2D shadowTexture[SHADOWMAP_COUNT] : register(t4);

inline half Fd_Burley(half ndotv, half ndotl, half ldoth, half roughness)
{
    half fd90 = 0.5f + 2 * ldoth * ldoth * roughness;
    half lightScatter = (1 + (fd90 - 1) * pow(1 - ndotl, 5));
    half viewScatter = (1 + (fd90 - 1) * pow(1 - ndotv, 5));

    half diffuse = lightScatter * viewScatter;

    // diffuse /= UNITY_PI;
    return diffuse;
}

inline float V_SmithGGXCorrelated(float ndotl, float ndotv, float alpha)
{
    float lambdaV = ndotl * (ndotv * (1 - alpha) + alpha);
    float lambdaL = ndotv * (ndotl * (1 - alpha) + alpha);
    return 0.5f / (lambdaV + lambdaL + 0.0001);
}

inline half D_GGX(half perceptualRoughness, half ndoth, half3 normal, half3 halfDir)
{
    half3 ncrossh = cross(normal, halfDir);
    half a = ndoth * perceptualRoughness;
    half k = perceptualRoughness / (dot(ncrossh, ncrossh) + a * a);
    half d = k * k * UNITY_INV_PI;
    return min(d, 65504.0);
}

inline half3 F_Schlick(half3 f0, half cos)
{
    return f0 + (1 - f0) * pow(1 - cos, 5);
}

float4 BRDF(float4 albedo, float metallic, float perceptualRoughness, float3 normal, float3 viewDir, float3 lightDir, float3 lightColor, float3 indirectDiffuse, float3 indirectSpeculr)
{
    float3 halfDir = normalize(lightDir + viewDir);
    float ndotv = abs(dot(normal, viewDir));
    float ndotl = max(0, dot(normal, lightDir));
    float ndoth = max(0, dot(normal, halfDir));
    float ldoth = max(0, dot(lightDir, halfDir));
    float reflectivity = lerp(_DielectricF0, 1, metallic);
    float3 f0 = lerp(_DielectricF0, albedo.rgb, metallic);

    float diffuseTerm = Fd_Burley(ndotv, ndotl, ldoth, perceptualRoughness) * ndotl;
    float3 diffuse = albedo.rgb * (1 - reflectivity) * lightColor * diffuseTerm;

    diffuse += albedo.rgb * (1 - reflectivity) * indirectDiffuse.rgb;

    float alpha = perceptualRoughness * perceptualRoughness;
    float V = V_SmithGGXCorrelated(ndotl, ndotv, alpha);
    float D = D_GGX(perceptualRoughness, ndotv, normal, halfDir);
    float3 F = F_Schlick(f0, ldoth);
    float3 specular = V * D * F * ndotl * lightColor;
    specular *= UNITY_PI;
    specular = max(0, specular);

    float surfaceReduction = 1.0 / (alpha * alpha + 1.0);
    float f90 = saturate((1 - perceptualRoughness) + reflectivity);
    specular += surfaceReduction * indirectSpeculr * lerp(f0, f90, pow(1 - ndotv, 5));

    float3 color = diffuse.rgb + specular;
    return float4(color, albedo.a);

}


float4 main(VS_OUT pin) : SV_TARGET
{
    half4 albedoColor = albedoTexture.Sample(samplerStates[_pointSampler], pin.texcoord);
    float4 normalColor = normalTexture.Sample(samplerStates[_pointSampler], pin.texcoord);
    float4 metallicRoughnessColor = metallicRoughnessTexture.Sample(samplerStates[_pointSampler], pin.texcoord);
    float4 emissiveColor = emissiveTexture.Sample(samplerStates[_pointSampler], pin.texcoord);
    
    float4 emissive = emissiveColor;
    float metallic = saturate(metallicRoughnessColor.r * 0.5f);
    float roughness = saturate((1.0 - metallicRoughnessColor.a) * 0.5f);

    float3 N = normalize(pin.worldNormal.xyz);
    float3 T = normalize(pin.worldTangent.xyz);
    float sigma = pin.worldTangent.w;
    T = normalize(T - N * dot(N, T));
    float3 B = normalize(cross(N, T) * sigma);

    float3 normalFactor = normalColor.xyz;
    normalFactor = (normalFactor * 2.0) - 1.0f;
    N = normalize((normalFactor.x * T) + (normalFactor.y * B) + (normalFactor.z * N));


    float3 L = normalize(directionalLightData.direction);
    float3 E = normalize(cameraPosition.xyz - pin.worldPosition.xyz);

    half4 c = BRDF(albedoColor, metallic, roughness, N, E, -L, directionalLightData.color, albedoColor, 1);

    c = saturate(c);

    //c += emissive;

    return float4(c.rgb, albedoColor.a);
}

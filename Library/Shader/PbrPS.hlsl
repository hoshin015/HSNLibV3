#include "Phong.hlsli"
#include "BidirectionalReflectanceDistributionFunction.hlsli"

SamplerState samplerStates[_samplerNum] : register(s0);

Texture2D diffuseTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D metallicRoughnessTexture : register(t2);
Texture2D emissiveTexture : register(t3);

Texture2D shadowTexture[SHADOWMAP_COUNT] : register(t4);

float4 main(VS_OUT pin) : SV_TARGET
{
    const float GAMMA = 2.2f;

    float4 albedoColor = diffuseTexture.Sample(samplerStates[_pointSampler], pin.texcoord);
    albedoColor.rgb = pow(albedoColor.rgb, GAMMA);
    float4 normalColor = normalTexture.Sample(samplerStates[_pointSampler], pin.texcoord);
    float4 metallicRoughnessColor = metallicRoughnessTexture.Sample(samplerStates[_pointSampler], pin.texcoord);
    float4 emissiveColor = emissiveTexture.Sample(samplerStates[_pointSampler], pin.texcoord);
    emissiveColor.rgb = pow(emissiveColor.rgb, GAMMA);

    float4 baseColorFactor = albedoColor;
    float4 emissiveFactor = emissiveColor;
    float roughnessFactor = 1.0 - metallicRoughnessColor.a;
    float metallicFactor = metallicRoughnessColor.r;

    const float3 f0 = lerp(0.04, baseColorFactor.rgb, metallicFactor);
    const float3 f90 = float3(1.0, 1.0, 1.0);
    const float alphaRoughness = roughnessFactor * roughnessFactor;
    const float3 cDiff = lerp(baseColorFactor.rgb, float3(0.0, 0.0, 0.0), metallicFactor);

    const float3 P = pin.worldPosition.xyz;
    const float3 V = normalize(cameraPosition.xyz - pin.worldPosition.xyz);

    float3 N = normalize(pin.worldNormal.xyz);
    float3 T = normalize(pin.worldTangent.xyz);
    float sigma = pin.worldTangent.w;
    T = normalize(T - N * dot(N, T));
    float3 B = normalize(cross(N, T) * sigma);

    float3 normalFactor = normalColor.xyz;
    normalFactor = (normalFactor * 2.0) - 1.0f;
    N = normalize((normalFactor.x * T) + (normalFactor.y * B) + (normalFactor.z * N));

    float3 diffuse = float3(0.0, 0.0, 0.0);
    float3 specular = float3(0.0, 0.0, 0.0);

    float3 L = normalize(-directionalLightData.direction.xyz);
    float3 Li = float3(1.0, 1.0, 1.0);
    const float NoL = max(0.0, dot(N, L));
    const float NoV = max(0.0, dot(N, V));

    if (NoL > 0.0 || NoV > 0.0)
    {
        const float3 R = reflect(-L, N);
        const float3 H = normalize(V + L);

        const float NoH = max(0.0, dot(N, H));
        const float HoV = max(0.0, dot(H, V));

        diffuse += Li * NoL * brdfLambertion(f0, f90, cDiff, HoV);
        specular += Li * NoL * brdfSpecularGgx(f0, f90, alphaRoughness, HoV, NoL, NoV, NoH);
    }

    float3 emissive = emissiveFactor.rgb * 1.2;

    float3 Lo = diffuse + specular + emissive;
    return float4(Lo, baseColorFactor.a);
}

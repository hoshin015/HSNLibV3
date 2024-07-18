#include "Phong.hlsli"
#include "PbrFunctions.hlsli"

SamplerState samplerStates[_samplerNum] : register(s0);

// PBR
Texture2D baseTexture : register(_baseTexture);
Texture2D normalTexture : register(_normalTexture);
Texture2D metallicRoughnessTexture : register(_metallicRoughnessTexture);
Texture2D emissiveTexture : register(_emissiveTexture);
Texture2D occlusionTexture : register(_occlusionTexture);

// 影
Texture2D shadowTexture[SHADOWMAP_COUNT] : register(_shadowTexture);

// IBL
TextureCube diffuseIem : register(_IEM);
TextureCube specularPmrem : register(_PMREM);
Texture2D lutGgx : register(_LutGgx);

struct PSOUT
{
    float4 accum : SV_TARGET0; // カラーバッファ
    float4 reveal : SV_TARGET1; // ウェイトバッファ
};
PSOUT main(VS_OUT pin)
{
	// --- 法線/従法線/接線 ---
    float4 normal = normalTexture.Sample(samplerStates[_anisotropicSampler], pin.texcoord);
    normal = (normal * 2.0) - 1.0;
    float3 n = normalize(pin.worldNormal.xyz);
    float3 t = normalize(pin.worldTangent.xyz);
    float sigma = pin.worldTangent.w;
    float3 b = normalize(cross(n, t) * sigma);
    float3 N = normalize((normal.x * t) + (normal.y * b) + (normal.z * n));
    
	// 色生成(エミッシブもここで追加)
    float4 finalColor = float4(0,0,0,0);

    float3 Ks = float3(1, 1, 1);
    float3 ToCamera = normalize(cameraPosition.xyz - pin.worldPosition.xyz); // 光が入射したサーフェースから視点に向かって伸びるベクトルを求めている
    float3 spec = CalcPhongSpecular(N, ToCamera, directionalLightData.color.rgb, ToCamera, 1.0f, Ks);
    float ePower = smoothstep(0.1, 0.9, spec.z);

    finalColor.rgb = float3(0.5, 0.5, 2) * (ePower * 1.0);


	finalColor.rgb *= pin.color.rgb;
	//finalColor.rgb = pow(finalColor.rgb, 1.0f / GammaFactor);
    finalColor = float4(finalColor.rgb, ePower * pin.color.a);

    // ------------ wboit ------------
    float4 color = finalColor;
    float weight = max(min(1.0, max(max(color.r, color.g), color.b) * color.a), color.a) * clamp(0.03 / (1e-5 + pow(pin.position.z / 1000.0, 4.0)), 1e-2, 3e3);

    PSOUT psout;
    psout.accum = float4(color.rgb * color.a, color.a) * weight;
    psout.reveal.r = color.a;

    return psout;
}

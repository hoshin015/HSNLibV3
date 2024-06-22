#include "Phong.hlsli"
#include "PbrFunctions.hlsli"

SamplerState samplerStates[_samplerNum] : register(s0);

Texture2D baseTexture : register(_baseTexture);
Texture2D normalTexture : register(_normalTexture);
Texture2D metallicRoughnessTexture : register(_metallicRoughnessTexture);
Texture2D emissiveTexture : register(_emissiveTexture);
Texture2D occlusionTexture : register(_occlusionTexture);

Texture2D shadowTexture[SHADOWMAP_COUNT] : register(_shadowTexture);

float4 main(VS_OUT pin) : SV_TARGET
{
    //	ガンマ係数
    static const float GammaFactor = 2.2f;

    // ベースカラー
    float4 baseColor = baseTexture.Sample(samplerStates[_pointSampler], pin.texcoord);
    baseColor.rgb = pow(baseColor.rgb, GammaFactor);

    // エミッシブ
    float4 emissiveColor = emissiveTexture.Sample(samplerStates[_pointSampler], pin.texcoord);
    emissiveColor.rgb = pow(emissiveColor.rgb, GammaFactor);


    // 法線/従法線/接線
    float4 normal = normalTexture.Sample(samplerStates[_anisotropicSampler], pin.texcoord);
    normal = (normal * 2.0) - 1.0;
    float3 n = normalize(pin.worldNormal.xyz);
    float3 t = normalize(pin.worldTangent.xyz);
    float sigma = pin.worldTangent.w;
    float3 b = normalize(cross(n, t) * sigma);
    float3 N = normalize((normal.x * t) + (normal.y * b) + (normal.z * n));

    // 金属質/粗さ
    float4 metallicRoughnessColor = metallicRoughnessTexture.Sample(samplerStates[_pointSampler], pin.texcoord);
    float roughness = 1.0f - metallicRoughnessColor.a;
    float metalness = metallicRoughnessColor.r;

#if 01	//	確認用のコードなので本来は不要
    //roughness = saturate(roughness + adjust_roughness);
    //metalness = saturate(metalness + adjust_metalness);
#endif


    // 光の遮蔽率
    float4 occlusionColor = occlusionTexture.Sample(samplerStates[_anisotropicSampler], pin.texcoord);
    float occlusionFactor = occlusionColor.r;
    const float occlusionStrength = 0.0f;


    // アルベド(非金属部分)
    float4 albedo = baseColor;

    // 入射光のうち拡散反射になる割合
    float3 diffuseReflectance = lerp(albedo.rgb, 0.0f, metalness);

    // 垂直反射時のフレネル反射率(非金属でも最低4%は鏡面反射する)
    float3 F0 = lerp(0.04f, albedo.rgb, metalness);

    // 視線ベクトル
    float3 V = normalize(pin.worldPosition.xyz - cameraPosition.xyz);

    // 直接光のシェーディング
    float3 totalDiffuse = (float3) 0;
    float3 totalSpecular = (float3) 0;
    {
		// 平行光源の処理
        float3 diffuse = (float3) 0;
        float3 specular = (float3) 0;

        float3 L = normalize(directionalLightData.direction.xyz);
        DirectBDRF(diffuseReflectance, F0, N, V, L, directionalLightData.color.rgb, roughness, diffuse, specular);
        totalDiffuse += diffuse;
        totalSpecular += specular;
    }

    // 遮蔽処理
    totalDiffuse = lerp(totalDiffuse, totalDiffuse * occlusionFactor, occlusionStrength);
    totalSpecular = lerp(totalSpecular, totalSpecular * occlusionFactor, occlusionStrength);

    // 色生成(エミッシブもここで追加)
    float3 finalColor = totalDiffuse + totalSpecular + emissiveColor;
    finalColor = pow(finalColor, 1.0f / GammaFactor);
    return float4(finalColor, baseColor.a);
}

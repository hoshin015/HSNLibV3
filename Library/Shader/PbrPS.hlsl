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
Texture2D   lutGgx : register(_LutGgx);

float4 main(VS_OUT pin) : SV_TARGET
{
	// ====== テクスチャ情報取得 ======

	// --- ベースカラー ---
	float4 albedoColor = baseTexture.Sample(samplerStates[_pointSampler], pin.texcoord);
	albedoColor.rgb    = pow(albedoColor.rgb, GammaFactor);

	// --- エミッシブ ---
	float4 emissiveColor = emissiveTexture.Sample(samplerStates[_pointSampler], pin.texcoord);
	emissiveColor.rgb    = pow(emissiveColor.rgb, GammaFactor);


	// --- 法線/従法線/接線 ---
	float4 normal = normalTexture.Sample(samplerStates[_anisotropicSampler], pin.texcoord);
	normal        = (normal * 2.0) - 1.0;
	float3 n      = normalize(pin.worldNormal.xyz);
	float3 t      = normalize(pin.worldTangent.xyz);
	float  sigma  = pin.worldTangent.w;
	float3 b      = normalize(cross(n, t) * sigma);
	float3 N      = normalize((normal.x * t) + (normal.y * b) + (normal.z * n));

	// --- 金属質/粗さ ---
	float4 metallicRoughnessColor = metallicRoughnessTexture.Sample(samplerStates[_pointSampler], pin.texcoord);
	float  roughness              = metallicRoughnessColor.a;
	float  metalness              = metallicRoughnessColor.r;

	// オブジェクトパラメータによる調整
#if 0
	roughness = clamp(roughness + roughnessPower, 0.0, 1.0);
    metalness = clamp(metalness + metalnessPower, 0.0, 1.0);
#endif
    emissiveColor *= emissivePower;

    //roughness = 1.0f;
    //metalness = 0.0f;

	// --- 光の遮蔽率 ---
	float4      occlusionColor    = occlusionTexture.Sample(samplerStates[_anisotropicSampler], pin.texcoord);
	float       occlusionFactor   = occlusionColor.r;
	const float occlusionStrength = 1.0f;


	// ====== PBR IBL 計算 ======

	// 入射光のうち拡散反射になる割合(metalness で反射率が決まる)
	float3 diffuseReflectance = lerp(albedoColor.rgb, 0.0f, metalness);

	// 垂直反射時のフレネル反射率(どんな物質でも最低4%は鏡面反射する)
	float3 F0 = lerp(0.04f, albedoColor.rgb, metalness);

	// 視線ベクトル
	float3 E = normalize(pin.worldPosition.xyz - cameraPosition.xyz);

	// ------ ディレクションライト ------
	float3 directionDiffuse  = (float3)0;
	float3 directionSpecular = (float3)0;
	{
		float3 L = normalize(directionalLightData.direction.xyz);
		DirectBDRF(diffuseReflectance, F0, N, E, L, directionalLightData.color.rgb, roughness, directionDiffuse,
		           directionSpecular);

		// 平行光源の影なので、平行光源に対して影を適応
		float3 shadow = CalcCascadedShadowColorPCFFilter(shadowTexture, samplerStates[_shadowSampler],
		                                                 pin.shadowTexcoord,
		                                                 shadowColor, shadowBias);
		directionDiffuse *= shadow;
		directionSpecular *= shadow;
	}

	// 遮蔽処理
	directionDiffuse  = lerp(directionDiffuse, directionDiffuse * occlusionFactor, occlusionStrength);
	directionSpecular = lerp(directionSpecular, directionSpecular * occlusionFactor, occlusionStrength);

	// IBL 処理
	directionDiffuse += DiffuseIBL(N, E, roughness, diffuseReflectance, F0, diffuseIem, samplerStates[_linearSampler]);
	directionSpecular += SpecularIBL(N, E, roughness, F0, lutGgx, specularPmrem, samplerStates[_linearSampler]);

	// 色生成(エミッシブもここで追加)
    float3 finalColor = directionDiffuse + directionSpecular + emissiveColor;
    finalColor *= pin.color.rgb;
	finalColor        = pow(finalColor, 1.0f / GammaFactor);
	return float4(finalColor, albedoColor.a);
}

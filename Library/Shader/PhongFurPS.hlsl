#include "Phong.hlsli"

SamplerState samplerStates[_samplerNum] : register(s0);

Texture2D diffuseTexture : register(_deffuseTexture);
Texture2D normalTexture : register(_normalTexture);
Texture2D specularTexture : register(_specularTexture);
Texture2D emissiveTexture : register(_emissiveTexture);
Texture2D occlusionTexture : register(_occlusionTexture);
Texture2D furTexture : register(_furTexture);

Texture2D shadowTexture[SHADOWMAP_COUNT] : register(_shadowTexture);

float4 main(VS_OUT pin) : SV_TARGET
{
	// ディフューズマップの取得
    float4 diffuseColor = diffuseTexture.Sample(samplerStates[_anisotropicSampler], pin.texcoord);
#if USE_LinearWorkflow
    diffuseColor.rgb = pow(diffuseColor.rgb, 2.2f);
#endif

    uint mipLevel = 0, width, height, numberOfLevels;
    furTexture.GetDimensions(mipLevel, width, height, numberOfLevels);
    const float aspectRatio = width / height;

    const float offset[3] = { 0.0, 1.3846153846, 3.2307692308 };
    const float weight[3] = { 0.2270270270, 0.3162162162, 0.0702702703 };

    // furTexture による uv 座標のずらし
    float4 furColor = furTexture.Sample(samplerStates[_pointSampler], pin.texcoord);
    
	// ループ変数の最大値を設定
    int maxSamplingCount = 3;
    
    int samplingCountU = int(furColor.r * maxSamplingCount);
    samplingCountU = min(samplingCountU, maxSamplingCount);

    float4 sampledColor = diffuseTexture.Sample(samplerStates[_linearBorderBlackSampler], pin.texcoord) * weight[0];
    for (int u = 1; u < samplingCountU; u++)
    {
        sampledColor += diffuseTexture.Sample(samplerStates[_linearBorderBlackSampler], pin.texcoord + float2(offset[u] / width, 0.0)) * weight[u];
        sampledColor += diffuseTexture.Sample(samplerStates[_linearBorderBlackSampler], pin.texcoord - float2(offset[u] / width, 0.0)) * weight[u];
    }

    int samplingCountV = int(furColor.g * maxSamplingCount);
    samplingCountV = min(samplingCountV, maxSamplingCount);
    for (int v = 1; v < samplingCountV; v++)
    {
        sampledColor += diffuseTexture.Sample(samplerStates[_linearBorderBlackSampler], pin.texcoord + float2(0.0, offset[v] / height)) * weight[v];
        sampledColor += diffuseTexture.Sample(samplerStates[_linearBorderBlackSampler], pin.texcoord - float2(0.0, offset[v] / height)) * weight[v];
    }
    if (samplingCountU + samplingCountV > 0)
    {
        sampledColor /= (2 * (samplingCountU + samplingCountV));
    }

    diffuseColor.rgb = sampledColor.rgb;

    

	// ノーマルマップの取得
	float4 normal = normalTexture.Sample(samplerStates[_anisotropicSampler], pin.texcoord);
    normal = (normal * 2.0) - 1.0;
    float3 n = normalize(pin.worldNormal.xyz);
    float3 t = normalize(pin.worldTangent.xyz);
    float sigma = pin.worldTangent.w;
    float3 b = normalize(cross(n, t) * sigma);
    float3 N = normalize((normal.x * t) + (normal.y * b) + (normal.z * n));

	// スペキュラマップの取得
    float specularPower = specularTexture.Sample(samplerStates[_anisotropicSampler], pin.texcoord);

	// エミッシブマップの取得
    float4 emissive = emissiveTexture.Sample(samplerStates[_anisotropicSampler], pin.texcoord);

	// 表面材質の性質
    float3 Kd = float3(0.8, 0.8, 0.8);
    float3 Ks = float3(1, 1, 1);
    float3 Ka = float3(1, 1, 1);


	// ------ ディレクションライト ------
    float3 L = normalize(directionalLightData.direction.xyz);

	// 拡散反射光
    float3 directionDiffuse = CalcLambertDiffse(N, L, directionalLightData.color.rgb, Kd);

	// 鏡面反射光
    float3 R = reflect(L, N); // 反射ベクトルを求めている
    float3 ToCamera = normalize(cameraPosition.xyz - pin.worldPosition.xyz); // 光が入射したサーフェースから視点に向かって伸びるベクトルを求めている
    float3 directionSpecular = CalcPhongSpecular(N, L, directionalLightData.color.rgb, ToCamera, 128.0f, Ks);

	// 平行光源の影なので、平行光源に対して影を適応
    float3 shadow = CalcCascadedShadowColorPCFFilter(shadowTexture, samplerStates[_shadowSampler], pin.shadowTexcoord,
	                                                 shadowColor, shadowBias);
    directionDiffuse *= shadow;
    directionSpecular *= shadow;


	// ------ ポイントライト ------
    float3 pointDiffuse = (float3) 0;
    float3 pointSpecular = (float3) 0;
    int i;
    for (i = 0; i < pointLightCount; ++i)
    {
		// ライトのベクトルを算出
        float3 lightVector = pin.worldPosition.xyz - pointLightData[i].position.xyz;
        float lightLength = length(lightVector);

		// ライトの範囲外なら計算しない
        if (lightLength >= pointLightData[i].range)
            continue;

		// 影響力は距離に比例して小さくなっていく
        float attenuate = saturate(1.0f - lightLength / pointLightData[i].range);

        lightVector = lightVector / lightLength; // 正規化
        pointDiffuse += CalcLambertDiffse(N, lightVector, pointLightData[i].color.rgb, Kd) * attenuate;
        pointSpecular += CalcPhongSpecular(N, lightVector, pointLightData[i].color.rgb, ToCamera, 128.0f, Ks) *
			attenuate;
    }

	// ------ スポットライト ------
    float3 spotDiffuse = (float3) 0;
    float3 spotSpecular = (float3) 0;
    for (i = 0; i < spotLightCount; ++i)
    {
		// ライトベクトルを算出
        float3 lightVector = pin.worldPosition.xyz - spotLightData[i].position.xyz;
        float lightLength = length(lightVector);

		// ライトの範囲外なら計算しない
        if (lightLength >= spotLightData[i].range)
            continue;

		// 影響力は距離に比例して小さくなっていく
        float attenuate = saturate(1.0f - lightLength / spotLightData[i].range);

        lightVector = lightVector / lightLength; // 正規化

		// 角度減数を算出してattenuateに乗算する
        float3 spotDirection = normalize(spotLightData[i].direction.rgb);
        float angle = dot(spotDirection, lightVector);
        float area = spotLightData[i].innerCorn - spotLightData[i].outerCorn;
        attenuate *= saturate(1.0f - (spotLightData[i].innerCorn - angle) / area);

        spotDiffuse += CalcLambertDiffse(N, lightVector, spotLightData[i].color.rgb, Kd) * attenuate;
        spotSpecular += CalcPhongSpecular(N, lightVector, spotLightData[i].color.rgb, ToCamera, 128.0f, Ks) * attenuate;
    }

	// ------------ 環境光 ------------
    float3 ambient = ambientLightColor.rgb * Ka;


	// 拡散反射光と鏡面反射光から最終的な光を計算    
    float3 diffuseLig = directionDiffuse + pointDiffuse + spotDiffuse;
    float3 specularLig = (directionSpecular + pointSpecular + spotSpecular) * specularPower;

    float3 lig = diffuseLig + specularLig + ambient;

    float4 finalColor;
    finalColor.a = diffuseColor.a * pin.color.a;
    finalColor.rgb = diffuseColor.rgb * lig.rgb * pin.color.rgb;

	// 消したほうがいいかも(白飛びを軽く防ぐ)
    //finalColor.rgb = saturate(finalColor.rgb);

    finalColor.rgb += emissive.rgb * emissivePower;
	
    return finalColor;
}

#include "Phong.hlsli"
#include "PbrFunctions.hlsli"

// �^�C�}�[�̏��
cbuffer TimerConstant : register(_timerConstant)
{
    float deltaTime;
    float nowTime;
    float2 timePad;
};

SamplerState samplerStates[_samplerNum] : register(s0);

// PBR
Texture2D baseTexture : register(_baseTexture);
Texture2D normalTexture : register(_normalTexture);
Texture2D metallicRoughnessTexture : register(_metallicRoughnessTexture);
Texture2D emissiveTexture : register(_emissiveTexture);
Texture2D occlusionTexture : register(_occlusionTexture);
Texture2D flowTexture : register(_flowTexture);

// �e
Texture2D shadowTexture[SHADOWMAP_COUNT] : register(_shadowTexture);

// IBL
TextureCube diffuseIem : register(_IEM);
TextureCube specularPmrem : register(_PMREM);
Texture2D lutGgx : register(_LutGgx);

float4 main(VS_OUT pin) : SV_TARGET
{
    float flowPower = 0.5f;
    float flowSpeed = 0.05f;

    float2 flowDir = flowTexture.Sample(samplerStates[_anisotropicSampler], pin.texcoord) - 0.5f;

    float progress1 = frac(nowTime * flowSpeed);
    float progress2 = frac(nowTime * flowSpeed + 0.5f);
    float2 uv1 = pin.texcoord + flowDir * progress1;
    float2 uv2 = pin.texcoord + flowDir * progress2;

    float lerpRate = abs((0.5 - progress1) / 0.5f);

	// ====== �e�N�X�`�����擾 ======

	// --- �x�[�X�J���[ ---
    //float4 albedoColor = baseTexture.Sample(samplerStates[_pointSampler], pin.texcoord);
    float4 albed1 = baseTexture.Sample(samplerStates[_pointSampler], uv1);
    float4 albed2 = baseTexture.Sample(samplerStates[_pointSampler], uv2);
    float4 albedoColor = lerp(albed1, albed2, lerpRate);

    albedoColor.rgb = pow(albedoColor.rgb, GammaFactor);

	// --- �G�~�b�V�u ---
    float4 emissive1 = emissiveTexture.Sample(samplerStates[_pointSampler], uv1);
    float4 emissive2 = emissiveTexture.Sample(samplerStates[_pointSampler], uv2);
    float4 emissiveColor = lerp(emissive1, emissive2, lerpRate);
    emissiveColor.rgb = pow(emissiveColor.rgb, GammaFactor);


	// --- �@��/�]�@��/�ڐ� ---
    float4 normal1 = normalTexture.Sample(samplerStates[_pointSampler], uv1);
    float4 normal2 = normalTexture.Sample(samplerStates[_pointSampler], uv2);
    float4 normal = lerp(normal1, normal2, lerpRate);
    normal = (normal * 2.0) - 1.0;
    float3 n = normalize(pin.worldNormal.xyz);
    float3 t = normalize(pin.worldTangent.xyz);
    float sigma = pin.worldTangent.w;
    float3 b = normalize(cross(n, t) * sigma);
    float3 N = normalize((normal.x * t) + (normal.y * b) + (normal.z * n));

	// --- ������/�e�� ---
    float4 metallicRoughness1 = metallicRoughnessTexture.Sample(samplerStates[_pointSampler], uv1);
    float4 metallicRoughness2 = metallicRoughnessTexture.Sample(samplerStates[_pointSampler], uv2);
    float4 metallicRoughnessColor = lerp(metallicRoughness1, metallicRoughness2, lerpRate);
    float roughness = metallicRoughnessColor.a; 
    float metalness = metallicRoughnessColor.r;
    

	// �I�u�W�F�N�g�p�����[�^�ɂ�钲��
#if 0
	roughness = clamp(roughness + roughnessPower, 0.0, 1.0);
    metalness = clamp(metalness + metalnessPower, 0.0, 1.0);
#endif
    emissiveColor *= emissivePower;


	// --- ���̎Օ��� ---
    float4 occlusionColor = occlusionTexture.Sample(samplerStates[_anisotropicSampler], pin.texcoord);
    float occlusionFactor = occlusionColor.r;
    const float occlusionStrength = 1.0f;


	// ====== PBR IBL �v�Z ======

	// ���ˌ��̂����g�U���˂ɂȂ銄��(metalness �Ŕ��˗������܂�)
    float3 diffuseReflectance = lerp(albedoColor.rgb, 0.0f, metalness);

	// �������ˎ��̃t���l�����˗�(�ǂ�ȕ����ł��Œ�4%�͋��ʔ��˂���)
    float3 F0 = lerp(0.04f, albedoColor.rgb, metalness);

	// �����x�N�g��
    float3 E = normalize(pin.worldPosition.xyz - cameraPosition.xyz);

	// ------ �f�B���N�V�������C�g ------
    float3 directionDiffuse = (float3) 0;
    float3 directionSpecular = (float3) 0;
	{
        float3 L = normalize(directionalLightData.direction.xyz);
        DirectBDRF(diffuseReflectance, F0, N, E, L, directionalLightData.color.rgb, roughness, directionDiffuse,
		           directionSpecular);

		// ���s�����̉e�Ȃ̂ŁA���s�����ɑ΂��ĉe��K��
        float3 shadow = CalcCascadedShadowColorPCFFilter(shadowTexture, samplerStates[_shadowSampler],
		                                                 pin.shadowTexcoord,
		                                                 shadowColor, shadowBias);
        directionDiffuse *= shadow;
        directionSpecular *= shadow;
    }

	// �Օ�����
    directionDiffuse = lerp(directionDiffuse, directionDiffuse * occlusionFactor, occlusionStrength);
    directionSpecular = lerp(directionSpecular, directionSpecular * occlusionFactor, occlusionStrength);

	// IBL ����
    directionDiffuse += DiffuseIBL(N, E, roughness, diffuseReflectance, F0, diffuseIem, samplerStates[_linearSampler]);
    directionSpecular += SpecularIBL(N, E, roughness, F0, lutGgx, specularPmrem, samplerStates[_linearSampler]);

	// �F����(�G�~�b�V�u�������Œǉ�)
    float3 finalColor = directionDiffuse + directionSpecular + emissiveColor;
    finalColor *= pin.color.rgb;
    finalColor = pow(finalColor, 1.0f / GammaFactor);
    return float4(finalColor, albedoColor.a);
}

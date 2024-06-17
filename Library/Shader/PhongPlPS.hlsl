#include "Phong.hlsli"

SamplerState samplerStates[_samplerNum] : register(s0);

Texture2D diffuseTexture : register(_deffuseTexture);
Texture2D normalTexture : register(_normalTexture);
Texture2D specularTexture : register(_specularTexture);
Texture2D emissiveTexture : register(_emissiveTexture);

Texture2D shadowTexture[SHADOWMAP_COUNT] : register(_shadowTexture);

float4 main(VS_OUT pin) : SV_TARGET
{
	// �f�B�t���[�Y�}�b�v�̎擾
    float4 diffuseColor = diffuseTexture.Sample(samplerStates[_anisotropicSampler], pin.texcoord);
#if USE_LinearWorkflow
    diffuseColor.rgb = pow(diffuseColor.rgb, 2.2f);
#endif

	// �m�[�}���}�b�v�̎擾
    float4 normal = normalTexture.Sample(samplerStates[_anisotropicSampler], pin.texcoord);
    normal = (normal * 2.0) - 1.0;
    float3 n = normalize(pin.worldNormal.xyz);
    float3 t = normalize(pin.worldTangent.xyz);
    float sigma = pin.worldTangent.w;
    float3 b = normalize(cross(n, t) * sigma);
    float3 N = normalize((normal.x * t) + (normal.y * b) + (normal.z * n));

	// �X�y�L�����}�b�v�̎擾
    float specularPower = specularTexture.Sample(samplerStates[_anisotropicSampler], pin.texcoord);

	// �G�~�b�V�u�}�b�v�̎擾
    float4 emissive = emissiveTexture.Sample(samplerStates[_anisotropicSampler], pin.texcoord);

	// �\�ʍގ��̐���
    float3 Kd = float3(0.8, 0.8, 0.8);
    float3 Ks = float3(1, 1, 1);
    float3 Ka = float3(1, 1, 1);
    
	// ------ �f�B���N�V�������C�g ------
    float3 L = normalize(directionalLightData.direction.xyz);

	// �g�U���ˌ�
    float3 directionDiffuse = CalcLambertDiffse(N, L, directionalLightData.color.rgb, Kd);

	// ���ʔ��ˌ�
    float3 R = reflect(L, N); // ���˃x�N�g�������߂Ă���
    float3 ToCamera = normalize(cameraPosition.xyz - pin.worldPosition.xyz); // �������˂����T�[�t�F�[�X���王�_�Ɍ������ĐL�т�x�N�g�������߂Ă���
    float3 directionSpecular = CalcPhongSpecular(N, L, directionalLightData.color.rgb, ToCamera, 128.0f, Ks);



	// ���s�����̉e�Ȃ̂ŁA���s�����ɑ΂��ĉe��K��
    float3 shadow = CalcCascadedShadowColorPCFFilter(shadowTexture, samplerStates[_shadowSampler], pin.shadowTexcoord,
	                                                 shadowColor, shadowBias);
    directionDiffuse *= shadow;
    directionSpecular *= shadow;


	// ------ �|�C���g���C�g ------
    float3 pointDiffuse = (float3) 0;
    float3 pointSpecular = (float3) 0;
    int i;
    for (i = 0; i < pointLightCount; ++i)
    {
		// ���C�g�̃x�N�g�����Z�o
        float3 lightVector = pin.worldPosition.xyz - pointLightData[i].position.xyz;
        float lightLength = length(lightVector);

		// ���C�g�͈̔͊O�Ȃ�v�Z���Ȃ�
        if (lightLength >= pointLightData[i].range)
            continue;

		// �e���͂͋����ɔ�Ⴕ�ď������Ȃ��Ă���
        float attenuate = saturate(1.0f - lightLength / pointLightData[i].range);

        lightVector = lightVector / lightLength; // ���K��
        pointDiffuse += CalcLambertDiffse(N, lightVector, pointLightData[i].color.rgb, Kd) * attenuate;
        pointSpecular += CalcPhongSpecular(N, lightVector, pointLightData[i].color.rgb, ToCamera, 128.0f, Ks) *
			attenuate;
    }

	// ------ �X�|�b�g���C�g ------
    float3 spotDiffuse = (float3) 0;
    float3 spotSpecular = (float3) 0;
    for (i = 0; i < spotLightCount; ++i)
    {
		// ���C�g�x�N�g�����Z�o
        float3 lightVector = pin.worldPosition.xyz - spotLightData[i].position.xyz;
        float lightLength = length(lightVector);

		// ���C�g�͈̔͊O�Ȃ�v�Z���Ȃ�
        if (lightLength >= spotLightData[i].range)
            continue;

		// �e���͂͋����ɔ�Ⴕ�ď������Ȃ��Ă���
        float attenuate = saturate(1.0f - lightLength / spotLightData[i].range);

        lightVector = lightVector / lightLength; // ���K��

		// �p�x�������Z�o����attenuate�ɏ�Z����
        float3 spotDirection = normalize(spotLightData[i].direction.rgb);
        float angle = dot(spotDirection, lightVector);
        float area = spotLightData[i].innerCorn - spotLightData[i].outerCorn;
        attenuate *= saturate(1.0f - (spotLightData[i].innerCorn - angle) / area);

        spotDiffuse += CalcLambertDiffse(N, lightVector, spotLightData[i].color.rgb, Kd) * attenuate;
        spotSpecular += CalcPhongSpecular(N, lightVector, spotLightData[i].color.rgb, ToCamera, 128.0f, Ks) * attenuate;
    }

	// ------------ ���� ------------
    float3 ambient = ambientLightColor.rgb * Ka;


	// �g�U���ˌ��Ƌ��ʔ��ˌ�����ŏI�I�Ȍ����v�Z    
    float3 diffuseLig = directionDiffuse + pointDiffuse + spotDiffuse;
    float3 specularLig = (directionSpecular + pointSpecular + spotSpecular) * specularPower;

    float3 lig = diffuseLig + specularLig + ambient;

    float4 finalColor;
    finalColor.a = diffuseColor.a * pin.color.a;
    finalColor.rgb = diffuseColor.rgb * lig.rgb * pin.color.rgb;

    finalColor.rgb += emissive.rgb * emissivePower;


    float3 spec = CalcPhongSpecular(N, ToCamera, directionalLightData.color.rgb, ToCamera, 1.0f, Ks);
    float ePower = smoothstep(0.1, 0.9, spec.z);
    finalColor.rgb += float3(1, 1, 1) * (ePower * 2.0);
	
    return finalColor;
}

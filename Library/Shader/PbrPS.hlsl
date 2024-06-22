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
    //	�K���}�W��
    static const float GammaFactor = 2.2f;

    // �x�[�X�J���[
    float4 baseColor = baseTexture.Sample(samplerStates[_pointSampler], pin.texcoord);
    baseColor.rgb = pow(baseColor.rgb, GammaFactor);

    // �G�~�b�V�u
    float4 emissiveColor = emissiveTexture.Sample(samplerStates[_pointSampler], pin.texcoord);
    emissiveColor.rgb = pow(emissiveColor.rgb, GammaFactor);


    // �@��/�]�@��/�ڐ�
    float4 normal = normalTexture.Sample(samplerStates[_anisotropicSampler], pin.texcoord);
    normal = (normal * 2.0) - 1.0;
    float3 n = normalize(pin.worldNormal.xyz);
    float3 t = normalize(pin.worldTangent.xyz);
    float sigma = pin.worldTangent.w;
    float3 b = normalize(cross(n, t) * sigma);
    float3 N = normalize((normal.x * t) + (normal.y * b) + (normal.z * n));

    // ������/�e��
    float4 metallicRoughnessColor = metallicRoughnessTexture.Sample(samplerStates[_pointSampler], pin.texcoord);
    float roughness = 1.0f - metallicRoughnessColor.a;
    float metalness = metallicRoughnessColor.r;

#if 01	//	�m�F�p�̃R�[�h�Ȃ̂Ŗ{���͕s�v
    //roughness = saturate(roughness + adjust_roughness);
    //metalness = saturate(metalness + adjust_metalness);
#endif


    // ���̎Օ���
    float4 occlusionColor = occlusionTexture.Sample(samplerStates[_anisotropicSampler], pin.texcoord);
    float occlusionFactor = occlusionColor.r;
    const float occlusionStrength = 0.0f;


    // �A���x�h(���������)
    float4 albedo = baseColor;

    // ���ˌ��̂����g�U���˂ɂȂ銄��
    float3 diffuseReflectance = lerp(albedo.rgb, 0.0f, metalness);

    // �������ˎ��̃t���l�����˗�(������ł��Œ�4%�͋��ʔ��˂���)
    float3 F0 = lerp(0.04f, albedo.rgb, metalness);

    // �����x�N�g��
    float3 V = normalize(pin.worldPosition.xyz - cameraPosition.xyz);

    // ���ڌ��̃V�F�[�f�B���O
    float3 totalDiffuse = (float3) 0;
    float3 totalSpecular = (float3) 0;
    {
		// ���s�����̏���
        float3 diffuse = (float3) 0;
        float3 specular = (float3) 0;

        float3 L = normalize(directionalLightData.direction.xyz);
        DirectBDRF(diffuseReflectance, F0, N, V, L, directionalLightData.color.rgb, roughness, diffuse, specular);
        totalDiffuse += diffuse;
        totalSpecular += specular;
    }

    // �Օ�����
    totalDiffuse = lerp(totalDiffuse, totalDiffuse * occlusionFactor, occlusionStrength);
    totalSpecular = lerp(totalSpecular, totalSpecular * occlusionFactor, occlusionStrength);

    // �F����(�G�~�b�V�u�������Œǉ�)
    float3 finalColor = totalDiffuse + totalSpecular + emissiveColor;
    finalColor = pow(finalColor, 1.0f / GammaFactor);
    return float4(finalColor, baseColor.a);
}

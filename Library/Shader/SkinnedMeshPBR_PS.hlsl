#include "SkinnedMeshPBR.hlsli"


#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

SamplerState sampler_states[3] : register(s0);
Texture2D texture_maps[4] : register(t0);
float4 main(VS_OUT pin) : SV_TARGET
{   
    // �A���x�h�J���[ (�g�U���ˌ�)
    float4 albedoColor = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    
    // �@���}�b�v�̎擾
    float4 normal = texture_maps[1].Sample(sampler_states[LINEAR], pin.texcoord);
    normal = (normal * 2.0) - 1.0;
    float3 n = normalize(pin.worldNormal.xyz);
    float3 t = normalize(pin.worldTangent.xyz);
    float sigma = pin.worldTangent.w;
    float3 b = normalize(cross(n, t) * sigma);
    float3 N = normalize((normal.x * t) + (normal.y * b) + (normal.z * n));
    
    // �X�y�L�����}�b�v�J���[
    float3 specColor = albedoColor;
    
    // �����x
    float metallic = texture_maps[2].Sample(sampler_states[LINEAR], pin.texcoord).r;
    
    // ���炩��
    float smooth = texture_maps[2].Sample(sampler_states[LINEAR], pin.texcoord).a;
    
    // AO�}�b�v�̎擾
    float ambientPower = texture_maps[3].Sample(sampler_states[LINEAR], pin.texcoord);
    
    // �\�ʍގ��̐���
    float3 Kd = float3(0.8, 0.8, 0.8);
    float3 Ks = float3(1, 1, 1);
    float3 Ka = float3(1, 1, 1);
    
    float3 ToCamera = normalize(cameraPosition.xyz - pin.worldPosition.xyz); // �������˂����T�[�t�F�[�X���王�_�Ɍ������ĐL�т�x�N�g�������߂Ă���
    // --- �f�B���N�V�������C�g ---
    float3 L = normalize(directionalLightData.direction.xyz);
    
    // �t���l�����˂��l�������g�U���˂��v�Z
    float diffuseFromFrenel = CalcDiffuseFromFresnel(N, L, ToCamera);
    // ���K�� Lambert �g�U���˂����߂�
    float3 lambertDiffuse = CalcLambertDiffse(N, L, directionalLightData.color.rgb, Kd) / PI;
    // �ŏI�I�Ȋg�U���ˌ������߂�
    float3 diffuse = albedoColor * diffuseFromFrenel * lambertDiffuse;
    
    
    
    // ���ʔ��ˌ�
    float3 R = reflect(L, N); // ���˃x�N�g�������߂Ă���
    float3 directionSpecular = CalcPhongSpecular(N, L, directionalLightData.color.rgb, ToCamera, 128.0f, Ks);
    float3 spec = CookTorranceSpecular(L, ToCamera, N, smooth) * directionalLightData.color.rgb;
    
    spec *= lerp(float3(1.0f, 1.0f, 1.0f), specColor, metallic);
    
    // ����
    float3 ambient = albedoColor.rgb * Ka;
    
    float3 lig = diffuse * (1.0f - smooth) + spec + ambient * ambientPower;
    
    // �ŏI�I�ȃJ���[���v�Z
    float4 finalColor = float4(lig, albedoColor.a);
    
    return finalColor;
}

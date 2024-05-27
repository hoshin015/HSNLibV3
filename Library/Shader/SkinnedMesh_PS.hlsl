#include "SkinnedMesh.hlsli"


#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

SamplerState sampler_states[3] : register(s0);
SamplerState shadowSampler : register(s3);
Texture2D texture_maps[4] : register(t0);
Texture2D shadowMap : register(t4);

//#if 1
//struct PSOUT
//{
//    float4 color : SV_TARGET0;
//    float4 flag : SV_TARGET1;
//};
//PSOUT main(VS_OUT pin)
//#else
//float4 main(VS_OUT pin) : SV_TARGET
//#endif
//{   
//    // �f�B�t���[�Y�}�b�v�̎擾
//    float4 diffuseColor = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
//    
//#if 0
//    // Inverse gamma process
//    const float GAMMA = 2.2;
//    diffuseColor.rgb = pow(diffuseColor.rgb, GAMMA);
//#endif
//    
//    
//    // �@���}�b�v�̎擾
//    float4 normal = texture_maps[1].Sample(sampler_states[LINEAR], pin.texcoord);
//    normal = (normal * 2.0) - 1.0;
//    float3 n = normalize(pin.worldNormal.xyz);
//    float3 t = normalize(pin.worldTangent.xyz);
//    float sigma = pin.worldTangent.w;
//    float3 b = normalize(cross(n, t) * sigma);
//    float3 N = normalize((normal.x * t) + (normal.y * b) + (normal.z * n));
//    
//    // �X�y�L�����}�b�v�̎擾
//    float4 specularPower = texture_maps[2].Sample(sampler_states[LINEAR], pin.texcoord);
//    
//    // �G�~�b�V�u�J���[�̎擾
//    float4 emissive = texture_maps[3].Sample(sampler_states[POINT], pin.texcoord);
//    
//    // �\�ʍގ��̐���
//    float3 Kd = float3(0.8, 0.8, 0.8);
//    float3 Ks = float3(1, 1, 1);
//    float3 Ka = float3(1, 1, 1);
//    
//    // --- �f�B���N�V�������C�g ---
//    float3 L = normalize(directionalLightData.direction.xyz);
//    
//    // �g�U���ˌ�
//    float3 directionDiffuse = CalcLambertDiffse(N, L, directionalLightData.color.rgb, Kd);
//    
//    // ���ʔ��ˌ�
//    float3 R = reflect(L, N); // ���˃x�N�g�������߂Ă���
//    float3 ToCamera = normalize(cameraPosition.xyz - pin.worldPosition.xyz); // �������˂����T�[�t�F�[�X���王�_�Ɍ������ĐL�т�x�N�g�������߂Ă���
//    float3 directionSpecular = CalcPhongSpecular(N, L, directionalLightData.color.rgb, ToCamera, 128.0f, Ks);
//    
//    // ���s�����̉e�Ȃ̂ŁA���s�����ɑ΂��ĉe��K��
//    float3 shadow = CalcShadowColorPCFFilter(shadowMap, shadowSampler, pin.shadowTexcoord, shadowColor, shadowBias);
//    directionDiffuse *= shadow;
//    directionSpecular *= shadow;
//
//    // --- �|�C���g���C�g ---
//    float3 pointDiffuse = (float3) 0;
//    float3 pointSpecular = (float3) 0;
//    int i;
//    for (i = 0; i < pointLightCount; ++i)
//    {
//        // ���C�g�̃x�N�g�����Z�o
//        float3 lightVector = pin.worldPosition.xyz - pointLightData[i].position.xyz;
//        float lightLength = length(lightVector);
//        
//        // ���C�g�͈̔͊O�Ȃ�v�Z���Ȃ�
//        if (lightLength >= pointLightData[i].range)
//            continue;
//        
//        // �e���͂͋����ɔ�Ⴕ�ď������Ȃ��Ă���
//        float attenuate = saturate(1.0f - lightLength / pointLightData[i].range);
//        
//        lightVector = lightVector / lightLength;    // ���K��
//        pointDiffuse += CalcLambertDiffse(N, lightVector, pointLightData[i].color.rgb, Kd) * attenuate;
//        pointSpecular += CalcPhongSpecular(N, lightVector, pointLightData[i].color.rgb, ToCamera, 128.0f, Ks) * attenuate;
//    }
//    
//    // --- �X�|�b�g���C�g ---
//    float3 spotDiffuse = (float3) 0;
//    float3 spotSpecular = (float3) 0;
//    for (i = 0; i < spotLightCount; ++i)
//    {
//        // ���C�g�x�N�g�����Z�o
//        float3 lightVector = pin.worldPosition.xyz - spotLightData[i].position.xyz;
//        float lightLength = length(lightVector);
//        
//        // ���C�g�͈̔͊O�Ȃ�v�Z���Ȃ�
//        if (lightLength >= spotLightData[i].range)
//            continue;
//        
//        // �e���͂͋����ɔ�Ⴕ�ď������Ȃ��Ă���
//        float attenuate = saturate(1.0f - lightLength / spotLightData[i].range);
//        
//        lightVector = lightVector / lightLength; // ���K��
//        
//        // �p�x�������Z�o����attenuate�ɏ�Z����
//        float3 spotDirection = normalize(spotLightData[i].direction.rgb);
//        float angle = dot(spotDirection, lightVector);
//        float area = spotLightData[i].innerCorn - spotLightData[i].outerCorn;
//        attenuate *= saturate(1.0f - (spotLightData[i].innerCorn - angle) / area);
//        
//        spotDiffuse += CalcLambertDiffse(N, lightVector, spotLightData[i].color.rgb, Kd) * attenuate;
//        spotSpecular += CalcPhongSpecular(N, lightVector, spotLightData[i].color.rgb, ToCamera, 128.0f, Ks) * attenuate;
//    }
//    
//    // ����
//    float3 ambient = ambientLightColor.rgb * Ka;
//    
//    // �g�U���ˌ��Ƌ��ʔ��ˌ�����ŏI�I�Ȍ����v�Z    
//    float3 diffuseLig = directionDiffuse + pointDiffuse + spotDiffuse;
//    float3 specularLig = (directionSpecular + pointSpecular + spotSpecular) * specularPower;
//
//    float3 lig = diffuseLig + specularLig + ambient;
//    
//    // �ŏI�I�ȃJ���[���v�Z
//    float4 finalColor;
//    
//    finalColor.a = diffuseColor.a * pin.color.a;
//    
//    //finalColor.rgb = (emissive.a) ? emissive.rgb : lig * diffuseColor.rgb;
//    finalColor.rgb = lerp(lig * diffuseColor.rgb, emissive.rgb * 5, emissive.a) * pin.color.rgb;
//    
//    //finalColor.rgb = emissive.rgb + diffuseColor.rgb * lig;
//   
//#if 1
//    PSOUT output = (PSOUT)0;
//    output.color = finalColor;
//    
//    output.flag = float4(0,0,0,1);
//    output.flag = emissive.a;
//    //output.flag.r = length(emissive.rgb);
//    //output.flag.r = max(max(emissive.r, emissive.g), emissive.b);
//    
//    return output;
//#else
//    return finalColor;
//#endif
//}


float4 main(VS_OUT pin) : SV_TARGET
{   
    // �f�B�t���[�Y�}�b�v�̎擾
    float4 diffuseColor = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    
#if 0
    // Inverse gamma process
    const float GAMMA = 2.2;
    diffuseColor.rgb = pow(diffuseColor.rgb, GAMMA);
#endif
    
    
    // �@���}�b�v�̎擾
    float4 normal = texture_maps[1].Sample(sampler_states[LINEAR], pin.texcoord);
    normal = (normal * 2.0) - 1.0;
    float3 n = normalize(pin.worldNormal.xyz);
    float3 t = normalize(pin.worldTangent.xyz);
    float sigma = pin.worldTangent.w;
    float3 b = normalize(cross(n, t) * sigma);
    float3 N = normalize((normal.x * t) + (normal.y * b) + (normal.z * n));
    
    // �X�y�L�����}�b�v�̎擾
    float specularPower = texture_maps[2].Sample(sampler_states[LINEAR], pin.texcoord).r;
    
    // AO�}�b�v�̎擾
    float ambientPower = texture_maps[3].Sample(sampler_states[LINEAR], pin.texcoord);
    
    // �\�ʍގ��̐���
    float3 Kd = float3(0.8, 0.8, 0.8);
    float3 Ks = float3(1, 1, 1);
    float3 Ka = float3(1, 1, 1);
    
    // --- �f�B���N�V�������C�g ---
    float3 L = normalize(directionalLightData.direction.xyz);
    
    // �g�U���ˌ�
    float3 directionDiffuse = CalcLambertDiffse(N, L, directionalLightData.color.rgb, Kd);
    
    // ���ʔ��ˌ�
    float3 R = reflect(L, N); // ���˃x�N�g�������߂Ă���
    float3 ToCamera = normalize(cameraPosition.xyz - pin.worldPosition.xyz); // �������˂����T�[�t�F�[�X���王�_�Ɍ������ĐL�т�x�N�g�������߂Ă���
    float3 directionSpecular = CalcPhongSpecular(N, L, directionalLightData.color.rgb, ToCamera, 128.0f, Ks);
    
    // ���s�����̉e�Ȃ̂ŁA���s�����ɑ΂��ĉe��K��
    float3 shadow = CalcShadowColorPCFFilter(shadowMap, shadowSampler, pin.shadowTexcoord, shadowColor, shadowBias);
    directionDiffuse *= shadow;
    directionSpecular *= shadow;

    // --- �|�C���g���C�g ---
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
        
        lightVector = lightVector / lightLength;    // ���K��
        pointDiffuse += CalcLambertDiffse(N, lightVector, pointLightData[i].color.rgb, Kd) * attenuate;
        pointSpecular += CalcPhongSpecular(N, lightVector, pointLightData[i].color.rgb, ToCamera, 128.0f, Ks) * attenuate;
    }
    
    // --- �X�|�b�g���C�g ---
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
    
    // ����
    float3 ambient = ambientLightColor.rgb * Ka * ambientPower;
    
    // �g�U���ˌ��Ƌ��ʔ��ˌ�����ŏI�I�Ȍ����v�Z    
    float3 diffuseLig = directionDiffuse + pointDiffuse + spotDiffuse;
    float3 specularLig = (directionSpecular + pointSpecular + spotSpecular) * specularPower;

    float3 lig = diffuseLig + specularLig + ambient;
    
    // �ŏI�I�ȃJ���[���v�Z
    float4 finalColor = float4(lig * diffuseColor.rgb, diffuseColor.a);
    
    return finalColor;
}

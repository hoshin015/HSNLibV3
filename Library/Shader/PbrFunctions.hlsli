// �~����
static const float PI = 3.141592654f;

//--------------------------------------------
//	�t���l����
//--------------------------------------------
//F0	: �������ˎ��̔��˗�
//VdotH	: �����x�N�g���ƃn�[�t�x�N�g���i�����ւ̃x�N�g���Ǝ��_�ւ̃x�N�g���̒��ԃx�N�g��
float3 CalcFresnel(float3 F0, float VdotH)
{
    return F0 + (1.0f - F0) * pow(clamp(1.0f - VdotH, 0.0f, 1.0f), 5.0f);
}

//--------------------------------------------
//	�g�U����BRDF(���K�������o�[�g�̊g�U����)
//--------------------------------------------
//VdotH		: �����ւ̃x�N�g���ƃn�[�t�x�N�g���Ƃ̓���
//fresnelF0	: �������ˎ��̃t���l�����ːF
//diffuseReflectance	: ���ˌ��̂����g�U���˂ɂȂ銄��
float3 DiffuseBRDF(float VdotH, float3 fresnelF0, float3 diffuseReflectance)
{
    return (1.0f - CalcFresnel(fresnelF0, VdotH)) * (diffuseReflectance / PI);
}

//--------------------------------------------
//	�@�����z�֐�
//--------------------------------------------
//NdotH		: �@���x�N�g���ƃn�[�t�x�N�g���i�����ւ̃x�N�g���Ǝ��_�ւ̃x�N�g���̒��ԃx�N�g���j�̓���
//roughness : �e��
float CalcNormalDistributionFunction(float NdotH, float roughness)
{
    float a = roughness * roughness;
    float b = (NdotH * NdotH) * (a - 1.0f) + 1.0f;
    return a / (PI * b * b);
}

//--------------------------------------------
//	�􉽌������̎Z�o
//--------------------------------------------
//NdotL		: �@���x�N�g���ƌ����ւ̃x�N�g���Ƃ̓���
//NdotV		: �@���x�N�g���Ǝ����ւ̃x�N�g���Ƃ̓���
//roughness : �e��
float CalcGeometryFunction(float NdotL, float NdotV, float roughness)
{
    float r = roughness * 0.5f;
    float shadowing = NdotL / (NdotL * (1.0 - r) + r);
    float masking = NdotV / (NdotV * (1.0 - r) + r);
    return shadowing * masking;
}

//--------------------------------------------
//	���ʔ���BRDF�i�N�b�N�E�g�����X�̃}�C�N���t�@�Z�b�g���f���j
//--------------------------------------------
//NdotV		: �@���x�N�g���Ǝ����ւ̃x�N�g���Ƃ̓���
//NdotL		: �@���x�N�g���ƌ����ւ̃x�N�g���Ƃ̓���
//NdotH		: �@���x�N�g���ƃn�[�t�x�N�g���Ƃ̓���
//VdotH		: �����ւ̃x�N�g���ƃn�[�t�x�N�g���Ƃ̓���
//fresnelF0	: �������ˎ��̃t���l�����ːF
//roughness	: �e��
float3 SpecularBRDF(float NdotV, float NdotL, float NdotH, float VdotH, float3 fresnelF0, float roughness)
{
	//	D��(�@�����z)
    float D = CalcNormalDistributionFunction(NdotH, roughness);
	//	G��(�􉽌�����)
    float G = CalcGeometryFunction(NdotL, NdotV, roughness);
	//	F��(�t���l������)
    float3 F = CalcFresnel(fresnelF0, VdotH);

    return D * G * F / (NdotL * NdotV * 4.0f);
}

//--------------------------------------------
//	���ڌ��̕����x�[�X���C�e�B���O
//--------------------------------------------
//diffuseReflectance	: ���ˌ��̂����g�U���˂ɂȂ銄��
//F0					: �������ˎ��̃t���l�����ːF
//normal				: �@���x�N�g��(���K���ς�)
//eyeVector				: ���_�Ɍ������x�N�g��(���K���ς�)
//lightVector			: �����Ɍ������x�N�g��(���K���ς�)
//lightColor			: ���C�g�J���[
//roughness				: �e��
void DirectBDRF(float3 diffuseReflectance,
				float3 F0,
				float3 normal,
				float3 eyeVector,
				float3 lightVector,
				float3 lightColor,
				float roughness,
				out float3 outDiffuse,
				out float3 outSpecular)
{
    float3 N = normal;
    float3 L = -lightVector;
    float3 V = -eyeVector;
    float3 H = normalize(L + V);

    float NdotV = max(0.0001f, dot(N, V));
    float NdotL = max(0.0001f, dot(N, L));
    float NdotH = max(0.0001f, dot(N, H));
    float VdotH = max(0.0001f, dot(V, H));

    float3 irradiance = lightColor * NdotL;

	//	�g�U����BRDF
    outDiffuse = DiffuseBRDF(VdotH, F0, diffuseReflectance) * irradiance;

	//	���ʔ���BRDF
    outSpecular = SpecularBRDF(NdotV, NdotL, NdotH, VdotH, F0, roughness) * irradiance;
}

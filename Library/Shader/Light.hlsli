// ���s�������
struct DirectionalLightData
{
    float4 direction;   // ����
    float4 color;       // �F
};
// �_�������
struct PointLightData
{
    float4 position;
    float4 color;
    float range;
    float3 pad;
};
// �_�����̍ő吔
static const int PointLightMax = 8;
// �X�|�b�g���C�g
struct SpotLightData
{
    float4 position;
    float4 direction;
    float4 color;
    float range;
    float innerCorn;
    float outerCorn;
    float dummy;
};
// �X�|�b�g���C�g�̍ő吔
static const int SpotLightMax = 8;


//--------------------------------------------
//	�����o�[�g�g�U���ˌv�Z�֐�
//--------------------------------------------
// normal		: �@��(���K���ς�)
// lightVector	: ���˃x�N�g��(���K���ς�)
// lightColor	: ���ˌ��F
// kd			: ���˗�(���˂̋���)
// �Ԃ��l		: �g�U���ːF
float3 CalcLambertDiffse(float3 normal, float3 lightVector, float3 lightColor, float3 kd)
{ 
    float3 diffuse = max(0, dot(normal, -lightVector)); // -��t���Ă���͓̂��ς̌��ʂ��x�N�g�������������Ă���ꍇ�� 1 �ɂ��邽��
    return kd * lightColor * diffuse;
}

//--------------------------------------------
//	�t�H���̋��ʔ��ˌv�Z�֐�
//--------------------------------------------
// normal		: �@��(���K���ς�)
// lightVector	: ���˃x�N�g��(���K���ς�)
// lightColor	: ���ˌ��F
// eyeVector	: �����x�N�g��(���K���ς�)
// shininess	: ����x(���������l)
// ks			: ���˗�(���˂̋���)
// �Ԃ��l		: ���ʔ��ːF
float3 CalcPhongSpecular(float3 normal, float3 lightVector, float3 lightColor, float3 eyeVector, float shininess, float3 ks)
{
    float3 R = reflect(lightVector, normal); // ���˃x�N�g�������߂Ă���
    float3 specular = pow(max(0, dot(R, eyeVector)), shininess);
    return ks * lightColor * specular;
}
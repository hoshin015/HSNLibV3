#include "Sprite3D.hlsli"

VS_OUT main(float3 poisition : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD)
{
    VS_OUT vout;
    // ���[�J�����W���烏�[���h���W�ɕϊ������̌�A�X�N���[�����W�ɕϊ�
    vout.position = mul(float4(poisition.xyz, 1), viewProjection);
    vout.color = color;
    vout.texcoord = texcoord;
    return vout;
}
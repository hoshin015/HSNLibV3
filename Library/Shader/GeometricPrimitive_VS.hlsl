#include "GeometricPrimitive.hlsli"

VS_OUT main(float4 poisition : POSITION)
{
    VS_OUT vout;
    // ���[�J�����W���烏�[���h���W�ɕϊ������̌�A�X�N���[�����W�ɕϊ�
    vout.position = mul(poisition, mul(world, viewProjection));
    vout.color = materialColor;
    return vout;
}
#include "../RegisterNum.h"


// �f�B�]���u
void dissolve(inout float4 color, float dissolveThreshold, float edgeThreshold, float4 edgeColor, Texture2D maskTexture, SamplerState state, float2 uv)
{
    // �}�X�N�̐F�� 0 dissolveThreshold �� 0 �̂Ƃ��ɑΉ����邽�߂�
    clip(dissolveThreshold - 0.01f);

	// �}�X�N�摜����ԐF���擾
    float mask = maskTexture.Sample(state, uv).r - 0.01f;
	// step�֐���p����mask�̒l��dissolveThreshold�̒l���r���ē��ߒl��0 or 1�ɂ���
    float alpha = step(mask, dissolveThreshold);
	// ���̏���
    float edgeValue = step(mask - dissolveThreshold, dissolveThreshold) * step(dissolveThreshold, mask) * step(mask, dissolveThreshold + edgeThreshold);


    // step(a, x)	(x >= a) ? 1 : 0
    // step(mask, dissolveThreshold)	(dissolveThreshold >= mask) ? 1 : 0

    //color.rgb += edgeColor.rgb * edgeValue;
    alpha = saturate(alpha + edgeValue);

	// color�̓��ߒl�ɏ�Z����
    color.a *= alpha;
	// �A���t�@��0�ȉ��Ȃ炻�������`�悵�Ȃ��悤�ɂ���
    clip(color.a - 0.01f);
}


// �^�C�����O
float2 tilling(float2 uv, float2 tile)
{
    float2 tilling = uv;
    tilling.x *= tile.x;
    tilling.y *= tile.y;
    return tilling;
}


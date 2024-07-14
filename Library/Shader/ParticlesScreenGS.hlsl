#include "Particles.hlsli"

#define screenWidth 1280.0f
#define screenHeight 720.0f;

StructuredBuffer<Particle> particleBuffer : register(t9);

[maxvertexcount(4)]
void main(
	point VS_OUT input[1] : SV_POSITION,
	inout TriangleStream<GS_OUT> output
)
{
    const float2 corners[] =
    {
        float2(-0.5f, -0.5f),   // ����
		float2(-0.5f, +0.5f),   // ����
		float2(+0.5f, -0.5f),   // �E��
		float2(+0.5f, +0.5f),   // �E��
    };
    float2 texcoords[] =
    {
        float2(0.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f),
    };

    Particle p = particleBuffer[input[0].vertexId];

    if (!p.isActive)
        return;

	// �A�X�y�N�g����l�������X�P�[�����O
    const float aspectRatio = 1280.0 / 720.0;
    float2 particleScale = float2(p.scale.x, p.scale.y);

    float2 originalPos = p.position.xy;

	// --- ���_�����o�^ ---
	[unroll]
    for (uint vertexIndex = 0; vertexIndex < 4; vertexIndex++)
    {
        GS_OUT element;

		// �R�[�i�[�̈ʒu���X�P�[�����O
        float2 scaleCorner;
        
        scaleCorner.x = corners[vertexIndex].x * particleScale.x;
        scaleCorner.y = corners[vertexIndex].y * particleScale.y;

    	float2 offsetPos = originalPos + scaleCorner.xy;

        //--- < �X�N���[�����W�n����N���b�v���W�ϊ� > ---
        element.position.x = 2.0f * offsetPos.x / screenWidth - 1.0f;
        element.position.y = 1.0f - 2.0f * offsetPos.y / screenHeight;
        element.position.zw = float2(0, 1);

        element.color = p.color;
        element.texcoord = texcoords[vertexIndex];

        output.Append(element);
    }
    output.RestartStrip();
}

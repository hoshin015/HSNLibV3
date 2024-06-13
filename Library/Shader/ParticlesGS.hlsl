#include "Particles.hlsli"

StructuredBuffer<Particle> particleBuffer : register(t9);

[maxvertexcount(4)]
void main(
	point VS_OUT input[1] : SV_POSITION,
	inout TriangleStream<GS_OUT> output
)
{
    const float2 corners[] =
    {
        float2(-1.0f, -1.0f),
        float2(-1.0f, +1.0f),
        float2(+1.0f, -1.0f),
        float2(+1.0f, +1.0f),
    };
    const float2 texcoords[] =
    {
        float2(0.0f, 1.0f),
        float2(0.0f, 0.0f),
        float2(1.0f, 1.0f),
        float2(1.0f, 0.0f),
    };

    Particle p = particleBuffer[input[0].vertexId];

    // �A�X�y�N�g����l�������X�P�[�����O
    const float aspectRatio = 1280.0 / 720.0;
    float2 particleScale = float2(p.scale.x, p.scale.y);

    // �p�x
    float setAngle = float2(0,0);

    if (p.billboardType == 1)
    {
	    // �X�N���[�����W�n�� velocity ���v�Z
        float4 screenVelocity = mul(float4(p.velocity, 0.0), viewProjection);
        screenVelocity = normalize(screenVelocity);

		// �X�N���[�����W�n�̊p�x���v�Z
        setAngle = atan2(screenVelocity.y, screenVelocity.x);
    }


    // ��]�s����v�Z
    float cosAngle = cos(setAngle);
    float sinAngle = sin(setAngle);

    [unroll]
    for (uint vertexIndex = 0; vertexIndex < 4; vertexIndex++)
    {
        GS_OUT element;

        // �R�[�i�[�̈ʒu���X�P�[�����O
        float2 scaleCorner;

        scaleCorner.x = corners[vertexIndex].x * particleScale.x;
        scaleCorner.y = corners[vertexIndex].y * particleScale.y;

        // ��]��K�p
        float2 rotateCorner;
        rotateCorner.x = scaleCorner.x * cosAngle - scaleCorner.y * sinAngle;
        rotateCorner.y = scaleCorner.x * sinAngle + scaleCorner.y * cosAngle;

        rotateCorner.y *= aspectRatio;

        // �N���b�v��Ԃւ̕ϊ�
        element.position = mul(float4(p.position, 1), viewProjection);
        element.position.xy += rotateCorner;

        element.color = p.color;
        element.texcoord = texcoords[vertexIndex];

        output.Append(element);
    }

    output.RestartStrip();
}

#include "Particles.hlsli"

#define TEX_W_SIZE 10240.0f
#define TEX_H_SIZE 10240.0f
#define TEX_P_SIZE 1024.0f
#define TEX_P_HALF_SIZE 512.0f


StructuredBuffer<Particle> particleBuffer : register(t9);

[maxvertexcount(4)]
void main(
	point VS_OUT                 input[1] : SV_POSITION,
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
	//float2 texcoords[] =
	//{
	//	float2(0.0f, 1.0f),
	//	float2(0.0f, 0.0f),
	//	float2(1.0f, 1.0f),
	//	float2(1.0f, 0.0f),
	//};
	float2 texcoords[4];

	Particle p = particleBuffer[input[0].vertexId];

    if (!p.isActive)
        return;

    switch (p.textureType)
    {
        case 0:
	{
                texcoords[0] = float2(TEX_P_SIZE * 0.0f / TEX_W_SIZE, TEX_P_SIZE * 1.0f / TEX_H_SIZE);
                texcoords[1] = float2(TEX_P_SIZE * 0.0f / TEX_W_SIZE, TEX_P_SIZE * 0.0f / TEX_H_SIZE);
                texcoords[2] = float2(TEX_P_SIZE * 1.0f / TEX_W_SIZE, TEX_P_SIZE * 1.0f / TEX_H_SIZE);
                texcoords[3] = float2(TEX_P_SIZE * 1.0f / TEX_W_SIZE, TEX_P_SIZE * 0.0f / TEX_H_SIZE);
            }
            break;
        case 1:
	{
                texcoords[0] = float2(TEX_P_SIZE * 1.0f / TEX_W_SIZE, TEX_P_SIZE * 1.0f / TEX_H_SIZE);
                texcoords[1] = float2(TEX_P_SIZE * 1.0f / TEX_W_SIZE, TEX_P_SIZE * 0.0f / TEX_H_SIZE);
                texcoords[2] = float2(TEX_P_SIZE * 2.0f / TEX_W_SIZE, TEX_P_SIZE * 1.0f / TEX_H_SIZE);
                texcoords[3] = float2(TEX_P_SIZE * 2.0f / TEX_W_SIZE, TEX_P_SIZE * 0.0f / TEX_H_SIZE);
            }
            break;
        case 2:
	{
                texcoords[0] = float2(TEX_P_SIZE * 2.0f / TEX_W_SIZE, TEX_P_SIZE * 1.0f / TEX_H_SIZE);
                texcoords[1] = float2(TEX_P_SIZE * 2.0f / TEX_W_SIZE, TEX_P_SIZE * 0.0f / TEX_H_SIZE);
                texcoords[2] = float2(TEX_P_SIZE * 3.0f / TEX_W_SIZE, TEX_P_SIZE * 1.0f / TEX_H_SIZE);
                texcoords[3] = float2(TEX_P_SIZE * 3.0f / TEX_W_SIZE, TEX_P_SIZE * 0.0f / TEX_H_SIZE);
            }
            break;
        case 3:
	{
                texcoords[0] = float2(TEX_P_SIZE * 3.0f / TEX_W_SIZE, TEX_P_SIZE * 1.0f / TEX_H_SIZE);
                texcoords[1] = float2(TEX_P_SIZE * 3.0f / TEX_W_SIZE, TEX_P_SIZE * 0.0f / TEX_H_SIZE);
                texcoords[2] = float2(TEX_P_SIZE * 4.0f / TEX_W_SIZE, TEX_P_SIZE * 1.0f / TEX_H_SIZE);
                texcoords[3] = float2(TEX_P_SIZE * 4.0f / TEX_W_SIZE, TEX_P_SIZE * 0.0f / TEX_H_SIZE);
            }
            break;
        case 4:
	{
                texcoords[0] = float2(TEX_P_SIZE * 4.0f / TEX_W_SIZE, TEX_P_SIZE * 1.0f / TEX_H_SIZE);
                texcoords[1] = float2(TEX_P_SIZE * 4.0f / TEX_W_SIZE, TEX_P_SIZE * 0.0f / TEX_H_SIZE);
                texcoords[2] = float2(TEX_P_SIZE * 5.0f / TEX_W_SIZE, TEX_P_SIZE * 1.0f / TEX_H_SIZE);
                texcoords[3] = float2(TEX_P_SIZE * 5.0f / TEX_W_SIZE, TEX_P_SIZE * 0.0f / TEX_H_SIZE);
            }
            break;
        case 5:
	{
                texcoords[0] = float2(TEX_P_SIZE * 5.0f / TEX_W_SIZE, TEX_P_SIZE * 1.0f / TEX_H_SIZE);
                texcoords[1] = float2(TEX_P_SIZE * 5.0f / TEX_W_SIZE, TEX_P_SIZE * 0.0f / TEX_H_SIZE);
                texcoords[2] = float2(TEX_P_SIZE * 6.0f / TEX_W_SIZE, TEX_P_SIZE * 1.0f / TEX_H_SIZE);
                texcoords[3] = float2(TEX_P_SIZE * 6.0f / TEX_W_SIZE, TEX_P_SIZE * 0.0f / TEX_H_SIZE);
            }
            break;
        case 6:
	{
		// ���݉��t���[���ڂ����v�Z
                float animIndex = (1.0 - (p.lifeTimer / p.lifeTime)) * 80.0;

                int _x = animIndex % 20;
                int _y = animIndex / 20;

                _y += 2; // ��������1�i(512*2 px)�����������炻�̕�


                texcoords[0] = float2(TEX_P_HALF_SIZE * _x / TEX_W_SIZE, TEX_P_HALF_SIZE * (_y + 1) / TEX_H_SIZE);
                texcoords[1] = float2(TEX_P_HALF_SIZE * _x / TEX_W_SIZE, TEX_P_HALF_SIZE * _y / TEX_H_SIZE);
                texcoords[2] = float2(TEX_P_HALF_SIZE * (_x + 1) / TEX_W_SIZE, TEX_P_HALF_SIZE * (_y + 1) / TEX_H_SIZE);
                texcoords[3] = float2(TEX_P_HALF_SIZE * (_x + 1) / TEX_W_SIZE, TEX_P_HALF_SIZE * _y / TEX_H_SIZE);
            }
            break;
        case 7:
	{
		// ���݉��t���[���ڂ����v�Z
                float animIndex = (1.0 - (p.lifeTimer / p.lifeTime)) * 20.0;

                int _x = animIndex % 20;
                int _y = animIndex / 20;

                _y += 6; // ��������3�i(512*6 px)�����������炻�̕�


                texcoords[0] = float2(TEX_P_HALF_SIZE * _x / TEX_W_SIZE, TEX_P_HALF_SIZE * (_y + 1) / TEX_H_SIZE);
                texcoords[1] = float2(TEX_P_HALF_SIZE * _x / TEX_W_SIZE, TEX_P_HALF_SIZE * _y / TEX_H_SIZE);
                texcoords[2] = float2(TEX_P_HALF_SIZE * (_x + 1) / TEX_W_SIZE, TEX_P_HALF_SIZE * (_y + 1) / TEX_H_SIZE);
                texcoords[3] = float2(TEX_P_HALF_SIZE * (_x + 1) / TEX_W_SIZE, TEX_P_HALF_SIZE * _y / TEX_H_SIZE);
            }
            break;
        case 8:
	{
		// ���݉��t���[���ڂ����v�Z
                float animIndex = (1.0 - (p.lifeTimer / p.lifeTime)) * 20.0;

                int _x = animIndex % 20;
                int _y = animIndex / 20;

                _y += 7; // ��������3.5�i(512*7 px)�����������炻�̕�


                texcoords[0] = float2(TEX_P_HALF_SIZE * _x / TEX_W_SIZE, TEX_P_HALF_SIZE * (_y + 1) / TEX_H_SIZE);
                texcoords[1] = float2(TEX_P_HALF_SIZE * _x / TEX_W_SIZE, TEX_P_HALF_SIZE * _y / TEX_H_SIZE);
                texcoords[2] = float2(TEX_P_HALF_SIZE * (_x + 1) / TEX_W_SIZE, TEX_P_HALF_SIZE * (_y + 1) / TEX_H_SIZE);
                texcoords[3] = float2(TEX_P_HALF_SIZE * (_x + 1) / TEX_W_SIZE, TEX_P_HALF_SIZE * _y / TEX_H_SIZE);
            }
            break;
        case 9:
	{
		// ���݉��t���[���ڂ����v�Z
                float animIndex = (1.0 - (p.lifeTimer / p.lifeTime)) * 80.0;

                int _x = animIndex % 20;
                int _y = animIndex / 20;

                _y += 8; // ��������4�i(1024*4 px)�����������炻�̕�


                texcoords[0] = float2(TEX_P_HALF_SIZE * _x / TEX_W_SIZE, TEX_P_HALF_SIZE * (_y + 1) / TEX_H_SIZE);
                texcoords[1] = float2(TEX_P_HALF_SIZE * _x / TEX_W_SIZE, TEX_P_HALF_SIZE * _y / TEX_H_SIZE);
                texcoords[2] = float2(TEX_P_HALF_SIZE * (_x + 1) / TEX_W_SIZE, TEX_P_HALF_SIZE * (_y + 1) / TEX_H_SIZE);
                texcoords[3] = float2(TEX_P_HALF_SIZE * (_x + 1) / TEX_W_SIZE, TEX_P_HALF_SIZE * _y / TEX_H_SIZE);
            }
            break;
        case 10:
	{
		// �Ή��u���X

		// ���݉��t���[���ڂ����v�Z
                float animIndex = (1.0 - (p.lifeTimer / p.lifeTime)) * 40.0;

                int _x = animIndex % 20;
                int _y = animIndex / 20;

                _y += 10; // ��������4�i(1024*6 px)�����������炻�̕�


                texcoords[0] = float2(TEX_P_HALF_SIZE * _x / TEX_W_SIZE, TEX_P_HALF_SIZE * (_y + 1) / TEX_H_SIZE);
                texcoords[1] = float2(TEX_P_HALF_SIZE * _x / TEX_W_SIZE, TEX_P_HALF_SIZE * _y / TEX_H_SIZE);
                texcoords[2] = float2(TEX_P_HALF_SIZE * (_x + 1) / TEX_W_SIZE, TEX_P_HALF_SIZE * (_y + 1) / TEX_H_SIZE);
                texcoords[3] = float2(TEX_P_HALF_SIZE * (_x + 1) / TEX_W_SIZE, TEX_P_HALF_SIZE * _y / TEX_H_SIZE);
            }
            break;
        case 11:
	{
                float animIndex = (1.0 - (p.lifeTimer / p.lifeTime)) * 21.0;

                int _x = animIndex % 10;
                int _y = animIndex / 10;

                _y += 6; // ��������6�i(1024*6 px)�����������炻�̕�
                texcoords[0] = float2(TEX_P_SIZE * _x / TEX_W_SIZE, TEX_P_SIZE * (_y + 1) / TEX_H_SIZE);
                texcoords[1] = float2(TEX_P_SIZE * _x / TEX_W_SIZE, TEX_P_SIZE * _y / TEX_H_SIZE);
                texcoords[2] = float2(TEX_P_SIZE * (_x + 1) / TEX_W_SIZE, TEX_P_SIZE * (_y + 1) / TEX_H_SIZE);
                texcoords[3] = float2(TEX_P_SIZE * (_x + 1) / TEX_W_SIZE, TEX_P_SIZE * _y / TEX_H_SIZE);
            }
            break;
    }


	// �A�X�y�N�g����l�������X�P�[�����O
	const float aspectRatio   = 1280.0 / 720.0;
	float2      particleScale = float2(p.scale.x, p.scale.y);

	// �p�x
    float setAngle = p.angle * 3.14 / 180;

	// billboardType �� velocity �Ȃ� velocity �̕����ɉ�]������
	if (p.billboardType == bt_Velocity)
	{
		// ndc ���W�n�� velocity ���v�Z
		float4 ndcVelocity = mul(float4(p.velocity, 0.0), viewProjection);
		ndcVelocity        = normalize(ndcVelocity);

		// ndc ���W�n�̊p�x���v�Z
		setAngle = atan2(ndcVelocity.y, ndcVelocity.x);
	}


	if (p.kind == pk_smoke)
	{
		// ndc ���W�n�� velocity ���v�Z
		float4 ndcVelocity = mul(float4(p.velocity, 0.0), viewProjection);
		ndcVelocity        = normalize(ndcVelocity);

		if (ndcVelocity.x < 0)
		{
			float2 foo[4] = texcoords;
			texcoords[0]  = foo[2];
			texcoords[1]  = foo[3];
			texcoords[2]  = foo[0];
			texcoords[3]  = foo[1];
		}
	}

	// ��]�s����v�Z
	float cosAngle = cos(setAngle);
	float sinAngle = sin(setAngle);

	// --- ���_�����o�^ ---
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

		element.color    = p.color;
		element.texcoord = texcoords[vertexIndex];

		output.Append(element);
	}
	output.RestartStrip();
}

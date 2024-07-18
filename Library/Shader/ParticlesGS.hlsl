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
		// 現在何フレーム目かを計算
                float animIndex = (1.0 - (p.lifeTimer / p.lifeTime)) * 80.0;

                int _x = animIndex % 20;
                int _y = animIndex / 20;

                _y += 2; // そもそも1段(512*2 px)だけ下だからその分


                texcoords[0] = float2(TEX_P_HALF_SIZE * _x / TEX_W_SIZE, TEX_P_HALF_SIZE * (_y + 1) / TEX_H_SIZE);
                texcoords[1] = float2(TEX_P_HALF_SIZE * _x / TEX_W_SIZE, TEX_P_HALF_SIZE * _y / TEX_H_SIZE);
                texcoords[2] = float2(TEX_P_HALF_SIZE * (_x + 1) / TEX_W_SIZE, TEX_P_HALF_SIZE * (_y + 1) / TEX_H_SIZE);
                texcoords[3] = float2(TEX_P_HALF_SIZE * (_x + 1) / TEX_W_SIZE, TEX_P_HALF_SIZE * _y / TEX_H_SIZE);
            }
            break;
        case 7:
	{
		// 現在何フレーム目かを計算
                float animIndex = (1.0 - (p.lifeTimer / p.lifeTime)) * 20.0;

                int _x = animIndex % 20;
                int _y = animIndex / 20;

                _y += 6; // そもそも3段(512*6 px)だけ下だからその分


                texcoords[0] = float2(TEX_P_HALF_SIZE * _x / TEX_W_SIZE, TEX_P_HALF_SIZE * (_y + 1) / TEX_H_SIZE);
                texcoords[1] = float2(TEX_P_HALF_SIZE * _x / TEX_W_SIZE, TEX_P_HALF_SIZE * _y / TEX_H_SIZE);
                texcoords[2] = float2(TEX_P_HALF_SIZE * (_x + 1) / TEX_W_SIZE, TEX_P_HALF_SIZE * (_y + 1) / TEX_H_SIZE);
                texcoords[3] = float2(TEX_P_HALF_SIZE * (_x + 1) / TEX_W_SIZE, TEX_P_HALF_SIZE * _y / TEX_H_SIZE);
            }
            break;
        case 8:
	{
		// 現在何フレーム目かを計算
                float animIndex = (1.0 - (p.lifeTimer / p.lifeTime)) * 20.0;

                int _x = animIndex % 20;
                int _y = animIndex / 20;

                _y += 7; // そもそも3.5段(512*7 px)だけ下だからその分


                texcoords[0] = float2(TEX_P_HALF_SIZE * _x / TEX_W_SIZE, TEX_P_HALF_SIZE * (_y + 1) / TEX_H_SIZE);
                texcoords[1] = float2(TEX_P_HALF_SIZE * _x / TEX_W_SIZE, TEX_P_HALF_SIZE * _y / TEX_H_SIZE);
                texcoords[2] = float2(TEX_P_HALF_SIZE * (_x + 1) / TEX_W_SIZE, TEX_P_HALF_SIZE * (_y + 1) / TEX_H_SIZE);
                texcoords[3] = float2(TEX_P_HALF_SIZE * (_x + 1) / TEX_W_SIZE, TEX_P_HALF_SIZE * _y / TEX_H_SIZE);
            }
            break;
        case 9:
	{
		// 現在何フレーム目かを計算
                float animIndex = (1.0 - (p.lifeTimer / p.lifeTime)) * 80.0;

                int _x = animIndex % 20;
                int _y = animIndex / 20;

                _y += 8; // そもそも4段(1024*4 px)だけ下だからその分


                texcoords[0] = float2(TEX_P_HALF_SIZE * _x / TEX_W_SIZE, TEX_P_HALF_SIZE * (_y + 1) / TEX_H_SIZE);
                texcoords[1] = float2(TEX_P_HALF_SIZE * _x / TEX_W_SIZE, TEX_P_HALF_SIZE * _y / TEX_H_SIZE);
                texcoords[2] = float2(TEX_P_HALF_SIZE * (_x + 1) / TEX_W_SIZE, TEX_P_HALF_SIZE * (_y + 1) / TEX_H_SIZE);
                texcoords[3] = float2(TEX_P_HALF_SIZE * (_x + 1) / TEX_W_SIZE, TEX_P_HALF_SIZE * _y / TEX_H_SIZE);
            }
            break;
        case 10:
	{
		// 火炎ブレス

		// 現在何フレーム目かを計算
                float animIndex = (1.0 - (p.lifeTimer / p.lifeTime)) * 40.0;

                int _x = animIndex % 20;
                int _y = animIndex / 20;

                _y += 10; // そもそも4段(1024*6 px)だけ下だからその分


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

                _y += 6; // そもそも6段(1024*6 px)だけ下だからその分
                texcoords[0] = float2(TEX_P_SIZE * _x / TEX_W_SIZE, TEX_P_SIZE * (_y + 1) / TEX_H_SIZE);
                texcoords[1] = float2(TEX_P_SIZE * _x / TEX_W_SIZE, TEX_P_SIZE * _y / TEX_H_SIZE);
                texcoords[2] = float2(TEX_P_SIZE * (_x + 1) / TEX_W_SIZE, TEX_P_SIZE * (_y + 1) / TEX_H_SIZE);
                texcoords[3] = float2(TEX_P_SIZE * (_x + 1) / TEX_W_SIZE, TEX_P_SIZE * _y / TEX_H_SIZE);
            }
            break;
    }


	// アスペクト比を考慮したスケーリング
	const float aspectRatio   = 1280.0 / 720.0;
	float2      particleScale = float2(p.scale.x, p.scale.y);

	// 角度
    float setAngle = p.angle * 3.14 / 180;

	// billboardType が velocity なら velocity の方向に回転させる
	if (p.billboardType == bt_Velocity)
	{
		// ndc 座標系の velocity を計算
		float4 ndcVelocity = mul(float4(p.velocity, 0.0), viewProjection);
		ndcVelocity        = normalize(ndcVelocity);

		// ndc 座標系の角度を計算
		setAngle = atan2(ndcVelocity.y, ndcVelocity.x);
	}


	if (p.kind == pk_smoke)
	{
		// ndc 座標系の velocity を計算
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

	// 回転行列を計算
	float cosAngle = cos(setAngle);
	float sinAngle = sin(setAngle);

	// --- 頂点生成登録 ---
	[unroll]
	for (uint vertexIndex = 0; vertexIndex < 4; vertexIndex++)
	{
		GS_OUT element;

		// コーナーの位置をスケーリング
		float2 scaleCorner;

		scaleCorner.x = corners[vertexIndex].x * particleScale.x;
		scaleCorner.y = corners[vertexIndex].y * particleScale.y;

		// 回転を適用
		float2 rotateCorner;
		rotateCorner.x = scaleCorner.x * cosAngle - scaleCorner.y * sinAngle;
		rotateCorner.y = scaleCorner.x * sinAngle + scaleCorner.y * cosAngle;

		rotateCorner.y *= aspectRatio;

		// クリップ空間への変換
		element.position = mul(float4(p.position, 1), viewProjection);
		element.position.xy += rotateCorner;

		element.color    = p.color;
		element.texcoord = texcoords[vertexIndex];

		output.Append(element);
	}
	output.RestartStrip();
}

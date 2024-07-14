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
        float2(-0.5f, -0.5f),   // 左下
		float2(-0.5f, +0.5f),   // 左上
		float2(+0.5f, -0.5f),   // 右下
		float2(+0.5f, +0.5f),   // 右上
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

	// アスペクト比を考慮したスケーリング
    const float aspectRatio = 1280.0 / 720.0;
    float2 particleScale = float2(p.scale.x, p.scale.y);

    float2 originalPos = p.position.xy;

	// --- 頂点生成登録 ---
	[unroll]
    for (uint vertexIndex = 0; vertexIndex < 4; vertexIndex++)
    {
        GS_OUT element;

		// コーナーの位置をスケーリング
        float2 scaleCorner;
        
        scaleCorner.x = corners[vertexIndex].x * particleScale.x;
        scaleCorner.y = corners[vertexIndex].y * particleScale.y;

    	float2 offsetPos = originalPos + scaleCorner.xy;

        //--- < スクリーン座標系からクリップ座標変換 > ---
        element.position.x = 2.0f * offsetPos.x / screenWidth - 1.0f;
        element.position.y = 1.0f - 2.0f * offsetPos.y / screenHeight;
        element.position.zw = float2(0, 1);

        element.color = p.color;
        element.texcoord = texcoords[vertexIndex];

        output.Append(element);
    }
    output.RestartStrip();
}

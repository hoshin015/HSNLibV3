#include "../RegisterNum.h"


// ディゾルブ
void dissolve(inout float4 color, float dissolveThreshold, float edgeThreshold, float4 edgeColor, Texture2D maskTexture, SamplerState state, float2 uv)
{
	// マスク画像から赤色を取得
    float mask = maskTexture.Sample(state, uv).r;
	// step関数を用いてmaskの値とdissolveThresholdの値を比較して透過値を0 or 1にする
    float alpha = step(mask, dissolveThreshold);
	// 縁の処理
    float edgeValue = step(mask - dissolveThreshold, dissolveThreshold) * step(dissolveThreshold, mask) * step(mask, dissolveThreshold + edgeThreshold);

    color.rgb += edgeColor.rgb * edgeValue;
    alpha = saturate(alpha + edgeValue);

	// colorの透過値に乗算する
    color.a *= alpha;
	// アルファが0以下ならそもそも描画しないようにする
    clip(color.a - 0.01f);
}


// タイリング
float2 tilling(float2 uv, float2 tile)
{
    float2 tilling = uv;
    tilling.x *= tile.x;
    tilling.y *= tile.y;
    return tilling;
}


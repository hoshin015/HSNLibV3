#pragma once
#include <memory>
#include "RockMainMesh.h"

class RockEffect
{
private:
	RockEffect() {}
	~RockEffect() {}

public:
	static RockEffect& Instance()
	{
		static RockEffect instance;
		return instance;
	}

	// 初期化
	void Initialize();

	// 更新
	void Update();
	// 描画
	void Render(bool isShadow = false);

	// 岩生成用情報
	struct RockEmitter
	{
		DirectX::XMFLOAT3 position;	// 生成座標
		DirectX::XMFLOAT3 angle;	// 生成角度(この角度から回転しません)
		DirectX::XMFLOAT3 scale;	// 生成スケール
		DirectX::XMFLOAT3 velocity;	// velocity (この値に毎秒重力が y 方向にのみ加算されます)
		float gravity;				// 重力
		float lifeTime = 10.0f;		// 寿命[s](この時間がたつと消えます)

		// 特殊な動きをする岩を作りたかったら行ってくれたら追加できるようにはしてます
	};
	// ロック追加
	void Emit(RockEmitter rockEmitter);

	// ロック追加
	void Emit(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 vel, DirectX::XMFLOAT2 scaleMinMax);


	std::unique_ptr<RockMainMesh> rockMesh1;
private:
};
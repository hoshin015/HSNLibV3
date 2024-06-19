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

	// ロック追加
	void Emit(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 vel, DirectX::XMFLOAT2 scaleMinMax);

	struct RockEmitter
	{
		DirectX::XMFLOAT3 position;
		float timer = 0.0f;
		float destoryTime = 10.0f;
		int generateCount = 10;
	};


	std::unique_ptr<RockMainMesh> rockMesh1;
private:
};
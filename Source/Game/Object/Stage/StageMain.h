#pragma once
#include <memory>
// --- Game ---
#include "Stage.h"

// ステージ
class StageMain : public Stage
{
public:
	StageMain(const char* filePath) : Stage(filePath) {}
	~StageMain() override {};

	// 更新処理
	void Update() override;

	// 描画処理
	void Render(bool isShadow = false) override;

	// レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;
};
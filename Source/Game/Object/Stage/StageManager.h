#pragma once
#include <vector>
// --- Game ---
#include "Stage.h"

// ステージマネージャー
class StageManager
{
private:
	StageManager() {}
	~StageManager() {}

public:
	// 唯一のインスタンス取得
	static StageManager& Instance()
	{
		static StageManager instance;
		return instance;
	}

	// 更新処理
	void Update();

	// 描画処理
	void Render(bool isShadow = false);

	// ステージ登録
	void Register(Stage* stage);

	// ステージ全削除
	void Clear();

	// レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

private:
	std::vector<Stage*> stages;
};
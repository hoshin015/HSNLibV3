#pragma once
// --- Game ---
#include "BaseObject.h"
#include "StaticModel.h"

// アニメーションしないモデル(インスタンス描画)
class StaticObject : public BaseObject
{
public:
	StaticObject() = default;
	StaticObject(const char* filePath);
	virtual ~StaticObject() = default;

	virtual void Update() = 0;
	virtual void Render(bool isShadow) = 0;

protected:
	std::unique_ptr<StaticModel> model;

	int					instancingCount = 0;		// インスタンシング数
	bool				exist[MAX_INSTANCE] = {};	// 描画するindexはtrueになる
	DirectX::XMFLOAT4X4 transforms[MAX_INSTANCE];	// indexの姿勢をいれる

	// 姿勢行列更新
	void UpdateTransform() override;

public:
	//// 割り当てて番号を返す
	//int AllocateInstancingIndex();
	//// 割り当てらた番号を解放する
	//void FreeInstancingIndex(int instancingIndex);
	//// 行列計算
	//void UpdateTransform(int instancingIndex, const DirectX::XMFLOAT4X4& transform);
	//// 現在の姿勢行列を取得
	//const DirectX::XMFLOAT4X4& GetTransform(int instancingIndex) const;
};
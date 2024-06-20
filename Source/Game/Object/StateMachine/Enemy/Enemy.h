#pragma once

#include "../../Base/AnimatedObject.h"

template<typename T>
class BehaviorTree;
template<typename T>
class BehaviorData;
template<typename T>
class NodeBase;

class Enemy : public AnimatedObject
{
private:
	Enemy(const char* filePath) : AnimatedObject(filePath) {}
	~Enemy() override;

public:
	static Enemy& Instance()
	{
		static Enemy instance("Data/Fbx/Albino/Albino.model");
		return instance;
	}

	void Initialize();
	void Update() override;
	void Render(bool isShadow = false) override;
	void DrawDebugGui();
	void DrawDebug();

	bool SearchPlayer();
	void RotateToTargetVec(const DirectX::XMFLOAT3& targetVec, float t);

private:
	// --- ビヘイビアツリー関連 ---
	void InitializeBehaviorTree();
	void FinalizeBehaviorTree();
	void UpdateBehaviorTree(float elapsedTime);

	BehaviorTree<Enemy>* aiTree_;		// この中にツリーが伸びていく
	BehaviorData<Enemy>* behaviorData_;	// ビヘイビアのデータを保存する変数
	NodeBase<Enemy>*	 activeNode_;	// 実行するノード

	float searchRange_ = 20.0f;

public:
	DirectX::XMFLOAT4 quaternion_ = { 0.0f, 0.0f, 0.0f, 1.0f };
	DirectX::XMFLOAT3 moveTargetPosition_;
	float runTimer_;

	float walkSpeed_ = 3.0f;
	float runSpeed_ = 10.0f;
};
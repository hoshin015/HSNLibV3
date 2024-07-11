#pragma once

#include "../../Library/Math/Matrix.h"

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
		static Enemy instance("Data/Fbx/Monster/gaoanimal_6.model");
		return instance;
	}

	const float GetWanderRange() const { return wanderRange; }
	const bool IsFoundPlayer() const { return foundPlayer; }

	const float GetRushSpeed() const { return rushSpeed; }
	const float GetRushEndSpeed() const { return rushEndSpeed; }
	const float GetRushChargeTimer() const { return rushChargeTimer; }
	const float GetWhileRushTimer() const { return whileRushTimer; }

	const float GetLongRange() const { return longRange; }
	const float GetMiddleRange() const { return middleRange; }
	const float GetShortRange() const { return shortRange; }

	const float GetHP() const { return hp; }
	const float GetFlinchValue() const { return flinchValue; }

	const Vector3 GetFrontVec();

	void SetFoundPlayer(const bool found) { foundPlayer = found; }
	void SetHP(const float hp) { this->hp = hp; }
	void SetFlinchValue(const float flinchValue) { this->flinchValue = flinchValue; }

	bool IsDown() { return flinchValue < 0.0f; }
	bool IsDead() { return hp < 0.0f; }


	void Initialize();
	void Finalize();
	void Update() override;
	void Render(bool isShadow = false) override;
	void DrawDebugGui();
	void DrawDebug();

	void Transform();
	void CollisionVSPlayer();
	void UpdateMove(float elapsedTime);
	void UpdateHorizontalVelocity(float elapsedTime, float elapsedFrame);
	void UpdateHorizontalMove(float elapsedTime, float elapsedFrame);
	void Move(const Vector3& vec, const float speed);
	bool SearchPlayer();
	void RotateToTargetVec(const DirectX::XMFLOAT3& targetVec, float t, const Vector3* tempFront = nullptr);
	void ClampPosition(float range);


	// --- エフェクト関連 ---
	void PlayRockEffect();
	void PlayFireBress(float angle);

private:
	// --- ビヘイビアツリー関連 ---
	void InitializeBehaviorTree();
	void FinalizeBehaviorTree();
	void UpdateBehaviorTree(float elapsedTime);
	void ShowNode(NodeBase<Enemy>* node, std::string nodeName);

	void OnDead();

	BehaviorTree<Enemy>* aiTree_;		// この中にツリーが伸びていく
	BehaviorData<Enemy>* behaviorData_;	// ビヘイビアのデータを保存する変数
	NodeBase<Enemy>*	 activeNode_;	// 実行するノード

	float searchRange_ = 75.0f;	// 探知範囲
	float wanderRange = 50.0f;	// 徘徊範囲
	bool foundPlayer;	// プレイヤーを見つけたか

	float longRange = 40.0f;	// 遠距離範囲
	float middleRange = 25.0f;	// 中距離範囲
	float shortRange = 15.0f;	// 近距離範囲

	// --- 突進関連 ---
	float rushSpeed = 15.0f;
	float rushEndSpeed = 4.0f;
	float rushChargeTimer = 1.25f;
	float whileRushTimer = 2.5f;

	Vector3 velocity;
	Vector3 moveVec;
	float acceleration = 3.0f;
	float friction = 0.2f;
	float maxSpeed = 0.0f;


	// --- ステータス関連 ---
	float hp;			// 体力
	float flinchValue;	// 怯み値

	bool alive;

public:
	Quaternion quaternion_;
	Vector3 moveTargetPosition_;
	float runTimer_;
	Vector3 targetVec;	// 回転する目標
	float turnAngle;

	float walkSpeed_ = 7.0f;
	float runSpeed_ = 10.0f;

	void DrawDebugImGui(int number);
};
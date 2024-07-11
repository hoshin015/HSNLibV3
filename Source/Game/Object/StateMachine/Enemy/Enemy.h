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


	// --- �G�t�F�N�g�֘A ---
	void PlayRockEffect();
	void PlayFireBress(float angle);

private:
	// --- �r�w�C�r�A�c���[�֘A ---
	void InitializeBehaviorTree();
	void FinalizeBehaviorTree();
	void UpdateBehaviorTree(float elapsedTime);
	void ShowNode(NodeBase<Enemy>* node, std::string nodeName);

	void OnDead();

	BehaviorTree<Enemy>* aiTree_;		// ���̒��Ƀc���[���L�тĂ���
	BehaviorData<Enemy>* behaviorData_;	// �r�w�C�r�A�̃f�[�^��ۑ�����ϐ�
	NodeBase<Enemy>*	 activeNode_;	// ���s����m�[�h

	float searchRange_ = 75.0f;	// �T�m�͈�
	float wanderRange = 50.0f;	// �p�j�͈�
	bool foundPlayer;	// �v���C���[����������

	float longRange = 40.0f;	// �������͈�
	float middleRange = 25.0f;	// �������͈�
	float shortRange = 15.0f;	// �ߋ����͈�

	// --- �ːi�֘A ---
	float rushSpeed = 15.0f;
	float rushEndSpeed = 4.0f;
	float rushChargeTimer = 1.25f;
	float whileRushTimer = 2.5f;

	Vector3 velocity;
	Vector3 moveVec;
	float acceleration = 3.0f;
	float friction = 0.2f;
	float maxSpeed = 0.0f;


	// --- �X�e�[�^�X�֘A ---
	float hp;			// �̗�
	float flinchValue;	// ���ݒl

	bool alive;

public:
	Quaternion quaternion_;
	Vector3 moveTargetPosition_;
	float runTimer_;
	Vector3 targetVec;	// ��]����ڕW
	float turnAngle;

	float walkSpeed_ = 7.0f;
	float runSpeed_ = 10.0f;

	void DrawDebugImGui(int number);
};
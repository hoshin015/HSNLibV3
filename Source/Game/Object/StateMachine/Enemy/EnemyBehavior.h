#pragma once

#include "../../../AI/BehaviorTree.h"

#include "Enemy.h"


enum class MonsterAnimation
{
	STAMP,
	BLESS,
	DOWN,
	WHILE_DOWN,
	ROAR,
	ROAR_BIG,
	IDLE,
	BITE_1,
	BITE_2,
	STAND_UP,
	DEAD,
	TACKLE,
	RUSH,
	WALK_LEFT,
	WALK_FOWARD,
	WALK_RIGHT,
	WALK_BEHIND,
};


// --- �ǐՍs���̔��� ---
class EnemyPursuitJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyPursuitJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};


// --- �ǐՍs�� ---
class EnemyPursuitAction final : public ActionBase<Enemy>
{
public:
	EnemyPursuitAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// --- �퓬�s���̔��� ---
class EnemyBattleJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyBattleJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};


// --- �U���̔��� ---
class EnemyAttackJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyAttackJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};



// --- �T���s���̔��� ---
class EnemyScoutJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyScoutJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};


// --- �p�j�s���̔��� ---
class EnemyWanderJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyWanderJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};


// --- �p�j�s�� ---
class EnemyWanderAction final : public ActionBase<Enemy>
{
public:
	EnemyWanderAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};


// --- �ҋ@�s�� ---
class EnemyIdleAction final : public ActionBase<Enemy>
{
public:
	EnemyIdleAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ==== �퓬�J�n���̍s������ ======================================================================================================================================================
class EnemyBattleInitJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyBattleInitJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};



// ===== �������̔��� ======================================================================================================================================================
class EnemyLongRangeJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyLongRangeJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};



// ===== �������̔��� ======================================================================================================================================================
class EnemyMiddleRangeJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyMiddleRangeJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};



// ===== �ߋ����̔��� ======================================================================================================================================================
class EnemyShortRangeJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyShortRangeJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};



// ===== ���݂̔��� ======================================================================================================================================================
class EnemyFlinchJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyFlinchJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};



// ===== ���S���� ======================================================================================================================================================
class EnemyDeadJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyDeadJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};



// ===== ����K�̍s�� ======================================================================================================================================================
class EnemyBigRoarAction final : public ActionBase<Enemy>
{
public:
	EnemyBigRoarAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== �����킹�̍s�� ======================================================================================================================================================
class EnemyAxisAlignmentAction final : public ActionBase<Enemy>
{
public:
	EnemyAxisAlignmentAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== �u���X�̍s�� ======================================================================================================================================================
class EnemyBlessAction final : public ActionBase<Enemy>
{
public:
	EnemyBlessAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== �Њd�s�� ======================================================================================================================================================
class EnemyThreatAction final : public ActionBase<Enemy>
{
public:
	EnemyThreatAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== �ːi�s�� ======================================================================================================================================================
class EnemyRushAction final : public ActionBase<Enemy>
{
public:
	EnemyRushAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== ���݂��s�� ======================================================================================================================================================
class EnemyStampAction final : public ActionBase<Enemy>
{
public:
	EnemyStampAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== ���݂��s�� ======================================================================================================================================================
class EnemyBiteAction final : public ActionBase<Enemy>
{
public:
	EnemyBiteAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== ���ݍ��݊��݂��s�� ======================================================================================================================================================
class EnemyRushingBiteAction final : public ActionBase<Enemy>
{
public:
	EnemyRushingBiteAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== ���ݍ��݊��݂���̍s�� ======================================================================================================================================================
class EnemyAfterRushingBiteAction final : public ActionBase<Enemy>
{
public:
	EnemyAfterRushingBiteAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== ���ݍs�� ======================================================================================================================================================
class EnemyFlinchAction final : public ActionBase<Enemy>
{
public:
	EnemyFlinchAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== ���S�s�� ======================================================================================================================================================
class EnemyDeadAction final : public ActionBase<Enemy>
{
public:
	EnemyDeadAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== �K����]�s�� ======================================================================================================================================================
class EnemyTailAttack final : public ActionBase<Enemy>
{
public:
	EnemyTailAttack(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};
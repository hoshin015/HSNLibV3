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
	ROTATION,
	BITE_1,
	BITE_2,
	STAND_UP,
	DEAD,
	SCOOP_UP,
	TACKLE_LEFT,
	TACKLE_RIGHT,
	TURN_LEFT,
	TURN_RIGHT,
	RUSH,
	WALK_LEFT,
	WALK_FOWARD,
	WALK_RIGHT,
	WALK_BEHIND,
};


class EnemyBaseBehavior : public ActionBase<Enemy>
{
public:
	EnemyBaseBehavior(Enemy* owner) : ActionBase(owner) {}
	BT_ActionState Run(float elapsedTime) override { return BT_ActionState::Complete; };

	bool IsInterrupted();
	void OnEndAction();	// �s�����I��������̌㏈��
	void ResetActionCount();
};


// --- �ǐՍs���̔��� ---
class EnemyPursuitJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyPursuitJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};


// --- �ǐՍs�� ---
class EnemyPursuitAction final : public EnemyBaseBehavior
{
public:
	EnemyPursuitAction(Enemy* owner) : EnemyBaseBehavior(owner) {}
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
class EnemyWanderAction final : public EnemyBaseBehavior
{
public:
	EnemyWanderAction(Enemy* owner) : EnemyBaseBehavior(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};


// --- �ҋ@�s�� ---
class EnemyIdleAction final : public EnemyBaseBehavior
{
public:
	EnemyIdleAction(Enemy* owner) : EnemyBaseBehavior(owner) {}
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



// ===== ���ʂ̔��� ======================================================================================================================================================
class EnemyFrontJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyFrontJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};



// ===== ����K�̔��� ======================================================================================================================================================
class EnemyBigRoarJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyBigRoarJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};








// ===== ����K�̍s�� ======================================================================================================================================================
class EnemyBigRoarAction final : public EnemyBaseBehavior
{
public:
	EnemyBigRoarAction(Enemy* owner) : EnemyBaseBehavior(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== �����킹�̍s�� ======================================================================================================================================================
class EnemyAxisAlignmentAction final : public EnemyBaseBehavior
{
public:
	EnemyAxisAlignmentAction(Enemy* owner) : EnemyBaseBehavior(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== �u���X�̍s�� ======================================================================================================================================================
class EnemyBlessAction final : public EnemyBaseBehavior
{
public:
	EnemyBlessAction(Enemy* owner) : EnemyBaseBehavior(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== �Њd�s�� ======================================================================================================================================================
class EnemyThreatAction final : public EnemyBaseBehavior
{
public:
	EnemyThreatAction(Enemy* owner) : EnemyBaseBehavior(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== �ːi�s�� ======================================================================================================================================================
class EnemyRushAction final : public EnemyBaseBehavior
{
public:
	EnemyRushAction(Enemy* owner) : EnemyBaseBehavior(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== ���݂��s�� ======================================================================================================================================================
class EnemyStampAction final : public EnemyBaseBehavior
{
public:
	EnemyStampAction(Enemy* owner) : EnemyBaseBehavior(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== ���݂��s�� ======================================================================================================================================================
class EnemyBiteAction final : public EnemyBaseBehavior
{
public:
	EnemyBiteAction(Enemy* owner) : EnemyBaseBehavior(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== ���ݍ��݊��݂��s�� ======================================================================================================================================================
class EnemyRushingBiteAction final : public EnemyBaseBehavior
{
public:
	EnemyRushingBiteAction(Enemy* owner) : EnemyBaseBehavior(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== ���ݍ��݊��݂���̍s�� ======================================================================================================================================================
class EnemyAfterRushingBiteAction final : public EnemyBaseBehavior
{
public:
	EnemyAfterRushingBiteAction(Enemy* owner) : EnemyBaseBehavior(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== ���ݍs�� ======================================================================================================================================================
class EnemyFlinchAction final : public EnemyBaseBehavior
{
public:
	EnemyFlinchAction(Enemy* owner) : EnemyBaseBehavior(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== ���S�s�� ======================================================================================================================================================
class EnemyDeadAction final : public EnemyBaseBehavior
{
public:
	EnemyDeadAction(Enemy* owner) : EnemyBaseBehavior(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== �K����]�s�� ======================================================================================================================================================
class EnemyTailAttack final : public EnemyBaseBehavior
{
public:
	EnemyTailAttack(Enemy* owner) : EnemyBaseBehavior(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== �d���グ�s�� ======================================================================================================================================================
class EnemyScoopUpAction final : public EnemyBaseBehavior
{
public:
	EnemyScoopUpAction(Enemy* owner) : EnemyBaseBehavior(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};
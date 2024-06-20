#pragma once

#include "../../../AI/BehaviorTree.h"

#include "Enemy.h"


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


// --- �U���s�� ---
class EnemyAttackAction final : public ActionBase<Enemy>
{
public:
	EnemyAttackAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};


// --- �U���s�� ---
class EnemyAttackAction2 final : public ActionBase<Enemy>
{
public:
	EnemyAttackAction2(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
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

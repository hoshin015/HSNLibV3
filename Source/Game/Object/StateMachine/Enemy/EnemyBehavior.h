#pragma once

#include "../../../AI/BehaviorTree.h"

#include "Enemy.h"


// --- ’ÇÕs“®‚Ì”»’è ---
class EnemyPursuitJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyPursuitJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};


// --- ’ÇÕs“® ---
class EnemyPursuitAction final : public ActionBase<Enemy>
{
public:
	EnemyPursuitAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// --- í“¬s“®‚Ì”»’è ---
class EnemyBattleJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyBattleJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};


// --- UŒ‚‚Ì”»’è ---
class EnemyAttackJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyAttackJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};


// --- UŒ‚s“® ---
class EnemyAttackAction final : public ActionBase<Enemy>
{
public:
	EnemyAttackAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};


// --- UŒ‚s“® ---
class EnemyAttackAction2 final : public ActionBase<Enemy>
{
public:
	EnemyAttackAction2(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// --- ’Tõs“®‚Ì”»’è ---
class EnemyScoutJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyScoutJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};


// --- œpœjs“®‚Ì”»’è ---
class EnemyWanderJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyWanderJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};


// --- œpœjs“® ---
class EnemyWanderAction final : public ActionBase<Enemy>
{
public:
	EnemyWanderAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};


// --- ‘Ò‹@s“® ---
class EnemyIdleAction final : public ActionBase<Enemy>
{
public:
	EnemyIdleAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};

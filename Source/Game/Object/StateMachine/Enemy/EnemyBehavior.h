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


// --- 追跡行動の判定 ---
class EnemyPursuitJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyPursuitJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};


// --- 追跡行動 ---
class EnemyPursuitAction final : public ActionBase<Enemy>
{
public:
	EnemyPursuitAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// --- 戦闘行動の判定 ---
class EnemyBattleJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyBattleJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};


// --- 攻撃の判定 ---
class EnemyAttackJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyAttackJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};



// --- 探索行動の判定 ---
class EnemyScoutJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyScoutJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};


// --- 徘徊行動の判定 ---
class EnemyWanderJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyWanderJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};


// --- 徘徊行動 ---
class EnemyWanderAction final : public ActionBase<Enemy>
{
public:
	EnemyWanderAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};


// --- 待機行動 ---
class EnemyIdleAction final : public ActionBase<Enemy>
{
public:
	EnemyIdleAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ==== 戦闘開始時の行動判定 ======================================================================================================================================================
class EnemyBattleInitJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyBattleInitJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};



// ===== 遠距離の判定 ======================================================================================================================================================
class EnemyLongRangeJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyLongRangeJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};



// ===== 中距離の判定 ======================================================================================================================================================
class EnemyMiddleRangeJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyMiddleRangeJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};



// ===== 近距離の判定 ======================================================================================================================================================
class EnemyShortRangeJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyShortRangeJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};



// ===== 怯みの判定 ======================================================================================================================================================
class EnemyFlinchJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyFlinchJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};



// ===== 死亡判定 ======================================================================================================================================================
class EnemyDeadJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyDeadJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};



// ===== 大咆哮の行動 ======================================================================================================================================================
class EnemyBigRoarAction final : public ActionBase<Enemy>
{
public:
	EnemyBigRoarAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== 軸合わせの行動 ======================================================================================================================================================
class EnemyAxisAlignmentAction final : public ActionBase<Enemy>
{
public:
	EnemyAxisAlignmentAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== ブレスの行動 ======================================================================================================================================================
class EnemyBlessAction final : public ActionBase<Enemy>
{
public:
	EnemyBlessAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== 威嚇行動 ======================================================================================================================================================
class EnemyThreatAction final : public ActionBase<Enemy>
{
public:
	EnemyThreatAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== 突進行動 ======================================================================================================================================================
class EnemyRushAction final : public ActionBase<Enemy>
{
public:
	EnemyRushAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== 踏みつけ行動 ======================================================================================================================================================
class EnemyStampAction final : public ActionBase<Enemy>
{
public:
	EnemyStampAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== 噛みつき行動 ======================================================================================================================================================
class EnemyBiteAction final : public ActionBase<Enemy>
{
public:
	EnemyBiteAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== 踏み込み噛みつき行動 ======================================================================================================================================================
class EnemyRushingBiteAction final : public ActionBase<Enemy>
{
public:
	EnemyRushingBiteAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== 踏み込み噛みつき後の行動 ======================================================================================================================================================
class EnemyAfterRushingBiteAction final : public ActionBase<Enemy>
{
public:
	EnemyAfterRushingBiteAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== 怯み行動 ======================================================================================================================================================
class EnemyFlinchAction final : public ActionBase<Enemy>
{
public:
	EnemyFlinchAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== 死亡行動 ======================================================================================================================================================
class EnemyDeadAction final : public ActionBase<Enemy>
{
public:
	EnemyDeadAction(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== 尻尾回転行動 ======================================================================================================================================================
class EnemyTailAttack final : public ActionBase<Enemy>
{
public:
	EnemyTailAttack(Enemy* owner) : ActionBase<Enemy>(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};
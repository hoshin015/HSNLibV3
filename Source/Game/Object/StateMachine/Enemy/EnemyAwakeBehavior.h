#pragma once

#include "EnemyBehavior.h"

// ===== �o���O�̔��� ======================================================================================================================================================
class EnemyBeforeAwakeJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyBeforeAwakeJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};



// ===== ���K�̗��U�� ======================================================================================================================================================
class EnemyLightningRoarAction final : public EnemyBaseBehavior
{
public:
	EnemyLightningRoarAction(Enemy* owner) : EnemyBaseBehavior(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== ����K�̗��U�� ======================================================================================================================================================
class EnemyLightningBigRoarAction final : public EnemyBaseBehavior
{
public:
	EnemyLightningBigRoarAction(Enemy* owner) : EnemyBaseBehavior(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== �����݂̗��U�� ======================================================================================================================================================
class EnemyLightningStampAction final : public EnemyBaseBehavior
{
public:
	EnemyLightningStampAction(Enemy* owner) : EnemyBaseBehavior(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== �K����]���t�� ======================================================================================================================================================
class EnemyLightningTailAction final : public EnemyBaseBehavior
{
public:
	EnemyLightningTailAction(Enemy* owner) : EnemyBaseBehavior(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== ���ːi ======================================================================================================================================================
class EnemyLightningRushAction final : public EnemyBaseBehavior
{
public:
	EnemyLightningRushAction(Enemy* owner) : EnemyBaseBehavior(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== ���d���グ ======================================================================================================================================================
class EnemyLightningScoopUpAction final : public EnemyBaseBehavior
{
public:
	EnemyLightningScoopUpAction(Enemy* owner) : EnemyBaseBehavior(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== �^�b�N�� ======================================================================================================================================================
class EnemyLightningTackleAction final : public EnemyBaseBehavior
{
public:
	EnemyLightningTackleAction(Enemy* owner) : EnemyBaseBehavior(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};
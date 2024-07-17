#pragma once

#include "EnemyBehavior.h"

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
#pragma once

#include "EnemyBehavior.h"

// ===== ™ôšK‚Ì—‹UŒ‚ ======================================================================================================================================================
class EnemyLightningRoarAction final : public EnemyBaseBehavior
{
public:
	EnemyLightningRoarAction(Enemy* owner) : EnemyBaseBehavior(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== ‘å™ôšK‚Ì—‹UŒ‚ ======================================================================================================================================================
class EnemyLightningBigRoarAction final : public EnemyBaseBehavior
{
public:
	EnemyLightningBigRoarAction(Enemy* owner) : EnemyBaseBehavior(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};
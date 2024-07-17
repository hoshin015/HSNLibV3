#pragma once

#include "EnemyBehavior.h"

// ===== ŠoÁ‘O‚Ì”»’è ======================================================================================================================================================
class EnemyBeforeAwakeJudgment final : public JudgmentBase<Enemy>
{
public:
	EnemyBeforeAwakeJudgment(Enemy* owner) : JudgmentBase(owner) {}
	bool Judgment() override;
};



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



// ===== ‘«“¥‚İ‚Ì—‹UŒ‚ ======================================================================================================================================================
class EnemyLightningStampAction final : public EnemyBaseBehavior
{
public:
	EnemyLightningStampAction(Enemy* owner) : EnemyBaseBehavior(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== K”ö‰ñ“]—‹•t‚« ======================================================================================================================================================
class EnemyLightningTailAction final : public EnemyBaseBehavior
{
public:
	EnemyLightningTailAction(Enemy* owner) : EnemyBaseBehavior(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== —‹“Ëi ======================================================================================================================================================
class EnemyLightningRushAction final : public EnemyBaseBehavior
{
public:
	EnemyLightningRushAction(Enemy* owner) : EnemyBaseBehavior(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== —‹‹d‚¢ã‚° ======================================================================================================================================================
class EnemyLightningScoopUpAction final : public EnemyBaseBehavior
{
public:
	EnemyLightningScoopUpAction(Enemy* owner) : EnemyBaseBehavior(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};



// ===== ƒ^ƒbƒNƒ‹ ======================================================================================================================================================
class EnemyLightningTackleAction final : public EnemyBaseBehavior
{
public:
	EnemyLightningTackleAction(Enemy* owner) : EnemyBaseBehavior(owner) {}
	BT_ActionState Run(float elapsedTime) override;
};
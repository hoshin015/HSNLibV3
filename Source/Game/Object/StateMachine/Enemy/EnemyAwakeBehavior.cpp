#include "EnemyAwakeBehavior.h"
#include "../../Effect/Lightning/LightningEffect.h"
#include "../Player/Player.h"
#include "../../../../../Library/Timer.h"


// ===== ™ôšK‚Ì—‹UŒ‚ ======================================================================================================================================================
BT_ActionState EnemyLightningRoarAction::Run(float elapsedTime)
{
	// --- ƒ_ƒEƒ“/Ž€–Sˆ— ---
	if (IsInterrupted())
		return BT_ActionState::Failed;

	switch (step)
	{
	case 0:
	{
		owner_->GetAnimator().SetNextState("hoeru");
		owner_->runTimer_ = 0.5f;
		step++;
		break;
	}

	
	case 1:
	{
		owner_->runTimer_ -= Timer::Instance().DeltaTime();
		if(owner_->runTimer_ < 0.0f)
		{
			LightningEffect::Instance().Emit(Player::Instance().GetPos());
			step++;
		}

		break;
	}


	case 2:
	{
		if (owner_->GetAnimator().GetEndMotion())
		{
			OnEndAction();
			return BT_ActionState::Complete;
		}

		break;
	}

	}

	return BT_ActionState::Run;
}



// ===== ‘å™ôšK‚Ì—‹UŒ‚ ======================================================================================================================================================
BT_ActionState EnemyLightningBigRoarAction::Run(float elapsedTime)
{
	// --- ƒ_ƒEƒ“/Ž€–Sˆ— ---
	if (IsInterrupted())
		return BT_ActionState::Failed;

	switch (step)
	{
	case 0:
	{
		owner_->GetAnimator().SetNextState("hoeru_big");
		owner_->runTimer_ = 0.2f;
		step++;
		break;
	}


	case 1:
	{
		owner_->runTimer_ -= Timer::Instance().DeltaTime();
		if (owner_->runTimer_ < 0.0f)
		{
			owner_->runTimer_ = 0.2f;
			float range = static_cast<float>(rand() % 50) + 10.0f;
			float theta = DirectX::XMConvertToRadians(static_cast<float>(rand() % 360));
			Vector3 position = Vector3(owner_->GetPos()) + Vector3(cosf(theta) * range, 0.0f, sinf(theta) * range);
			LightningEffect::Instance().Emit(position.vec_);
		}

		if (owner_->GetAnimator().GetEndMotion())
		{
			OnEndAction();
			return BT_ActionState::Complete;
		}

		break;
	}

	}

	return BT_ActionState::Run;
}

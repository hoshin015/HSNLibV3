#include "PlayerState.h"
#include "Player.h"
#include "../../../../../Library/Math/OperatorXMFloat3.h"
#include "../../../../../Library/Input/InputManager.h"

///////////////////////////
///
///		‚P‘w–Ú
///
///////////////////////////


void PlayerNormalState::Enter()
{
	SetSubState(static_cast<int>(Player::Normal::Idle));
}


void PlayerNormalState::Execute()
{
	currentSubState->Execute();
}


void PlayerNormalState::Exit()
{
}


///////////////////////////
///
///		‚Q‘w–Ú
///
///////////////////////////


// ========================================================
// 	PlayerIdle
// ========================================================

void PlayerIdleState::Enter()
{
	owner->SetVelocity({ 0,0,0 });

	owner->SetBeforeAnimationIndex(owner->GetCurrentAnimationIndex());
	owner->SetAnimationBlendTimer(0.0f);
	owner->SetAnimationBlendTransitionDurations(0.2f);
	owner->PlayAnimation(static_cast<int>(PlayerAnimNum::Idle), true);
}

void PlayerIdleState::Execute()
{
	owner->Input();

	// ‘–‚èˆÚ“®—ÊŒvŽZ
	owner->CalcWalkVelocity();

	if(Float3Length(owner->GetVelocity()) >= 0.001f)
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Walk));
		if (owner->GetInputMap<bool>("Run"))
		{
			owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Run));
		}
	}
	if (owner->GetInputMap<bool>("Attack"))
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Attack));
	}
	if (owner->GetInputMap<bool>("Drink"))
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Drink));
	}

	if (owner->GetInputMap<bool>("Dodge"))
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Dodge));

	// ‰ñ“]
	owner->Turn();

	// ˆÚ“®
	owner->Move();
}

void PlayerIdleState::Exit()
{
}

// ========================================================
// 	PlayerWalk
// ========================================================

void PlayerWalkState::Enter()
{
	owner->SetBeforeAnimationIndex(owner->GetCurrentAnimationIndex());
	owner->SetAnimationBlendTimer(0.0f);
	owner->SetAnimationBlendTransitionDurations(0.2f);
	owner->PlayAnimation(static_cast<int>(PlayerAnimNum::Walk), true);
}

void PlayerWalkState::Execute()
{
	owner->Input();

	// •àsˆÚ“®—ÊŒvŽZ
	owner->CalcWalkVelocity();

	if (owner->GetInputMap<bool>("Run"))
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Run));
	}
	if (Float3Length(owner->GetVelocity()) < 0.001f)
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Idle));
	}
	if(owner->GetInputMap<bool>("Attack"))
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Attack));
	}
	if (owner->GetInputMap<bool>("Drink"))
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Drink));
	}

	if (owner->GetInputMap<bool>("Dodge"))
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Dodge));

	// ‰ñ“]
	owner->Turn();

	// ˆÚ“®
	owner->Move();
}

void PlayerWalkState::Exit()
{
}

// ========================================================
// 	PlayerRun
// ========================================================

void PlayerRunState::Enter()
{
	owner->SetBeforeAnimationIndex(owner->GetCurrentAnimationIndex());
	owner->SetAnimationBlendTimer(0.0f);
	owner->SetAnimationBlendTransitionDurations(0.2f);
	owner->PlayAnimation(static_cast<int>(PlayerAnimNum::Run), true);
}

void PlayerRunState::Execute()
{
	owner->Input();

	// ‘–‚èˆÚ“®—ÊŒvŽZ
	owner->CalcRunVelocity();

	if (!owner->GetInputMap<bool>("Run"))
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Walk));
	}
	if (Float3Length(owner->GetVelocity()) < 0.001f)
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Idle));
	}
	if (owner->GetInputMap<bool>("Attack"))
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Attack));
	}
	if (owner->GetInputMap<bool>("Drink"))
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Drink));
	}

	if (owner->GetInputMap<bool>("Dodge"))
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Dodge));

	// ‰ñ“]
	owner->Turn();

	// ˆÚ“®
	owner->Move();
}

void PlayerRunState::Exit()
{
}

// ========================================================
// 	PlayerAttack
// ========================================================

void PlayerAttackState::Enter()
{
	//owner->PlayAnimation(static_cast<int>(PlayerAnimNum::Attack), false);
	//owner->SetInputMap("Attack", false);
	//owner->GetAnimator().SetParameter("attack", false);
	owner->AStatus().attackTimer = owner->CStatus().attackReceptionTime;
	owner->AStatus().notAcceptTimer = owner->CStatus().notAcceptTime;
}

void PlayerAttackState::Execute()
{
	owner->InputAttack();

	if(owner->GetInputMap<bool>("EndAttack"))
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Idle));
	}

	owner->CalcAttackVelocity();
	owner->CalcRootAnimationVelocity();
	owner->Move();
}

void PlayerAttackState::Exit()
{
}

// ========================================================
// 	PlayerDrink
// ========================================================

void PlayerDrinkState::Enter()
{
	owner->SetDoubleCurrentAnimationIndex(static_cast<int>(PlayerAnimNum::Drink));
	owner->SetIsDoubleAnimation(true);
}

void PlayerDrinkState::Execute()
{
	// •àsˆÚ“®—ÊŒvŽZ
	owner->CalcWalkVelocity();

	if (Float3Length(owner->GetVelocity()) < 0.001f)
	{
		owner->PlayAnimation(static_cast<int>(PlayerAnimNum::Idle), true);
	}
	else
	{
		owner->PlayAnimation(static_cast<int>(PlayerAnimNum::Walk), true);
	}


	if (!owner->GetIsDoubleAnimation())
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Idle));
	}

	// ‰ñ“]
	owner->Turn();

	// ˆÚ“®
	owner->Move();
}

void PlayerDrinkState::Exit()
{
}

void PlayerDodgeState::Enter() {
	owner->AStatus().dodgeTimer = owner->CStatus().dodgeTime;
	owner->GetAnimator().SetParameter("endDodge", false);
}

void PlayerDodgeState::Execute() {
	owner->Input();

	owner->CalcDodgeVelocity();

	Player::ConstantStatus& cs = owner->CStatus();
	const float dodgeLowTimer = cs.dodgeTime - cs.dodgeLowestTime;

	if ((owner->AStatus().dodgeTimer < dodgeLowTimer&& !owner->GetInputMap<bool>("DodgeHold"))||
		owner->AStatus().dodgeTimer < 0) {
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Idle));
		if (Vector2 move = owner->GetInputMap<DirectX::XMFLOAT2>("Move");
			move.Length() > owner->CStatus().dashDeadZone) {
			owner->SetInputMap("Run", true);
			owner->GetAnimator().SetParameter("run", true);
		}
		owner->GetAnimator().SetParameter("endDodge", true);
	}

	// ‰ñ“]
	//owner->Turn();

	// ˆÚ“®
	owner->Move();
}

void PlayerDodgeState::Exit() {
}

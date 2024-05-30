#include "PlayerState.h"
#include "Player.h"
#include "../../../../../Library/Math/OperatorXMFloat3.h"

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
	owner->PlayAnimation(static_cast<int>(PlayerAnimNum::Idle), true);
}

void PlayerIdleState::Execute()
{
	if(Float3Length(owner->GetVelocity()) >= 0.001f)
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Walk));
	}
}

void PlayerIdleState::Exit()
{
}

// ========================================================
// 	PlayerWalk
// ========================================================

void PlayerWalkState::Enter()
{
	owner->PlayAnimation(static_cast<int>(PlayerAnimNum::Walk), true);
}

void PlayerWalkState::Execute()
{
	if (Float3Length(owner->GetVelocity()) < 0.001f)
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Idle));
	}
}

void PlayerWalkState::Exit()
{
}

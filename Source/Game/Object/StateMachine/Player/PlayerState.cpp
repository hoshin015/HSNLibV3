#include "PlayerState.h"
#include "Player.h"
#include "../../../../../Library/Timer.h"
#include "../../../../../Library/3D/CameraManager.h"
#include "../../../../../Library/Math/OperatorXMFloat3.h"
#include "../../../../../Library/Input/InputManager.h"
#include "../../../../../Library/Particle/EmitterManager.h"
#include "../../../../UserInterface/UiGame.h"

///////////////////////////
///
///		�P�w��
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
///		�Q�w��
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

	// ����ړ��ʌv�Z
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

	if (owner->AStatus().isHitDamage)
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Damage));

	// ��]
	owner->Turn();

	// �ړ�
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

	// ���s�ړ��ʌv�Z
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

	if (owner->AStatus().isHitDamage)
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Damage));

	// ��]
	owner->Turn();

	// �ړ�
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

	// ����ړ��ʌv�Z
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

	if (owner->AStatus().isHitDamage)
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Damage));

	// ��]
	owner->Turn();

	// �ړ�
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
	//owner->GetAnimator().SetParameter("attack", true);
	owner->GetAnimator().SetNextState("attack1");
}

void PlayerAttackState::Execute()
{
	owner->InputAttack();

	if(owner->GetInputMap<bool>("EndAttack"))
	{
		owner->swordTrail->Clear();
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Idle));
	}
	if (owner->GetInputMap<bool>("Dodge"))
	{
		owner->swordTrail->Clear();
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Dodge));
	}

	
	owner->swordTrail->SetSwordPos(owner->GetBonePosition("sword_05"), owner->GetBonePosition("sword_01"));
	

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
	// ���s�ړ��ʌv�Z
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

	// ��]
	owner->Turn();

	// �ړ�
	owner->Move();
}

void PlayerDrinkState::Exit()
{
}

void PlayerDodgeState::Enter() {
	owner->AStatus().dodgeTimer = owner->CStatus().dodgeTime;
	owner->GetAnimator().SetParameter("endDodge", false);
	owner->GetAnimator().SetNextState("dodge");
}

void PlayerDodgeState::Execute() {
	owner->Input();

	owner->CalcDodgeVelocity();

	Player::ConstantStatus& cs = owner->CStatus();
	const float dodgeLowTimer = cs.dodgeTime - cs.dodgeLowestTime;
	const float justDodgeTimer = cs.dodgeTime - cs.justDodgeTime;
	const float dodgeInvincibleTime = cs.dodgeTime - cs.dodgeInvincibleTime;

	if ((owner->AStatus().dodgeTimer < dodgeLowTimer&& !owner->GetInputMap<bool>("DodgeHold"))||
		owner->AStatus().dodgeTimer < 0) {
		owner->AStatus().isHitDamage = false;
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Idle));
		if (Vector2 move = owner->GetInputMap<DirectX::XMFLOAT2>("Move");
			move.Length() > owner->CStatus().dashDeadZone) {
			owner->SetInputMap("Run", true);
			owner->GetAnimator().SetParameter("run", true);
		}
		owner->GetAnimator().SetParameter("endDodge", true);
	}

	// �A�j���[�V�������~�܂��Ă��Ȃ��Ȃ�
	//if(!owner->GetAnimator().GetEndMotion())
	{
		Emitter* emitter0 = new Emitter();
		emitter0->position = owner->GetPos();
		emitter0->emitterData.duration = 10.0;
		emitter0->emitterData.looping = false;
		emitter0->emitterData.burstsTime = 0.05;
		emitter0->emitterData.burstsCount = 2;
		emitter0->emitterData.particleKind = pk_simpleFire;
		emitter0->emitterData.particleLifeTimeMin = 0.5f;
		emitter0->emitterData.particleLifeTimeMax = 1.0f;
		emitter0->emitterData.particleSpeedMin = 1.0f;
		emitter0->emitterData.particleSpeedMax = 1.0f;
		emitter0->emitterData.particleSizeMin = { 0.25f, 1.0f };
		emitter0->emitterData.particleSizeMax = { 1.0f, 2.0f };
		emitter0->emitterData.particleColorMin = { 1.0, 3.0, 15.0, 1 };
		emitter0->emitterData.particleColorMax = { 1.0, 3.0, 15.0, 1 };
		emitter0->emitterData.particleGravity = 0;
		emitter0->emitterData.particleBillboardType = 0;
		emitter0->emitterData.particleTextureType = 0;
		emitter0->emitterData.burstsOneShot = 2;
		EmitterManager::Instance().Register(emitter0);
	}

	if (owner->AStatus().dodgeTimer <= cs.justDodgeTime && owner->AStatus().isHitDamage)
		owner->AStatus().isJustDodge = true;

	if(owner->AStatus().isHitDamage&&owner->AStatus().isInvincibleInvalidDamage)
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Damage));

	if (owner->AStatus().dodgeTimer < cs.dodgeInvincibleTime) {
		if (owner->AStatus().isHitDamage)owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Damage));
	}
	else owner->AStatus().isHitDamage = false;

	if(owner->GetInputMap<bool>("Attack")) {
		owner->GetAnimator().SetParameter("endDodge",true);
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Attack));
	}
	// ��]
	//owner->Turn();

	// �ړ�
	owner->Move();
}

void PlayerDodgeState::Exit() {
}

void PlayerDamageState::Enter() {
	Player::AbilityStatus& as = owner->AStatus();
	as.hp -= as.hitDamage;

	if (as.hp > 0)
		as.isFlying ?
		owner->GetAnimator().SetNextState("down") :
		owner->GetAnimator().SetNextState("hit");
	else {
		as.hp = 0;
		owner->GetAnimator().SetNextState("death");
		CameraManager::Instance().SetCurrentCamera("PlayerDeadCamera");
		UiGame::Instance().OnDown();
	}
}

void PlayerDamageState::Execute() {
	using namespace DirectX;
	Player::AbilityStatus& as = owner->AStatus();
	as.isHitDamage = false;

	const Animator::State* current = owner->GetAnimator().GetCurrentState();
	if (owner->AStatus().hp<=0) {
		const Animator::State* rise = &owner->GetAnimator().GetState("rise");
		if (owner->GetAnimator().GetEndMotion() && !CameraManager::Instance().IsCurrentCamera("PlayerDeadCamera")) {
			if(current != rise)owner->GetAnimator().SetNextState("rise");
			else {
				owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Idle));
				owner->GetAnimator().SetNextState("walk");
			}
		}
	}
	else if (owner->GetAnimator().GetEndMotion()) {
		if(current == &owner->GetAnimator().GetState("down")) {
			owner->GetAnimator().SetNextState("wakeUp");
		}
		else {
			owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Normal::Idle));
				owner->GetAnimator().SetNextState("walk");
		}
	}

	if (owner->AStatus().isFlying && current == &owner->GetAnimator().GetState("down")) {
		Vector2 vel;
		vel.x = owner->GetVelocity().x;
		vel.y = owner->GetVelocity().z;
		vel += owner->AStatus().flyVec.xz() * Timer::Instance().DeltaTime();
		owner->SetVelocity({vel.x,0, vel.y});

		vel.Normalize();
		float ang = XMConvertToDegrees(atan2(-vel.x,-vel.y));

		owner->SetAngleY(ang);
		owner->Move();
	}
}

void PlayerDamageState::Exit() {

}

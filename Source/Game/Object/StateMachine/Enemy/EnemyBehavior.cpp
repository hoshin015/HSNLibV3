#include "EnemyBehavior.h"

#include "../../../../../Library/Graphics/RadialBlur.h"
#include "../../Library/Math/Math.h"

#include "../../Effect/Breath/BreathEffect.h"

#include "../Player/Player.h"
#include "../../../../../Library/Timer.h"
#include "../../../../../Library/3D/CameraManager.h"
#include "../../Effect/Lightning/LightningEffect.h"
#include "../../Effect/Rock/RockEffect.h"
#include "../../Effect/Special/SpecialEffect.h"



// ===== �G���ʂ̏��� ======================================================================================================================================================
bool EnemyBaseBehavior::IsInterrupted()
{
	if (owner_->IsDown() || owner_->IsDead())
		return true;

	return false;
}


bool EnemyBaseBehavior::WasAttacked()
{
	return owner_->wasAttacked;
}


void EnemyBaseBehavior::OnEndAction()
{
	step = 0;
	owner_->runTimer_ = 0.0f;

	if (owner_->IsFoundPlayer())
		owner_->actionCount++;	// �s���񐔂����Z

	// �����蔻��̃t���O������������
	owner_->ClearAnimSphereCollisionDamagedFlag();
	owner_->ClearSeFlag();
}


void EnemyBaseBehavior::ResetActionCount()
{
	owner_->roarNeededActionCount = 25 + (rand() % 10) - 5;
	owner_->actionCount = 0;
}




// --- �ǐՍs���̔��� ---
bool EnemyPursuitJudgment::Judgment()
{
	DirectX::XMFLOAT3 playerPosition = Player::Instance().GetPos();
	DirectX::XMFLOAT3 enemyPosition = owner_->GetPos();

	// --- �v���C���[�Ƃ̋������Ƃ� ---
	float length = Math::XMFloat3Length(playerPosition, enemyPosition);


	// --- ���������ȏ�Ȃ�ǐ� ---
	if (length > 10.0f)
		return true;

	else
		return false;
}


// --- �ǐՍs�� ---
BT_ActionState EnemyPursuitAction::Run(float elapsedTime)
{
	// --- �_�E��/���S���� ---
	if (IsInterrupted() || WasAttacked())
		return BT_ActionState::Failed;

	switch (step)
	{
		// --- �����ݒ� ---
	case 0:

		//owner_->PlayAnimation(static_cast<int>(MonsterAnimation::WALK_FOWARD), true);	// �A�j���[�V�����̐ݒ�
		owner_->GetAnimator().SetNextState("walk_mae");
		owner_->runTimer_ = 5.0f;

		step++;
		break;


		// --- ���C������ ---
	case 1:
		DirectX::XMFLOAT3 playerPosition = Player::Instance().GetPos();
		DirectX::XMFLOAT3 enemyPosition = owner_->GetPos();
		float length = Math::XMFloat3Length(playerPosition, enemyPosition);


		// --- �v���C���[�ւ̃x�N�g�����Ƃ� ---
		DirectX::XMFLOAT3 vec = playerPosition - enemyPosition;
		vec = Math::XMFloat3Normalize(vec);
		vec.y = 0.0f;

		// --- �x�N�g���̕����ɉ�] ---
		owner_->RotateToTargetVec(vec, 0.3f);

		// --- �ʒu�̍X�V ---
		const float moveSpeed = owner_->runSpeed_ * elapsedTime;
		DirectX::XMFLOAT3 moveVec = vec * moveSpeed;
		owner_->Move(moveVec, owner_->runSpeed_);

		owner_->runTimer_ -= elapsedTime;
		if (owner_->runTimer_ < 0.0f)
		{
			OnEndAction();
			return BT_ActionState::Failed;
		}


		// --- �v���C���[�ɋ߂Â����� ---
		if (length < 10.0f)
		{
			OnEndAction();
			return BT_ActionState::Complete;    // �ǐՊ���
		}

		break;
	}

	return BT_ActionState::Run; // �ǐՌp��
}



// --- �퓬��Ԃ̔��� ---
bool EnemyBattleJudgment::Judgment()
{
	return owner_->IsFoundPlayer();
}

// --- �U���s���̔��� ---
bool EnemyAttackJudgment::Judgment()
{
	DirectX::XMFLOAT3 playerPosition = Player::Instance().GetPos();
	DirectX::XMFLOAT3 enemyPosition = owner_->GetPos();

	// --- �v���C���[�Ƃ̋������Ƃ� ---
	float length = Math::XMFloat3Length(playerPosition, enemyPosition);


	// --- ���������ȉ��Ȃ�U�� ---
	if (length < owner_->GetShortRange())
		return true;

	else
		return false;
}



// --- �T���s���̔��� ---
bool EnemyScoutJudgment::Judgment()
{
	return !owner_->IsFoundPlayer();
}


// --- �p�j�s���̔��� ---
bool EnemyWanderJudgment::Judgment()
{
	// --- �ڕW�ւ̃x�N�g�� ---
	Vector3 moveVec = owner_->moveTargetPosition_ - owner_->GetPos();

	// --- �ڕW��艓�������� ---
	float length = moveVec.Length();
	if (length > 1.0f)
	{
		return true;
	}

	return false;
}

// --- �p�j�s�� ---
BT_ActionState EnemyWanderAction::Run(float elapsedTime)
{
	// --- �_�E��/���S���� ---
	if (IsInterrupted() || WasAttacked())
		return BT_ActionState::Failed;



	switch (step)
	{
		// --- �����ݒ� ---
	case 0:

		//owner_->PlayAnimation(static_cast<int>(MonsterAnimation::WALK_FOWARD), true);
		owner_->GetAnimator().SetNextState("walk_mae");

		step++;
		break;


		// --- ���C������ ---
	case 1:

		// --- �ڕW�Ɍ������Ĉړ� ---
		Vector3 position = owner_->GetPos();
		Vector3 moveVec = owner_->moveTargetPosition_ - position;

		// --- �ڕW�Ƃ̋������߂��Ȃ����� ---
		if (moveVec.Length() < 1.0f)
		{
			OnEndAction();
			return BT_ActionState::Complete;
		}


		// --- �ʒu�̍X�V ---
		moveVec.Normalize();
		owner_->RotateToTargetVec(moveVec.vec_, 0.3f);
		moveVec *= owner_->walkSpeed_ * elapsedTime;
		owner_->Move(moveVec, owner_->walkSpeed_);


		// --- �v���C���[���������� ---
		if (owner_->SearchPlayer())
		{
			OnEndAction();
			return BT_ActionState::Complete;
		}


		break;
	}

	return BT_ActionState::Run;
}


// --- �ҋ@�s�� ���̈ړ��n�_�����߂� ---
BT_ActionState EnemyIdleAction::Run(float elapsedTime)
{
	// --- �_�E��/���S���� ---
	if (IsInterrupted())
		return BT_ActionState::Failed;



	switch (step)
	{
		// --- �����ݒ� ---
	case 0:

		owner_->runTimer_ = (rand() / FLT_MAX) * 2.0f + 3.0f;	// 3.0 ~ 5.0
		//owner_->PlayAnimation(static_cast<int>(MonsterAnimation::IDLE), true);
		owner_->GetAnimator().SetNextState("idle");

		step++;
		break;


		// --- ���C������ ---
	case 1:

		owner_->runTimer_ -= elapsedTime;

		// --- �ҋ@���Ԃ��߂����� ---
		if (owner_->runTimer_ < 0.0f)
		{
			float theta = DirectX::XMConvertToRadians(rand() % 360);
			float wanderRange = owner_->GetWanderRange();
			owner_->moveTargetPosition_ = { cosf(theta) * wanderRange, 0.0f, sinf(theta) * wanderRange };

			OnEndAction();
			return BT_ActionState::Complete;
		}


		// --- �v���C���[���������� ---
		if (owner_->SearchPlayer())
		{
			OnEndAction();
			return BT_ActionState::Complete;
		}

		break;
	}
	return BT_ActionState::Run;
}



// ==== �퓬�J�n���̍s������ ======================================================================================================================================================
bool EnemyBattleInitJudgment::Judgment()
{
	// --- ���łɌ����Ă����� ---
	if (owner_->IsFoundPlayer())
		return false;


	// --- �v���C���[���������琬�� ---
	if (owner_->SearchPlayer() || owner_->wasAttacked)
	{
		return true;
	}

	return false;
}



// ===== �������̔��� ======================================================================================================================================================
bool EnemyLongRangeJudgment::Judgment()
{
	Vector3 playerPosition = Player::Instance().GetPos();
	Vector3 enemyPosition = owner_->GetPos();

	// --- �v���C���[�Ƃ̋������Ƃ� ---
	Vector3 vec = playerPosition - enemyPosition;
	float length = vec.Length();


	// --- ���������ȏ�Ȃ琬�� ---
	if (length >= owner_->GetMiddleRange())
		return true;

	else
		return false;
}



// ===== �������̔��� ======================================================================================================================================================
bool EnemyMiddleRangeJudgment::Judgment()
{
	Vector3 playerPosition = Player::Instance().GetPos();
	Vector3 enemyPosition = owner_->GetPos();

	// --- �v���C���[�Ƃ̋������Ƃ� ---
	Vector3 vec = playerPosition - enemyPosition;
	float length = vec.Length();


	// --- ���������ȉ��Ȃ琬�� ---
	if (length < owner_->GetMiddleRange())
		return true;

	else
		return false;
}



// ===== �ߋ����̔��� ======================================================================================================================================================
bool EnemyShortRangeJudgment::Judgment()
{
	Vector3 playerPosition = Player::Instance().GetPos();
	Vector3 enemyPosition = owner_->GetPos();

	// --- �v���C���[�Ƃ̋������Ƃ� ---
	Vector3 vec = playerPosition - enemyPosition;
	float length = vec.Length();


	// --- ���������ȉ��Ȃ琬�� ---
	if (length < owner_->GetShortRange())
		return true;

	else
		return false;
}



// ===== ���݂̔��� ======================================================================================================================================================
bool EnemyFlinchJudgment::Judgment()
{
	return owner_->IsDown();
}



// ===== ���S���� ======================================================================================================================================================
bool EnemyDeadJudgment::Judgment()
{
	return owner_->IsDead();
}



// ===== ���ʂ̔��� ======================================================================================================================================================
bool EnemyFrontJudgment::Judgment()
{
	Vector3 front = owner_->GetFrontVec();
	Vector3 vec = Player::Instance().GetPos();
	vec = vec - owner_->GetPos();
	vec.Normalize();
	float dot = front.Dot(vec);

	// --- ���� ---
	if (dot > 0.75f)
	{
		return true;
	}

	return false;
}



// ===== ����K�̔��� ======================================================================================================================================================
bool EnemyBigRoarJudgment::Judgment()
{
	if (owner_->IsFoundPlayer() && owner_->actionCount > owner_->roarNeededActionCount)
		return true;

	return false;
}



// ===== �����̔��� ======================================================================================================================================================
bool EnemyFootJudgment::Judgment()
{
	Vector3 position = owner_->GetPos();
	Vector3 vec = position - Player::Instance().GetPos();
	float length = vec.Length();
	if (length < 3.0f)
		return true;

	return false;
}


// ===== ���E�̔��� ======================================================================================================================================================
bool EnemyRightJudgment::Judgment()
{
	Matrix R;
	R.MakeRotationFromQuaternion(owner_->quaternion_);
	Vector3 right = R.v_[0].xyz();
	Vector3 vec = Player::Instance().GetPos();
	vec = vec - owner_->GetPos();
	vec.Normalize();
	float dot = right.Dot(vec);

	// --- ���� ---
	if (dot > 0.5f || dot < -0.5f)
	{
		return true;
	}

	return false;
}



// ===== ���ݍ��݊��݂����I��������̔��� ======================================================================================================================================================
bool EnemyEndRushingBiteJudgment::Judgment()
{
	if (owner_->endRushingBite)
	{
		owner_->endRushingBite = false;
		return true;
	}

	return false;
}



// ===== ���̔��� ======================================================================================================================================================
bool EnemyBehindJudgment::Judgment()
{
	Vector3 front = owner_->GetFrontVec();
	Vector3 vec = Vector3(Player::Instance().GetPos()) - owner_->GetPos();
	vec.Normalize();
	float dot = front.Dot(vec);
	if (dot < 0.0f)
	{
		return true;
	}

	return false;
}



// ===== �o���̔��� ======================================================================================================================================================
bool EnemyAwakeJudgment::Judgment()
{
	if (!owner_->IsAwake() && owner_->GetHP() < owner_->GetMaxHP() * 0.5f)
	{
		owner_->SetAwake(true);
		return true;
	}

	return false;
}



// ===== �o����̔��� ======================================================================================================================================================
bool EnemyAwakedJudgment::Judgment()
{
	return owner_->IsAwake();
}




// ===== ����K�̍s�� ======================================================================================================================================================
BT_ActionState EnemyBigRoarAction::Run(float elapsedTime)
{
	// --- �_�E��/���S���� ---
	if (IsInterrupted())
		return BT_ActionState::Failed;


	switch (step)
	{
		// --- �����ݒ� ---
	case 0:
		//owner_->PlayAnimation(static_cast<int>(MonsterAnimation::ROAR_BIG), false);
		owner_->GetAnimator().SetNextState("hoeru_big");
		owner_->SetFoundPlayer(true);

		step++;
		break;

		// --- ���C������ ---
	case 1:

		// --- radialBlur ���� ---
		owner_->bigRoarTimer += Timer::Instance().DeltaTime();

		// --- �J�����V�F�C�N ---
		float t = owner_->runTimer_;
		owner_->runTimer_ += Timer::Instance().DeltaTime();
		if (t < 0.7f && owner_->runTimer_ > 0.7f)
		{
			CameraManager::Instance().shakeTimer = 1.5f;
			CameraManager::Instance().shakePower = 200.0f;
		}

		// radialBlur ON OFF
		if (!owner_->radialBlur->GetIsRadial() && owner_->bigRoarRadialUpSampling.startTime < owner_->bigRoarTimer)
		{
			owner_->radialBlur->SetSamplingCount(0.0f);
			owner_->radialBlur->SetBlurPower(0.02f);
			owner_->radialBlur->SetIsRadial(true);
		}
		if (owner_->radialBlur->GetIsRadial() && owner_->bigRoarRadialDownSampling.endTime < owner_->bigRoarTimer)
		{
			owner_->radialBlur->SetIsRadial(false);
		}

		// sampling ��������
		float sampCount;
		sampCount = Easing::GetNowParam(Easing::OutQuad<float>, owner_->bigRoarTimer, owner_->bigRoarRadialUpSampling);
		if (owner_->bigRoarRadialDownSampling.startTime < owner_->bigRoarTimer)
		{
			sampCount = Easing::GetNowParam(Easing::OutQuad<float>, owner_->bigRoarTimer, owner_->bigRoarRadialDownSampling);
		}
		owner_->radialBlur->SetSamplingCount(static_cast<float>(static_cast<int>(sampCount)));

		// ���W�v�Z
		DirectX::XMFLOAT3 bonePos = Enemy::Instance().GetBonePosition("sitaago");
		DirectX::XMFLOAT2 ndc = Math::ScreenToNdcPos(Math::WorldToScreenPos(bonePos, CameraManager::Instance().GetCamera().get()));
		ndc.x = (ndc.x + 1.0f) / 2.0f;
		ndc.y = (ndc.y + 1.0f) / 2.0f;
		owner_->radialBlur->SetBlurPosition(ndc);


		// BUG::�u���[�o�O
		// --- �A�j���[�V�������I������� ---
		if (owner_->GetAnimator().GetEndMotion())
		{
			OnEndAction();
			ResetActionCount();
			owner_->bigRoarTimer = 0.0f;	// ���W�A���u���[�p�^�C�}�[�����Z�b�g
			return BT_ActionState::Complete;
		}

		break;
	}


	return BT_ActionState::Run;
}



// ===== �����킹�̍s�� ======================================================================================================================================================
BT_ActionState EnemyAxisAlignmentAction::Run(float elapsedTime)
{
	// --- �_�E��/���S���� ---
	if (IsInterrupted())
		return BT_ActionState::Failed;

	switch (step)
	{
	case 0:

	{
		Matrix R;
		R.MakeRotationFromQuaternion(owner_->quaternion_);
		Vector3 front = R.v_[2].xyz();
		Vector3 playerPosition = Player::Instance().GetPos();
		Vector3 vec = playerPosition - owner_->GetPos();
		vec.Normalize();

		float cross = (front.z * vec.x) - (front.x * vec.z);

		if (cross < 0.0f)
			//owner_->PlayAnimation(static_cast<int>(MonsterAnimation::TURN_LEFT), false);
			owner_->GetAnimator().SetNextState("tokotoko_left");

		else
			//owner_->PlayAnimation(static_cast<int>(MonsterAnimation::TURN_RIGHT), false);
			owner_->GetAnimator().SetNextState("tokotoko_right");

		owner_->turnAngle = owner_->targetVec.Dot(vec);


		// --- �ڕW�Ɍ����Ă����� ---
		float dot = front.Dot(vec);
		if (dot > 0.99f)
		{
			OnEndAction();
			return BT_ActionState::Complete;
		}
	}




	step++;
	break;

	case 1:
	{
#if 1
		Vector3 position = owner_->GetPos();
		Vector3 playerPosition = Player::Instance().GetPos();

		Vector3 targetVec = playerPosition - position;
		targetVec.Normalize();

		// --- ��]�O�ɓ��ς����߂āA�⊮�ʂ����߂� ---
		Matrix R;
		R.MakeRotationFromQuaternion(owner_->quaternion_);
		Vector3 front = R.v_[2].xyz();

		// --- -1.0 ~ 1.0 �� 0.0 ~ 0.9�� ---
		float dot = front.Dot(targetVec);
		dot += 1.0f;
		dot /= 2.0f;
		dot -= 0.1f;
		dot *= 0.05f;

		// --- �ڕW�̕��։�] ---
		owner_->RotateToTargetVec(targetVec.vec_, 0.1f + dot/*, &owner_->front*/);


		R.MakeRotationFromQuaternion(owner_->quaternion_);
		front = R.v_[2].xyz();

		// --- �ڕW�Ɍ����Ă����� ---
		dot = front.Dot(targetVec);
		if (dot > 0.99f)
		{
			OnEndAction();
			return BT_ActionState::Complete;
		}


		if (owner_->GetAnimator().GetEndMotion())
		{
			OnEndAction();
			return BT_ActionState::Complete;
		}
#else
		Quaternion rotation;
		rotation.SetRotationY(owner_->turnAngle / 10.0f);
		owner_->quaternion_ *= rotation;

		Matrix R;
		R.MakeRotationFromQuaternion(owner_->quaternion_);
		Vector3 front = R.v_[2].xyz();

		Vector3 vec = Player::Instance().GetPos() - owner_->GetPos();
		vec.Normalize();

		float dot = front.Dot(vec);
		if (dot > 0.99f)
		{
			step = 0;
			return BT_ActionState::Complete;
		}
#endif

		break;
	}

	}

	return BT_ActionState::Run;
}



// ===== �u���X�̍s�� ======================================================================================================================================================
BT_ActionState EnemyBlessAction::Run(float elapsedTime)
{
	// --- �_�E��/���S���� ---
	if (IsInterrupted())
		return BT_ActionState::Failed;



	switch (step)
	{
	case 0:

		owner_->runTimer_ = 1.5f;
		//owner_->PlayAnimation(static_cast<int>(MonsterAnimation::BLESS), false);
		owner_->GetAnimator().SetNextState("buresu");

		LightningEffect::Instance().HeadAuraEmit(3.5f);

		step++;
		break;

	case 1:
	{
		Vector3 position = owner_->GetPos();
		Vector3 moveVec = Vector3(Player::Instance().GetPos()) - position;

		// --- �ʒu�̍X�V ---
		moveVec.Normalize();
		owner_->RotateToTargetVec(moveVec.vec_, 0.3f);
	}

	owner_->runTimer_ -= elapsedTime;
	if (owner_->runTimer_ < 0.0f)
	{
		Matrix R;
		R.MakeRotationFromQuaternion(owner_->quaternion_);
		Vector3 front = R.v_[2].xyz();
		float atan = atan2(front.z, front.x);
		float theta = DirectX::XMConvertToDegrees(atan);
		owner_->PlayFireBress(-(theta + 180.0f));
		owner_->runTimer_ = 0.0f;
		step++;
	}

	break;


	case 2:

		// --- �G�̕����x�N�g���擾 ---
		DirectX::XMFLOAT4X4 T = owner_->GetTransform();
		DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&T);
		DirectX::XMFLOAT3 front;
		DirectX::XMFLOAT3 right;
		DirectX::XMStoreFloat3(&front, Transform.r[2]);

		float positionLength = 2.0f;
		DirectX::XMFLOAT3 breathPosition = Enemy::Instance().GetBonePosition("sitaago");
		breathPosition += {
			(front.x)* positionLength,
				-1,
				(front.z)* positionLength
		};

		BreathEffect::Instance().SetPosition(breathPosition);

		// --- �A�j���[�V�������I�������I�� ---
		if (owner_->GetAnimator().GetEndMotion())
		{
			OnEndAction();
			return BT_ActionState::Complete;
		}

		break;
	}

	return BT_ActionState::Run;
}



// ===== �Њd�s�� ======================================================================================================================================================
BT_ActionState EnemyThreatAction::Run(float elapsedTime)
{
	// --- �_�E��/���S���� ---
	if (IsInterrupted())
		return BT_ActionState::Failed;



	switch (step)
	{
	case 0:

		// owner_->PlayAnimation(static_cast<int>(MonsterAnimation::ROAR), false);
		owner_->GetAnimator().SetNextState("hoeru");

		step++;
		break;

	case 1:

		// --- �A�j���[�V�������I�������I�� ---
		if (owner_->GetAnimator().GetEndMotion())
		{
			OnEndAction();
			return BT_ActionState::Complete;
		}

		break;
	}

	return BT_ActionState::Run;
}



// ===== �ːi�s�� ======================================================================================================================================================
BT_ActionState EnemyRushAction::Run(float elapsedTime)
{
	// --- �_�E��/���S���� ---
	if (IsInterrupted())
		return BT_ActionState::Failed;

	//	--- �␶���p�ϐ� ---
	static float rockNowTimer = 0.0f;
	static float rockTimer = 0.0f;
	const static float rockTime = 0.005f;
	const static float rockStartTime_2 = 0.3f;
	const static float rockEndTime_2 = 10.0f;
	const static float rockStartTime_3 = 0.5f;
	const static float rockEndTime_3 = 10.0f;
	static bool isEndRock = false;

	switch (step)
	{
	case 0:

		//owner_->PlayAnimation(static_cast<int>(MonsterAnimation::RUSH), false);
		owner_->GetAnimator().SetNextState("tossin");
		owner_->runTimer_ = owner_->GetRushChargeTimer();

		{
			Matrix R;
			R.MakeRotationFromQuaternion(owner_->quaternion_);
			owner_->targetVec = R.v_[2].xyz();
		}

		rockNowTimer = 0.0f;
		rockTimer = 0.0f;
		isEndRock = false;

		step++;
		break;


		// --- �ːi�n�߂̗��߂̎��� ---
	case 1:
	{
		// --- �ڕW�Ɍ������Ĉړ� ---
		Vector3 position = owner_->GetPos();
		Vector3 moveVec = Vector3(Player::Instance().GetPos()) - position;

		// --- �ʒu�̍X�V ---
		moveVec.Normalize();
		owner_->RotateToTargetVec(moveVec.vec_, 0.6f);
	}

	owner_->runTimer_ -= elapsedTime;

	if (owner_->runTimer_ < 0.0f)
	{
		owner_->runTimer_ = owner_->GetWhileRushTimer();
		Matrix R;
		R.MakeRotationFromQuaternion(owner_->quaternion_);
		owner_->targetVec = R.v_[2].xyz();
		step++;
	}

	break;


	// --- �ːi�� ---
	case 2:
	{
		// --- ���ʂֈړ� ---
		Vector3 rootMotionVel = owner_->GetAnimator().GetVelocity();
		rootMotionVel *= 70;
		float len = rootMotionVel.Length();
		owner_->Move(owner_->targetVec, len);

		// --- �␶�� ---
		rockNowTimer += Timer::Instance().DeltaTime();
		if (rockStartTime_2 < rockNowTimer && rockNowTimer < rockEndTime_2)
		{
			rockTimer += Timer::Instance().DeltaTime();
			while (rockTimer > rockTime)
			{
				// ��G�t�F�N�g����
				RockEffect::RockEmitter rock;
				rock.position = Enemy::Instance().GetBonePosition("sitaago_end");
				rock.angle = { Math::RandomRange(0,359), Math::RandomRange(0,359),Math::RandomRange(0,359) };
				rock.scale = { Math::RandomRange(0.25,0.5), Math::RandomRange(0.25,0.5),Math::RandomRange(0.25,0.5) };
				float r = Math::RandomRange(5, 10);
				rock.velocity = { Math::RandomRange(-3,3),Math::RandomRange(1,3),Math::RandomRange(-3,3) };
				rock.gravity = 10;
				rock.lifeTime = 3;
				RockEffect::Instance().Emit(rock);

				rockTimer -= rockTime;
			}
		}

		owner_->runTimer_ -= elapsedTime;
		if (owner_->runTimer_ < 0.0f)
		{
			rockNowTimer = 0.0f;
			step++;
		}

		break;
	}


	// --- �ːi�I��� ---
	case 3:
	{
		if (!isEndRock)
		{
			rockNowTimer += Timer::Instance().DeltaTime();
			if (rockStartTime_3 < rockNowTimer && rockNowTimer < rockEndTime_3)
			{
				for (int i = 0; i < 30; i++)
				{
					// --- �G�̕����x�N�g���擾 ---
					DirectX::XMFLOAT4X4 T = owner_->GetTransform();
					DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&T);
					DirectX::XMFLOAT3 front;
					DirectX::XMFLOAT3 right;
					DirectX::XMStoreFloat3(&front, Transform.r[2]);
					DirectX::XMStoreFloat3(&right, Transform.r[0]);

					// ��G�t�F�N�g����
					RockEffect::RockEmitter rock;
					rock.position = Enemy::Instance().GetBonePosition("sitaago_end");
					rock.angle = { Math::RandomRange(0,359), Math::RandomRange(0,359),Math::RandomRange(0,359) };
					rock.angle = { Math::RandomRange(0,359), Math::RandomRange(0,359),Math::RandomRange(0,359) };
					rock.scale = { Math::RandomRange(0.25,0.5), Math::RandomRange(0.25,0.5),Math::RandomRange(0.25,0.5) };
					float rFront = Math::RandomRange(5, 10);
					float rRight = Math::RandomRange(-3, 3);
					float power = Math::RandomRange(1, 3);
					rock.velocity = {
						(rFront * front.x + rRight * right.x) * power,
						Math::RandomRange(15,30),
						(rFront * front.z + rRight * right.z) * power
					};
					rock.gravity = 30;
					rock.lifeTime = 10;
					RockEffect::Instance().Emit(rock);

					rockTimer -= rockTime;
				}
			}
			isEndRock = true;
		}

		owner_->Move(owner_->targetVec, owner_->GetRushEndSpeed());

	}

	break;
	}

	// --- �A�j���[�V�������I�������I�� ---
	if (owner_->GetAnimator().GetEndMotion())
	{
		OnEndAction();
		return BT_ActionState::Complete;
	}


	return BT_ActionState::Run;
}



// ===== ���݂��s�� ======================================================================================================================================================
BT_ActionState EnemyStampAction::Run(float elapsedTime)
{
	// --- �_�E��/���S���� ---
	if (IsInterrupted())
		return BT_ActionState::Failed;



	switch (step)
	{
	case 0:

		owner_->runTimer_ = 0.75f;
		//owner_->PlayAnimation(static_cast<int>(MonsterAnimation::STAMP), false);
		owner_->GetAnimator().SetNextState("asidon");

		step++;
		break;

	case 1:

		owner_->runTimer_ -= elapsedTime;
		if (owner_->runTimer_ < 0.0f)	// ���݂�����
		{
			// --- �������߂������� ---
			if ((Vector3(Player::Instance().GetPos()) - owner_->GetPos()).Length() < 20.0f)
				CameraManager::Instance().shakeTimer = 0.5f;

			owner_->PlayRockEffect();
			owner_->runTimer_ = 0.0f;
			step++;
		}

		break;


	case 2:

		// --- �A�j���[�V�������I�������I�� ---
		if (owner_->GetAnimator().GetEndMotion())
		{
			OnEndAction();
			return BT_ActionState::Complete;
		}

		break;
	}

	return BT_ActionState::Run;
}



// ===== ���݂��s�� ======================================================================================================================================================
BT_ActionState EnemyBiteAction::Run(float elapsedTime)
{
	// --- �_�E��/���S���� ---
	if (IsInterrupted())
		return BT_ActionState::Failed;


	switch (step)
	{
	case 0:

		// owner_->PlayAnimation(static_cast<int>(MonsterAnimation::BITE_1), false);
		owner_->GetAnimator().SetNextState("kamituki_1");

		step++;
		break;

	case 1:

		// --- �A�j���[�V�������I�������I�� ---
		if (owner_->GetAnimator().GetEndMotion())
		{
			// owner_->PlayAnimation(static_cast<int>(MonsterAnimation::BITE_2), false);
			owner_->GetAnimator().SetNextState("kamituki_2");

			step++;
		}

		break;

	case 2:

		// --- �A�j���[�V�������I�������I�� ---
		if (owner_->GetAnimator().GetEndMotion())
		{
			OnEndAction();
			return BT_ActionState::Complete;
		}

		break;
	}

	return BT_ActionState::Run;
}



// ===== ���ݍ��݊��݂��s�� ======================================================================================================================================================
BT_ActionState EnemyRushingBiteAction::Run(float elapsedTime)
{
	// --- �_�E��/���S���� ---
	if (IsInterrupted())
		return BT_ActionState::Failed;


	switch (step)
	{
	case 0:

		// owner_->PlayAnimation(static_cast<int>(MonsterAnimation::BITE_1), false);
		owner_->GetAnimator().SetNextState("kamituki_1");

		step++;
		break;

	case 1:

	{
		Matrix R;
		R.MakeRotationFromQuaternion(owner_->quaternion_);
		Vector3 front = R.v_[2].xyz();

		owner_->Move(front * 0.15f, 0.5f);
	}

	// --- �A�j���[�V�������I�������I�� ---
	if (owner_->GetAnimator().GetEndMotion())
	{
		// owner_->PlayAnimation(static_cast<int>(MonsterAnimation::BITE_2), false);
		owner_->GetAnimator().SetNextState("kamituki_1");

		step++;
	}

	break;

	case 2:

	{
		Matrix R;
		R.MakeRotationFromQuaternion(owner_->quaternion_);
		Vector3 front = R.v_[2].xyz();

		Vector3 position = owner_->GetPos();
		position += front * 0.15f;
		owner_->SetPos(position.vec_);
	}

	// --- �A�j���[�V�������I�������I�� ---
	if (owner_->GetAnimator().GetEndMotion())
	{
		OnEndAction();
		owner_->endRushingBite = true;
		return BT_ActionState::Complete;
	}

	break;
	}

	return BT_ActionState::Run;
}



// ===== ���ݍs�� ======================================================================================================================================================
BT_ActionState EnemyFlinchAction::Run(float elapsedTime)
{
	// --- ���S���� ---
	if (owner_->IsDead())
		return BT_ActionState::Failed;


	switch (step)
	{
	case 0:

		owner_->runTimer_ = 5.0f;
		// owner_->PlayAnimation(static_cast<int>(MonsterAnimation::DOWN), false);
		owner_->GetAnimator().SetNextState("down");

		step++;

		break;


	case 1:

		if (owner_->GetAnimator().GetEndMotion())
		{
			// owner_->PlayAnimation(static_cast<int>(MonsterAnimation::WHILE_DOWN), true);
			owner_->GetAnimator().SetNextState("down_tyu");

			step++;
		}

		break;


	case 2:

		owner_->runTimer_ -= elapsedTime;
		if (owner_->runTimer_ < 0.0f)
		{
			// owner_->PlayAnimation(static_cast<int>(MonsterAnimation::STAND_UP), false);
			owner_->GetAnimator().SetNextState("okiru");

			step++;
		}

		break;


	case 3:

		if (owner_->GetAnimator().GetEndMotion())
		{
			OnEndAction();
			owner_->SetFlinchValue(100.0f); // Todo : ���ݒl��
			return BT_ActionState::Complete;
		}

		break;
	}


	return BT_ActionState::Run;
}



// ===== ���S�s�� ======================================================================================================================================================
BT_ActionState EnemyDeadAction::Run(float elapsedTime)
{

	switch (step)
	{
	case 0:

		// owner_->PlayAnimation(static_cast<int>(MonsterAnimation::DEAD), false);
		owner_->GetAnimator().SetNextState("sinu");
		step++;

		break;
	}

	return BT_ActionState::Run;
}



// ===== �K����]�s�� ======================================================================================================================================================
BT_ActionState EnemyTailAttack::Run(float elapsedTime)
{
	// --- �_�E��/���S���� ---
	if (IsInterrupted())
		return BT_ActionState::Failed;


	//	--- �␶���p�ϐ� ---
	static float rockNowTimer = 0.0f;
	static float rockTimer = 0.0f;
	const static float rockTime = 0.005f;
	const static float rockStartTime = 1.35f;
	const static float rockEndTime = 2.0f;


	switch (step)
	{
	case 0:
		// owner_->PlayAnimation(static_cast<int>(MonsterAnimation::ROTATION), false);
		owner_->GetAnimator().SetNextState("kaiten");

		rockNowTimer = 0.0f;
		rockTimer = 0.0f;

		step++;
		break;


	case 1:
		// --- �␶�� ---
		rockNowTimer += Timer::Instance().DeltaTime();
		if (rockStartTime < rockNowTimer && rockNowTimer < rockEndTime)
		{
			rockTimer += Timer::Instance().DeltaTime();
			while (rockTimer > rockTime)
			{
				// ��G�t�F�N�g����
				RockEffect::RockEmitter rock;
				rock.position = Enemy::Instance().GetBonePosition("sippo_end");
				rock.position.y = 0.0f;	// �n�ʂɐ���
				rock.angle = { Math::RandomRange(0,359), Math::RandomRange(0,359),Math::RandomRange(0,359) };
				rock.scale = { Math::RandomRange(0.25,0.5), Math::RandomRange(0.25,0.5),Math::RandomRange(0.25,0.5) };
				float r = Math::RandomRange(5, 10);
				rock.velocity = { Math::RandomRange(-3,3), Math::RandomRange(1,3), Math::RandomRange(-3,3) };
				rock.gravity = 10;
				rock.lifeTime = 3;
				RockEffect::Instance().Emit(rock);

				rockTimer -= rockTime;
			}
		}


		if (owner_->GetAnimator().GetEndMotion())
		{
			OnEndAction();
			return BT_ActionState::Complete;
		}

		break;
	}

	return BT_ActionState::Run;
}


// ===== �d���グ�s�� ======================================================================================================================================================
BT_ActionState EnemyScoopUpAction::Run(float elapsedTime)
{
	// --- �_�E��/���S���� ---
	if (owner_->IsDown() || owner_->IsDead())
		return BT_ActionState::Failed;

	//	--- �␶���p�ϐ� ---
	static float rockNowTimer = 0.0f;
	const static float rockStartTime1 = 0.75f;
	const static float rockStartTime2 = 1.1f;
	static bool isRockPlay1 = false;
	static bool isRockPlay2 = false;

	switch (step)
	{
	case 0:
		// owner_->PlayAnimation(static_cast<int>(MonsterAnimation::SCOOP_UP), false);
		owner_->GetAnimator().SetNextState("sukuiage");
		owner_->runTimer_ = 0.5f;

		rockNowTimer = 0.0f;
		isRockPlay1 = false;
		isRockPlay2 = false;

		step++;
		break;


	case 1:
	{
		owner_->runTimer_ -= elapsedTime;

		if (owner_->runTimer_ < 0.0f)
		{
			owner_->runTimer_ = 0.0f;
			step++;
		}

		break;
	}


	case 2:
	{
		Matrix R;
		R.MakeRotationFromQuaternion(owner_->quaternion_);
		Vector3 front = R.v_[2].xyz();

		Vector3 position = owner_->GetPos();
		position += front * 5.0f * elapsedTime;
		owner_->SetPos(position.vec_);

		// --- ��G�t�F�N�g ---
		rockNowTimer += Timer::Instance().DeltaTime();
		if (!isRockPlay1 && rockStartTime1 < rockNowTimer)
		{
			for (int i = 0; i < 15; i++)
			{
				// ��G�t�F�N�g����
				RockEffect::RockEmitter rock;
				rock.position = Enemy::Instance().GetBonePosition("sitaago_end");
				rock.angle = { Math::RandomRange(0,359), Math::RandomRange(0,359),Math::RandomRange(0,359) };
				rock.scale = { Math::RandomRange(0.1,0.3), Math::RandomRange(0.1,0.3),Math::RandomRange(0.1,0.3) };
				float r = Math::RandomRange(5, 10);
				rock.velocity = { Math::RandomRange(-5,5),Math::RandomRange(10,15),Math::RandomRange(-5,5) };
				rock.gravity = 20;
				rock.lifeTime = 10;
				RockEffect::Instance().Emit(rock);
			}
			isRockPlay1 = true;
		}
		if (!isRockPlay2 && rockStartTime2 < rockNowTimer)
		{
			for (int i = 0; i < 30; i++)
			{
				// --- �G�̕����x�N�g���擾 ---
				DirectX::XMFLOAT4X4 T = owner_->GetTransform();
				DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&T);
				DirectX::XMFLOAT3 front;
				DirectX::XMFLOAT3 right;
				DirectX::XMStoreFloat3(&front, Transform.r[2]);
				DirectX::XMStoreFloat3(&right, Transform.r[0]);

				// ��G�t�F�N�g����
				RockEffect::RockEmitter rock;
				rock.position = Enemy::Instance().GetBonePosition("sitaago_end");
				rock.angle = { Math::RandomRange(0,359), Math::RandomRange(0,359),Math::RandomRange(0,359) };
				rock.scale = { Math::RandomRange(0.5,1.0), Math::RandomRange(0.5,1.0),Math::RandomRange(0.5,1.0) };
				float r = Math::RandomRange(5, 10);
				float rFront = Math::RandomRange(5, 10);
				float rRight = Math::RandomRange(-3, 3);
				float power = Math::RandomRange(1, 3);
				rock.velocity = {
					(rFront * front.x + rRight * right.x) * power,
					Math::RandomRange(15,30),
					(rFront * front.z + rRight * right.z) * power
				};
				rock.gravity = 30;
				rock.lifeTime = 10;
				RockEffect::Instance().Emit(rock);
			}
			isRockPlay2 = true;
		}


		if (owner_->GetAnimator().GetEndMotion())
		{
			step = 0;
			return BT_ActionState::Complete;
		}
		break;
	}

	}


	return BT_ActionState::Run;
}



// ===== �^�b�N�� ======================================================================================================================================================
BT_ActionState EnemyTackleAction::Run(float elapsedTime)
{
	// --- �_�E��/���S���� ---
	if (IsInterrupted())
		return BT_ActionState::Failed;


	switch (step)
	{
	case 0:
	{
		step++;
		// --- �E�����̎擾 ---
		Matrix R;
		R.MakeRotationFromQuaternion(owner_->quaternion_);
		Vector3 right = R.v_[0].xyz();

		Vector3 playerPos = Player::Instance().GetPos();
		Vector3 vec = playerPos - owner_->GetPos();
		float length = vec.Length();
		vec.Normalize();


		// --- �E�����Ƃ̓��� ---
		float dotR = right.Dot(vec);
		if (dotR > 0.5f)	// ���ɂ�����^�b�N��
		{
			// owner_->PlayAnimation(static_cast<int>(MonsterAnimation::TACKLE_RIGHT), false);
			owner_->GetAnimator().SetNextState("takkuru_right");
			break;
		}

		else if (dotR < -0.5f)
		{
			// --- �E�^�b�N�� ---
			// owner_->PlayAnimation(static_cast<int>(MonsterAnimation::TACKLE_LEFT), false);
			owner_->GetAnimator().SetNextState("takkuru_left");

			break;
		}

		break;
	}

	}

	// --- �A�j���[�V�������I�������I�� ---
	if (owner_->GetAnimator().GetEndMotion())
	{
		OnEndAction();
		return BT_ActionState::Complete;
	}

	return BT_ActionState::Run;
}



// ===== �����ֈړ� ======================================================================================================================================================
BT_ActionState EnemyMoveCenterAction::Run(float elapsedTime)
{
	// --- �_�E��/���S���� ---
	if (IsInterrupted())
		return BT_ActionState::Failed;

	switch (step)
	{
	case 0:

		// owner_->PlayAnimation(static_cast<int>(MonsterAnimation::WALK_FOWARD), true);
		owner_->GetAnimator().SetNextState("walk_mae");
		step++;

		break;


	case 1:
	{
		Vector3 moveVec = Vector3(0.0f, 0.0f, 0.0f) - owner_->GetPos();
		if (moveVec.Length() < 1.0f)
		{
			OnEndAction();
			return BT_ActionState::Complete;
		}

		moveVec.Normalize();

		owner_->RotateToTargetVec(moveVec.vec_, 0.1f);
		Vector3 front = owner_->GetFrontVec();
		owner_->Move(front, owner_->runSpeed_);

		break;
	}

	}

	return BT_ActionState::Run;
}



// ===== �o���ڍs�s�� ======================================================================================================================================================
BT_ActionState EnemyDeathBlowAction::Run(float elapsedTime)
{
	switch (step)
	{
	case 0:
		owner_->runTimer_ = 1.2f;
		step++;
		break;

	case 1:

		//owner_->PlayAnimation(static_cast<int>(MonsterAnimation::DEATHBLOW_1), false);
		owner_->GetAnimator().SetNextState("hissatu_1");
		step++;

		break;


	case 2:
	{
		//owner_->runTimer_ -= Timer::Instance().DeltaTime();
		//if (owner_->runTimer_ < 0.0f)
		//{
		//	owner_->runTimer_ = 100.0f;
		//}

		if (owner_->GetAnimator().GetEndMotion())
		{
			//owner_->PlayAnimation(static_cast<int>(MonsterAnimation::DEATHBLOW_2), false);
			owner_->GetAnimator().SetNextState("hissatu_2");
			SpecialEffect::Instance().Emit();

			owner_->runTimer_ = 8.5f;
			step++;
		}

		break;
	}


	case 3:
	{
		owner_->runTimer_ -= Timer::Instance().DeltaTime();
		//if (owner_->GetAnimator().GetEndMotion())
		//{
		//	owner_->hissatuCount++;
		//	//owner_->PlayAnimation(static_cast<int>(MonsterAnimation::DEATHBLOW_3), false);
		//}

		if (owner_->runTimer_ < 0.0f && owner_->GetAnimator().GetEndMotion())
		{
			owner_->GetAnimator().SetNextState("hissatu_3");
			owner_->runTimer_ = 0.0f;
			owner_->hissatuCount = 0;

			step++;
		}

		break;
	}


	case 4:
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
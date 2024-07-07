#include "EnemyBehavior.h"

#include "../../Library/Math/Math.h"

#include "../Player/Player.h"


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
	if (owner_->IsDown() || owner_->IsDead())
		return BT_ActionState::Failed;

	switch (step)
	{
		// --- �����ݒ� ---
	case 0:

		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::WALK_FOWARD), true);	// �A�j���[�V�����̐ݒ�

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
		enemyPosition += moveVec;
		owner_->SetPos(enemyPosition);


		// --- �v���C���[�ɋ߂Â����� ---
		if (length < 10.0f)
		{
			step = 0;
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
	// TODO : ���E�Ƃ��Č�������
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
	if (owner_->IsDown() || owner_->IsDead())
		return BT_ActionState::Failed;



	switch (step)
	{
		// --- �����ݒ� ---
	case 0:

		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::WALK_FOWARD), true);
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
			step = 0;
			return BT_ActionState::Complete;
		}


		// --- �ʒu�̍X�V ---
		moveVec.Normalize();
		owner_->RotateToTargetVec(moveVec.vec_, 0.3f);
		moveVec *= owner_->walkSpeed_ * elapsedTime;
		position = position + moveVec;
		owner_->SetPos(position.vec_);


		// --- �v���C���[���������� ---
		if (owner_->SearchPlayer())
		{
			step = 0;
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
	if (owner_->IsDown() || owner_->IsDead())
		return BT_ActionState::Failed;



	switch (step)
	{
		// --- �����ݒ� ---
	case 0:

		owner_->runTimer_ = (rand() / FLT_MAX) * 2.0f + 3.0f;	// 3.0 ~ 5.0
		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::IDLE), true);

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

			owner_->runTimer_ = 0.0f;
			step = 0;
			return BT_ActionState::Complete;
		}


		// --- �v���C���[���������� ---
		if (owner_->SearchPlayer())
		{
			owner_->runTimer_ = 0.0f;
			step = 0;
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
	if (owner_->SearchPlayer())
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
	if (length > owner_->GetLongRange())
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




// ===== ����K�̍s�� ======================================================================================================================================================
BT_ActionState EnemyBigRoarAction::Run(float elapsedTime)
{
	// --- �_�E��/���S���� ---
	if (owner_->IsDown() || owner_->IsDead())
		return BT_ActionState::Failed;



	switch (step)
	{
		// --- �����ݒ� ---
	case 0:

		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::ROAR_BIG), false);
		owner_->SetFoundPlayer(true); // �v���C���[�𔭌�

		step++;
		break;

		// --- ���C������ ---
	case 1:

		// --- �A�j���[�V�������I������� ---
		if (owner_->GetAnimationEndFlag())
		{
			step = 0;
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
	if (owner_->IsDown() || owner_->IsDead())
		return BT_ActionState::Failed;

	switch (step)
	{
	case 0:

		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::IDLE), true);
		owner_->SetCurrentAnimationSeconds(0.0f);
		owner_->SetAnimationEndFlag(false);

		// --- ��]�̂��߂ɐ��ʂ��擾 ---
		{
			Matrix R;
			R.MakeRotationFromQuaternion(owner_->quaternion_);
			owner_->targetVec = R.v_[2].xyz();
		}

		step++;
		break;

	case 1:
	{

		Vector3 position = owner_->GetPos();
		Vector3 playerPosition = Player::Instance().GetPos();

		Vector3 targetVec = playerPosition - position;
		targetVec.Normalize();

		// --- �ڕW�̕��։�] ---
		owner_->RotateToTargetVec(targetVec.vec_, 0.1f/*, &owner_->front*/);


		Matrix R;
		R.MakeRotationFromQuaternion(owner_->quaternion_);
		Vector3 front = R.v_[2].xyz();

		// --- �ڕW�Ɍ����Ă����� ---
		float dot = front.Dot(targetVec);
		if (dot > 0.99f)
		{
			step = 0;
			return BT_ActionState::Complete;
		}

		break;
	}

	}

	return BT_ActionState::Run;
}



// ===== �u���X�̍s�� ======================================================================================================================================================
BT_ActionState EnemyBlessAction::Run(float elapsedTime)
{
	// --- �_�E��/���S���� ---
	if (owner_->IsDown() || owner_->IsDead())
		return BT_ActionState::Failed;



	switch (step)
	{
	case 0:

		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::BLESS), false);

		step++;
		break;

	case 1:

		// --- �A�j���[�V�������I�������I�� ---
		if (owner_->GetAnimationEndFlag())
		{
			step = 0;
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
	if (owner_->IsDown() || owner_->IsDead())
		return BT_ActionState::Failed;



	switch (step)
	{
	case 0:

		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::ROAR), false);

		step++;
		break;

	case 1:

		// --- �A�j���[�V�������I�������I�� ---
		if (owner_->GetAnimationEndFlag())
		{
			step = 0;
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
	if (owner_->IsDown() || owner_->IsDead())
		return BT_ActionState::Failed;



	switch (step)
	{
	case 0:

		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::RUSH), false);
		owner_->runTimer_ = owner_->GetRushChargeTimer();

		{
			Matrix R;
			R.MakeRotationFromQuaternion(owner_->quaternion_);
			owner_->targetVec = R.v_[2].xyz();
		}

		step++;
		break;


		// --- �ːi�n�߂̗��߂̎��� ---
	case 1:

		owner_->runTimer_ -= elapsedTime;

		if (owner_->runTimer_ < 0.0f)
		{
			owner_->runTimer_ = owner_->GetWhileRushTimer();
			step++;
		}

		break;


		// --- �ːi�� ---
	case 2:
	{
		// --- ���ʂֈړ� ---
		Vector3 position = owner_->GetPos();
		position += owner_->targetVec * owner_->GetRushSpeed() * elapsedTime;
		owner_->SetPos(position.vec_);

		owner_->runTimer_ -= elapsedTime;
		if (owner_->runTimer_ < 0.0f)
		{
			step++;
		}

		break;
	}


	// --- �ːi�I��� ---
	case 3:
	{
		// --- ���ʂֈړ� ---
		Vector3 position = owner_->GetPos();
		position += owner_->targetVec * owner_->GetRushEndSpeed() * elapsedTime;
		owner_->SetPos(position.vec_);

		break;
	}

	}

	// --- �A�j���[�V�������I�������I�� ---
	if (owner_->GetAnimationEndFlag())
	{
		step = 0;
		owner_->runTimer_ = 0.0f;
		return BT_ActionState::Complete;
	}


	return BT_ActionState::Run;
}



// ===== ���݂��s�� ======================================================================================================================================================
BT_ActionState EnemyStampAction::Run(float elapsedTime)
{
	// --- �_�E��/���S���� ---
	if (owner_->IsDown() || owner_->IsDead())
		return BT_ActionState::Failed;



	switch (step)
	{
	case 0:

		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::STAMP), false);

		step++;
		break;

	case 1:

		// --- �A�j���[�V�������I�������I�� ---
		if (owner_->GetAnimationEndFlag())
		{
			step = 0;
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
	if (owner_->IsDown() || owner_->IsDead())
		return BT_ActionState::Failed;



	switch (step)
	{
	case 0:

		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::BITE_1), false);

		step++;
		break;

	case 1:

		// --- �A�j���[�V�������I�������I�� ---
		if (owner_->GetAnimationEndFlag())
		{
			owner_->PlayAnimation(static_cast<int>(MonsterAnimation::BITE_2), false);
			step++;
		}

		break;

	case 2:

		// --- �A�j���[�V�������I�������I�� ---
		if (owner_->GetAnimationEndFlag())
		{
			step = 0;
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
	if (owner_->IsDown() || owner_->IsDead())
		return BT_ActionState::Failed;



	switch (step)
	{
	case 0:

		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::BITE_1), false);

		step++;
		break;

	case 1:

	{
		Matrix R;
		R.MakeRotationFromQuaternion(owner_->quaternion_);
		Vector3 front = R.v_[2].xyz();

		Vector3 position = owner_->GetPos();
		position += front * 0.15f;
		owner_->SetPos(position.vec_);
	}

	// --- �A�j���[�V�������I�������I�� ---
	if (owner_->GetAnimationEndFlag())
	{
		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::BITE_2), false);
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
	if (owner_->GetAnimationEndFlag())
	{
		step = 0;
		return BT_ActionState::Complete;
	}

	break;
	}

	return BT_ActionState::Run;
}



// ===== ���ݍ��݊��݂���̍s�� ======================================================================================================================================================
BT_ActionState EnemyAfterRushingBiteAction::Run(float elapsedTime)
{
	// --- �_�E��/���S���� ---
	if (owner_->IsDown() || owner_->IsDead())
		return BT_ActionState::Failed;



	switch (step)
	{
	case 0:

		// --- ���̎��_�� ���݂� ���E�^�b�N�� �Њd �������Ȃ� �����߂� ---
	{
		// --- ���ʂƉE�����̎擾 ---
		Matrix R;
		R.MakeRotationFromQuaternion(owner_->quaternion_);
		Vector3 front = R.v_[2].xyz();
		Vector3 right = R.v_[0].xyz();

		Vector3 playerPos = Player::Instance().GetPos();
		Vector3 vec = playerPos - owner_->GetPos();
		float length = vec.Length();
		vec.Normalize();


		// --- �������߂������� ---
		if (length < 10.0f)
		{
			owner_->PlayAnimation(static_cast<int>(MonsterAnimation::STAMP), false);
			step++;
			break;
		}

		// --- �E�����Ƃ̓��� ---
		float dotR = right.Dot(vec);
		if (dotR > 0.5f)	// ���ɂ�����^�b�N��
		{
			owner_->PlayAnimation(static_cast<int>(MonsterAnimation::TACKLE), false);
			step = 2;
			owner_->runTimer_ = 0.25f;
			break;
		}

		else if (dotR < -0.5f)
		{
			// --- �E�^�b�N�� ---
			owner_->PlayAnimation(static_cast<int>(MonsterAnimation::TACKLE), false);
			step = 3;
			owner_->runTimer_ = 0.25f;
			break;
		}


		float dotF = front.Dot(vec);
		if (dotF > 0.5f)	// ���ʂɂ�����
		{
			return BT_ActionState::Complete;
		}

		else if (dotF < -0.5f)	// ���ɂ�����
		{
			owner_->PlayAnimation(static_cast<int>(MonsterAnimation::ROAR), false);
			step++;
			break;
		}
	}

	break;


	case 1:

		// --- �A�j���[�V�������I�������I�� ---
		if (owner_->GetAnimationEndFlag())
		{
			step = 0;
			return BT_ActionState::Complete;
		}

		break;


	case 2:	// �E�^�b�N��
	{
		owner_->runTimer_ -= elapsedTime;

		if(owner_->runTimer_ < 0.0f)
		{
			Matrix R;
			R.MakeRotationFromQuaternion(owner_->quaternion_);
			Vector3 right = R.v_[0].xyz();

			Vector3 position = owner_->GetPos();
			position += right * 0.1f;
			owner_->SetPos(position.vec_);
		}

		// --- �A�j���[�V�������I�������I�� ---
		if (owner_->GetAnimationEndFlag())
		{
			step = 0;
			owner_->runTimer_ = 0.0f;
			return BT_ActionState::Complete;
		}

		break;
	}


	case 3:	// �E�^�b�N��
	{
		owner_->runTimer_ -= elapsedTime;

		if (owner_->runTimer_ < 0.0f)
		{
			Matrix R;
			R.MakeRotationFromQuaternion(owner_->quaternion_);
			Vector3 left = -R.v_[0].xyz();

			Vector3 position = owner_->GetPos();
			position += left * 0.1f;
			owner_->SetPos(position.vec_);
		}

		// --- �A�j���[�V�������I�������I�� ---
		if (owner_->GetAnimationEndFlag())
		{
			step = 0;
			owner_->runTimer_ = 0.0f;
			return BT_ActionState::Complete;
		}

		break;
	}

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
		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::DOWN), false);
		step++;

		break;


	case 1:

		if (owner_->GetAnimationEndFlag())
		{
			owner_->PlayAnimation(static_cast<int>(MonsterAnimation::WHILE_DOWN), true);
			step++;
		}

		break;


	case 2:

		owner_->runTimer_ -= elapsedTime;
		if (owner_->runTimer_ < 0.0f)
		{
			owner_->PlayAnimation(static_cast<int>(MonsterAnimation::STAND_UP), false);
			step++;
		}

		break;


	case 3:

		if (owner_->GetAnimationEndFlag())
		{
			owner_->runTimer_ = 0.0f;
			step = 0;
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

		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::DEAD), false);
		step++;

		break;
	}

	return BT_ActionState::Run;
}



// ===== �K����]�s�� ======================================================================================================================================================
BT_ActionState EnemyTailAttack::Run(float elapsedTime)
{
	switch (step)
	{
	case 0:
		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::IDLE), true);

		step++;
		break;


	case 1:
	{
		float rotateSpeed = elapsedTime * 360.0f;
		Quaternion orientation;
		orientation.SetRotationDegY(rotateSpeed);

		owner_->quaternion_ *= orientation;
		owner_->runTimer_ += rotateSpeed;

		if (owner_->runTimer_ > 180.0f)
		{
			step++;
			owner_->runTimer_ = 1.5f;
		}

		break;
	}


	case 2:
	{
		owner_->runTimer_ -= elapsedTime;

		if (owner_->runTimer_ < 0.0f)
		{
			step++;
			owner_->runTimer_ = 0.0f;
		}

		break;
	}


	case 3:
	{
		float rotateSpeed = elapsedTime * 360.0f;
		Quaternion orientation;
		orientation.SetRotationDegY(rotateSpeed);

		owner_->quaternion_ *= orientation;
		owner_->runTimer_ += rotateSpeed;

		if (owner_->runTimer_ > 180.0f)
		{
			step++;
			owner_->runTimer_ = 1.0f;
		}

		break;
	}


	case 4:
	{
		owner_->runTimer_ -= elapsedTime;

		if (owner_->runTimer_ < 0.0f)
		{
			step = 0;
			owner_->runTimer_ = 0.0f;
			return BT_ActionState::Complete;
		}
	}

	}

	return BT_ActionState::Run;
}

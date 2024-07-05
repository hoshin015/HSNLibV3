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




// ===== ����K�̍s�� ======================================================================================================================================================
BT_ActionState EnemyBigRoarAction::Run(float elapsedTime)
{
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
	switch(step)
	{
	case 0:

		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::IDLE), true);
		owner_->SetCurrentAnimationSeconds(0.0f);
		owner_->SetAnimationEndFlag(false);

		// --- ��]�̂��߂ɐ��ʂ��擾 ---
		{
			Matrix R;
			R.MakeRotationFromQuaternion(owner_->quaternion_);
			owner_->front = R.v_[2].xyz();
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
	switch (step)
	{
	case 0:

		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::RUSH), false);
		owner_->runTimer_ = owner_->GetRushChargeTimer();

		{
			Matrix R;
			R.MakeRotationFromQuaternion(owner_->quaternion_);
			owner_->front = R.v_[2].xyz();
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
		position += owner_->front * owner_->GetRushSpeed() * elapsedTime;
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
		position += owner_->front * owner_->GetRushEndSpeed() * elapsedTime;
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

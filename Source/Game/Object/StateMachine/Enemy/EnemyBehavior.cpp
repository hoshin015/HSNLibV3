#include "EnemyBehavior.h"

#include "../../Library/Math/Math.h"
#include "../../Library/Math/OperatorXMFloat3.h"

#include "../Player/Player.h"



// --- �ǐՍs���̔��� ---
bool EnemyPursuitJudgment::Judgment()
{
	DirectX::XMFLOAT3 playerPosition = Player::Instance().GetPos();
	DirectX::XMFLOAT3 enemyPosition = owner_->GetPos();

	// --- �v���C���[�Ƃ̋������Ƃ� ---
	// TODO : ���E�Ƃ��Č�������
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

		//owner_->PlayAnimation(9/*Run*/, true);	// �A�j���[�V�����̐ݒ�
		owner_->PlayAnimation(13/*Run*/, true);	// �A�j���[�V�����̐ݒ�

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
	return owner_->SearchPlayer();
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
	if (length < 10.0f)
		return true;

	else
		return false;
}


// --- �U���s�� ---
BT_ActionState EnemyAttackAction::Run(float elapsedTime)
{
	switch (step)
	{
		// --- �����ݒ� ---
	case 0:

		//owner_->PlayAnimation(0/*Attack*/, false);	// �A�j���[�V�����̐ݒ�
		owner_->PlayAnimation(0/*Attack*/, false);	// �A�j���[�V�����̐ݒ�
		owner_->SetCurrentAnimationSeconds(0.0f);
		owner_->SetAnimationEndFlag(false);

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


BT_ActionState EnemyAttackAction2::Run(float elapsedTime)
{
	switch (step)
	{
		// --- �����ݒ� ---
	case 0:

		//owner_->PlayAnimation(1/*Attack2*/, false);	// �A�j���[�V�����̐ݒ�
		owner_->PlayAnimation(1/*Attack2*/, false);	// �A�j���[�V�����̐ݒ�
		owner_->SetCurrentAnimationSeconds(0.0f);
		owner_->SetAnimationEndFlag(false);

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



// --- �T���s���̔��� ---
bool EnemyScoutJudgment::Judgment()
{
	return !owner_->SearchPlayer();
}


// --- �p�j�s���̔��� ---
bool EnemyWanderJudgment::Judgment()
{
	// --- �ڕW�ւ̃x�N�g�� ---
	DirectX::XMFLOAT3 moveVec = owner_->moveTargetPosition_ - owner_->GetPos();

	// --- �ڕW�̋��������������� ---
	if (Float3Length(moveVec) > 1.0f)
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

		//owner_->PlayAnimation(12/*Walk*/, true);
		owner_->PlayAnimation(17/*Walk*/, true);
		step++;
		break;


		// --- ���C������ ---
	case 1:

		// --- �ڕW�Ɍ������Ĉړ� ---
		DirectX::XMFLOAT3 moveVec = owner_->moveTargetPosition_ - owner_->GetPos();

		// --- �ڕW�Ƃ̋������߂��Ȃ����� ---
		if (Float3Length(moveVec) < 1.0f)
		{
			step = 0;
			return BT_ActionState::Complete;
		}


		// --- �ʒu�̍X�V ---
		moveVec = Math::XMFloat3Normalize(moveVec);
		owner_->RotateToTargetVec(moveVec, 0.3f);
		moveVec *= owner_->walkSpeed_ * elapsedTime;
		DirectX::XMFLOAT3 position = owner_->GetPos() + moveVec;
		owner_->SetPos(position);


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
		//owner_->PlayAnimation(6/*Idle*/, true);
		owner_->PlayAnimation(10/*Idle*/, true);

			step++;
		break;


		// --- ���C������ ---
	case 1:

		owner_->runTimer_ -= elapsedTime;

		// --- �ҋ@���Ԃ��߂����� ---
		if (owner_->runTimer_ < 0.0f)
		{
			const float radius = 50.0f;
			float theta = DirectX::XMConvertToRadians(rand() % 360);
			owner_->moveTargetPosition_ = { cosf(theta) * radius, 0.0f, sinf(theta) * radius };

			owner_->runTimer_ = 0.0f;
			step = 0;
			return BT_ActionState::Complete;
		}

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

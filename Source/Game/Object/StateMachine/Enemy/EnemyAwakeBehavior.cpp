#include "EnemyAwakeBehavior.h"
#include "../../Effect/Lightning/LightningEffect.h"
#include "../Player/Player.h"
#include "../../../../../Library/Timer.h"
#include "../../Effect/Rock/RockEffect.h"
#include "../../../../../Library/3D/CameraManager.h"
#include "../../../../../Library/Math/Math.h"

#include <DirectXTex.h>



// ===== �o���O�̔��� ======================================================================================================================================================
bool EnemyBeforeAwakeJudgment::Judgment()
{
	return !owner_->IsAwake();
}


// ===== ���K�̗��U�� ======================================================================================================================================================
BT_ActionState EnemyLightningRoarAction::Run(float elapsedTime)
{
	// --- �_�E��/���S���� ---
	if (IsInterrupted())
		return BT_ActionState::Failed;

	switch (step)
	{
	case 0:
	{
		owner_->GetAnimator().SetNextState("hoeru");
		owner_->runTimer_ = 0.5f;
		LightningEffect::Instance().HeadAuraEmit(3.5f);
		step++;
		break;
	}


	case 1:
	{
		owner_->runTimer_ -= Timer::Instance().DeltaTime();
		if (owner_->runTimer_ < 0.0f)
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



// ===== ����K�̗��U�� ======================================================================================================================================================
BT_ActionState EnemyLightningBigRoarAction::Run(float elapsedTime)
{
	// --- �_�E��/���S���� ---
	if (IsInterrupted())
		return BT_ActionState::Failed;

	switch (step)
	{
	case 0:
	{
		owner_->GetAnimator().SetNextState("hoeru_big");
		owner_->runTimer_ = 0.2f;
		LightningEffect::Instance().HeadAuraEmit(3.5f);
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



// ===== �����݂̗��U�� ======================================================================================================================================================
BT_ActionState EnemyLightningStampAction::Run(float elapsedTime)
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
		LightningEffect::Instance().HeadAuraEmit(3.5f);
		step++;
		break;

	case 1:

		owner_->runTimer_ -= elapsedTime;
		if (owner_->runTimer_ < 0.0f)	// ���݂�����
		{
			// --- �������߂������� ---
			if ((Vector3(Player::Instance().GetPos()) - owner_->GetPos()).Length() < 20.0f)
				CameraManager::Instance().shakeTimer = 0.5f;

			for (int i = 0; i < 10; i++)
			{
				// ��G�t�F�N�g����
				RockEffect::RockEmitter rock;
				rock.position = Enemy::Instance().GetBonePosition("tumasaki_L");
				DirectX::XMFLOAT3 rPos = { (rand() % 2 - 1.0f), (rand() % 2 - 1.0f) , (rand() % 2 - 1.0f) };
				rock.position += rPos;
				rock.angle = { Math::RandomRange(0,359), Math::RandomRange(0,359),Math::RandomRange(0,359) };
				rock.scale = { Math::RandomRange(0.25,0.75), Math::RandomRange(0.25,0.75),Math::RandomRange(0.25,0.75) };
				float r = Math::RandomRange(5, 10);
				rock.velocity = { Math::RandomRange(-3,3),Math::RandomRange(1,5),Math::RandomRange(-3,3) };
				rock.gravity = 10;
				rock.lifeTime = 3;
				RockEffect::Instance().Emit(rock);
			}


			for (int i = 0; i < 6; i++)
			{
				float range = static_cast<float>(rand() % 25) + 10.0f;
				float theta = DirectX::XMConvertToRadians(static_cast<float>(rand() % 360));
				Vector3 position = Vector3(owner_->GetPos()) + Vector3(cosf(theta) * range, 0.0f, sinf(theta) * range);
				LightningEffect::Instance().Emit(position.vec_);
			}


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



// ===== �K����]���t�� ======================================================================================================================================================
BT_ActionState EnemyLightningTailAction::Run(float elapsedTime)
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
		LightningEffect::Instance().HeadAuraEmit(3.5f);

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


			// ���G�t�F�N�g
			owner_->runTimer_ -= Timer::Instance().DeltaTime();
			if(owner_->runTimer_ < 0.0f)
			{
				owner_->runTimer_ = 0.05f;

				Vector3 position = owner_->GetPos();
				Vector3 tailPosition = Enemy::Instance().GetBonePosition("sippo_end");
				tailPosition.y = 0.0f;
				Vector3 vec = tailPosition - position;
				vec.Normalize();
				tailPosition += vec * (static_cast<float>(rand() % 10) + 3.0f);
				LightningEffect::Instance().Emit(tailPosition.vec_);
			}
		}


		// ��
		//owner_->runTimer_ -= Timer::Instance().DeltaTime();
		//if (owner_->runTimer_ < 0.0f)
		//{
		//	owner_->runTimer_ = 0.125f;
		//	Vector3 tailPos = owner_->GetBonePosition("sippo_end");
		//	tailPos.y = 0.0f;
		//	LightningEffect::Instance().Emit(tailPos.vec_);
		//}



		if (owner_->GetAnimator().GetEndMotion())
		{
			OnEndAction();
			return BT_ActionState::Complete;
		}

		break;
	}

	return BT_ActionState::Run;
}



// ===== ���ːi ======================================================================================================================================================
BT_ActionState EnemyLightningRushAction::Run(float elapsedTime)
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
	static float timer = 0.0f;

	switch (step)
	{
	case 0:

		//owner_->PlayAnimation(static_cast<int>(MonsterAnimation::RUSH), false);
		owner_->GetAnimator().SetNextState("tossin");
		owner_->runTimer_ = owner_->GetRushChargeTimer();
		LightningEffect::Instance().HeadAuraEmit(3.5f);

		{
			Matrix R;
			R.MakeRotationFromQuaternion(owner_->quaternion_);
			owner_->targetVec = R.v_[2].xyz();
		}

		rockNowTimer = 0.0f;
		rockTimer = 0.0f;
		isEndRock = false;

		timer = 1.0f;

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
		// rootMotionVel *= 70;
		Matrix R;
		R.MakeRotationFromQuaternion(owner_->quaternion_);
		rootMotionVel.TransformCoord(rootMotionVel, R);
		Vector3 pos = owner_->GetPos();
		pos += rootMotionVel;
		owner_->SetPos(pos.vec_);

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

		timer -= elapsedTime;
		if (timer < 0.0f)
		{
			timer = 0.2f;
			LightningEffect::Instance().Emit(owner_->GetPos());
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



// ===== ���d���グ ======================================================================================================================================================
BT_ActionState EnemyLightningScoopUpAction::Run(float elapsedTime)
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
		LightningEffect::Instance().HeadAuraEmit(3.5f);


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
			int r = rand() % 2;
			owner_->PlayFanLightning(front, 30.0f, 15.0f, 30);

			switch (r)
			{
			case 0:
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
				rock.scale = { Math::RandomRange(3.5,5.0), Math::RandomRange(3.5,5.0),Math::RandomRange(3.5,5.0) };
				float rFront = 20;
				float rRight = 0;
				float power = 3;
				rock.velocity = {
					(rFront * front.x + rRight * right.x) * power,
					5,
					(rFront * front.z + rRight * right.z) * power
				};
				rock.gravity = 30;
				rock.lifeTime = 10;

				rock.isDamaged = false;
				rock.damageRadius = 4.0f;
				rock.damage = 50.0f;
				RockEffect::Instance().Emit(rock);
			}
			break;
			case 1:
			{
				// --- �G�̕����x�N�g���擾 ---
				DirectX::XMFLOAT4X4 T = owner_->GetTransform();
				DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&T);
				DirectX::XMFLOAT3 front;
				DirectX::XMFLOAT3 right;
				DirectX::XMStoreFloat3(&front, Transform.r[2]);
				DirectX::XMStoreFloat3(&right, Transform.r[0]);


				float rFront = 20;
				float power = 4;

				for (int i = 0; i < 3; i++)
				{
					// ��G�t�F�N�g����
					RockEffect::RockEmitter rock;
					rock.position = Enemy::Instance().GetBonePosition("sitaago_end");
					rock.angle = { Math::RandomRange(0,359), Math::RandomRange(0,359),Math::RandomRange(0,359) };
					rock.scale = { Math::RandomRange(1.5,2.0), Math::RandomRange(1.5,2.0),Math::RandomRange(1.5,2.0) };

					float rRight = -10;
					rRight += 10 * i;
					rock.velocity = {
						(rFront * front.x + rRight * right.x) * power,
						5,
						(rFront * front.z + rRight * right.z) * power
					};
					rock.gravity = 30;
					rock.lifeTime = 10;


					rock.isDamaged = false;
					rock.damageRadius = 1.8f;
					rock.damage = 50.0f;

					RockEffect::Instance().Emit(rock);
				}
			}
			break;
			}
			isRockPlay2 = true;
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



// ===== �^�b�N�� ======================================================================================================================================================
BT_ActionState EnemyLightningTackleAction::Run(float elapsedTime)
{
	// --- �_�E��/���S���� ---
	if (IsInterrupted())
		return BT_ActionState::Failed;


	switch (step)
	{
	case 0:
	{
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
			step = 1;
			break;
		}

		else if (dotR < -0.5f)
		{
			// --- �E�^�b�N�� ---
			// owner_->PlayAnimation(static_cast<int>(MonsterAnimation::TACKLE_LEFT), false);
			owner_->GetAnimator().SetNextState("takkuru_left");
			step = 2;

			break;
		}

		break;
	}


	case 1: // �E
	{
		break;
	}


	case 2:	// ��
	{
		break;
	}

	}

	// --- �A�j���[�V�������I�������I�� ---
	if (owner_->GetAnimator().GetEndMotion())
	{
		Matrix R;
		R.MakeRotationFromQuaternion(owner_->quaternion_);
		Vector3 right = R.v_[2].xyz();

		owner_->PlayFanLightning((step == 1) ? right : -right, 30.0f, 5.0f, 10);

		OnEndAction();
		return BT_ActionState::Complete;
	}

	return BT_ActionState::Run;
}

#include "EnemyBehavior.h"

#include "../../../../../Library/Graphics/RadialBlur.h"
#include "../../Library/Math/Math.h"

#include "../../Effect/Breath/BreathEffect.h"

#include "../Player/Player.h"
#include "../../../../../Library/Timer.h"


// --- 追跡行動の判定 ---
bool EnemyPursuitJudgment::Judgment()
{
	DirectX::XMFLOAT3 playerPosition = Player::Instance().GetPos();
	DirectX::XMFLOAT3 enemyPosition = owner_->GetPos();

	// --- プレイヤーとの距離をとる ---
	float length = Math::XMFloat3Length(playerPosition, enemyPosition);


	// --- 距離が一定以上なら追跡 ---
	if (length > 10.0f)
		return true;

	else
		return false;
}


// --- 追跡行動 ---
BT_ActionState EnemyPursuitAction::Run(float elapsedTime)
{
	// --- ダウン/死亡処理 ---
	if (owner_->IsDown() || owner_->IsDead())
		return BT_ActionState::Failed;

	switch (step)
	{
		// --- 初期設定 ---
	case 0:

		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::WALK_FOWARD), true);	// アニメーションの設定

		step++;
		break;


		// --- メイン処理 ---
	case 1:
		DirectX::XMFLOAT3 playerPosition = Player::Instance().GetPos();
		DirectX::XMFLOAT3 enemyPosition = owner_->GetPos();
		float length = Math::XMFloat3Length(playerPosition, enemyPosition);


		// --- プレイヤーへのベクトルをとる ---
		DirectX::XMFLOAT3 vec = playerPosition - enemyPosition;
		vec = Math::XMFloat3Normalize(vec);
		vec.y = 0.0f;

		// --- ベクトルの方向に回転 ---
		owner_->RotateToTargetVec(vec, 0.3f);

		// --- 位置の更新 ---
		const float moveSpeed = owner_->runSpeed_ * elapsedTime;
		DirectX::XMFLOAT3 moveVec = vec * moveSpeed;
		enemyPosition += moveVec;
		owner_->SetPos(enemyPosition);


		// --- プレイヤーに近づいたら ---
		if (length < 10.0f)
		{
			step = 0;
			return BT_ActionState::Complete;    // 追跡完了
		}

		break;
	}

	return BT_ActionState::Run; // 追跡継続
}



// --- 戦闘状態の判定 ---
bool EnemyBattleJudgment::Judgment()
{
	return owner_->IsFoundPlayer();
}

// --- 攻撃行動の判定 ---
bool EnemyAttackJudgment::Judgment()
{
	DirectX::XMFLOAT3 playerPosition = Player::Instance().GetPos();
	DirectX::XMFLOAT3 enemyPosition = owner_->GetPos();

	// --- プレイヤーとの距離をとる ---
	// TODO : 視界とか再現したい
	float length = Math::XMFloat3Length(playerPosition, enemyPosition);


	// --- 距離が一定以下なら攻撃 ---
	if (length < owner_->GetShortRange())
		return true;

	else
		return false;
}



// --- 探索行動の判定 ---
bool EnemyScoutJudgment::Judgment()
{
	return !owner_->IsFoundPlayer();
}


// --- 徘徊行動の判定 ---
bool EnemyWanderJudgment::Judgment()
{
	// --- 目標へのベクトル ---
	Vector3 moveVec = owner_->moveTargetPosition_ - owner_->GetPos();

	// --- 目標より遠かったら ---
	float length = moveVec.Length();
	if (length > 1.0f)
	{
		return true;
	}

	return false;
}

// --- 徘徊行動 ---
BT_ActionState EnemyWanderAction::Run(float elapsedTime)
{	
	// --- ダウン/死亡処理 ---
	if (owner_->IsDown() || owner_->IsDead())
		return BT_ActionState::Failed;



	switch (step)
	{
		// --- 初期設定 ---
	case 0:

		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::WALK_FOWARD), true);
		step++;
		break;


		// --- メイン処理 ---
	case 1:

		// --- 目標に向かって移動 ---
		Vector3 position = owner_->GetPos();
		Vector3 moveVec = owner_->moveTargetPosition_ - position;

		// --- 目標との距離が近くなったら ---
		if (moveVec.Length() < 1.0f)
		{
			step = 0;
			return BT_ActionState::Complete;
		}


		// --- 位置の更新 ---
		moveVec.Normalize();
		owner_->RotateToTargetVec(moveVec.vec_, 0.3f);
		moveVec *= owner_->walkSpeed_ * elapsedTime;
		position = position + moveVec;
		owner_->SetPos(position.vec_);


		// --- プレイヤーを見つけたら ---
		if (owner_->SearchPlayer())
		{
			step = 0;
			return BT_ActionState::Complete;
		}


		break;
	}

	return BT_ActionState::Run;
}


// --- 待機行動 次の移動地点を決める ---
BT_ActionState EnemyIdleAction::Run(float elapsedTime)
{
	// --- ダウン/死亡処理 ---
	if (owner_->IsDown() || owner_->IsDead())
		return BT_ActionState::Failed;



	switch (step)
	{
		// --- 初期設定 ---
	case 0:

		owner_->runTimer_ = (rand() / FLT_MAX) * 2.0f + 3.0f;	// 3.0 ~ 5.0
		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::IDLE), true);

		step++;
		break;


		// --- メイン処理 ---
	case 1:

		owner_->runTimer_ -= elapsedTime;

		// --- 待機時間を過ぎたら ---
		if (owner_->runTimer_ < 0.0f)
		{
			float theta = DirectX::XMConvertToRadians(rand() % 360);
			float wanderRange = owner_->GetWanderRange();
			owner_->moveTargetPosition_ = { cosf(theta) * wanderRange, 0.0f, sinf(theta) * wanderRange };

			owner_->runTimer_ = 0.0f;
			step = 0;
			return BT_ActionState::Complete;
		}


		// --- プレイヤーを見つけたら ---
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



// ==== 戦闘開始時の行動判定 ======================================================================================================================================================
bool EnemyBattleInitJudgment::Judgment()
{
	// --- すでに見つけていたら ---
	if (owner_->IsFoundPlayer())
		return false;


	// --- プレイヤーを見つけたら成功 ---
	if (owner_->SearchPlayer())
	{
		return true;
	}

	return false;
}



// ===== 遠距離の判定 ======================================================================================================================================================
bool EnemyLongRangeJudgment::Judgment()
{
	Vector3 playerPosition = Player::Instance().GetPos();
	Vector3 enemyPosition = owner_->GetPos();

	// --- プレイヤーとの距離をとる ---
	Vector3 vec = playerPosition - enemyPosition;
	float length = vec.Length();


	// --- 距離が一定以上なら成功 ---
	if (length > owner_->GetLongRange())
		return true;

	else
		return false;
}



// ===== 中距離の判定 ======================================================================================================================================================
bool EnemyMiddleRangeJudgment::Judgment()
{
	Vector3 playerPosition = Player::Instance().GetPos();
	Vector3 enemyPosition = owner_->GetPos();

	// --- プレイヤーとの距離をとる ---
	Vector3 vec = playerPosition - enemyPosition;
	float length = vec.Length();


	// --- 距離が一定以下なら成功 ---
	if (length < owner_->GetMiddleRange())
		return true;

	else
		return false;
}



// ===== 近距離の判定 ======================================================================================================================================================
bool EnemyShortRangeJudgment::Judgment()
{
	Vector3 playerPosition = Player::Instance().GetPos();
	Vector3 enemyPosition = owner_->GetPos();

	// --- プレイヤーとの距離をとる ---
	Vector3 vec = playerPosition - enemyPosition;
	float length = vec.Length();


	// --- 距離が一定以下なら成功 ---
	if (length < owner_->GetShortRange())
		return true;

	else
		return false;
}



// ===== 怯みの判定 ======================================================================================================================================================
bool EnemyFlinchJudgment::Judgment()
{
	return owner_->IsDown();
}



// ===== 死亡判定 ======================================================================================================================================================
bool EnemyDeadJudgment::Judgment()
{
	return owner_->IsDead();
}



// ===== 正面の判定 ======================================================================================================================================================
bool EnemyFrontJudgment::Judgment()
{
	Vector3 front = owner_->GetFrontVec();
	Vector3 vec = Player::Instance().GetPos();
	vec = vec - owner_->GetPos();
	vec.Normalize();
	float dot = front.Dot(vec);

	// --- 正面 ---
	if (dot > 0.75f)
	{
		return true;
	}

	return false;
}




// ===== 大咆哮の行動 ======================================================================================================================================================
BT_ActionState EnemyBigRoarAction::Run(float elapsedTime)
{
	// --- ダウン/死亡処理 ---
	if (owner_->IsDown() || owner_->IsDead())
		return BT_ActionState::Failed;


	switch (step)
	{
		// --- 初期設定 ---
	case 0:
		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::ROAR_BIG), false);
		owner_->SetFoundPlayer(true); // プレイヤーを発見

		step++;
		break;

		// --- メイン処理 ---
	case 1:

		// --- radialBlur 処理 ---
		owner_->bigRoarTimer += Timer::Instance().DeltaTime();

		// radialBlur ON OFF
		if(!owner_->radialBlur->GetIsRadial() && owner_->bigRoarRadialUpSampling.startTime < owner_->bigRoarTimer)
		{
			owner_->radialBlur->SetSamplingCount(0.0f);
			owner_->radialBlur->SetBlurPower(0.02f);
			owner_->radialBlur->SetIsRadial(true);
		}
		if(owner_->radialBlur->GetIsRadial() && owner_->bigRoarRadialDownSampling.endTime > owner_->bigRoarTimer)
		{
			owner_->radialBlur->SetIsRadial(false);
		}

		// sampling 増減処理
		owner_->radialBlur->SetSamplingCount(Easing::GetNowParam(Easing::OutQuad<float>, owner_->bigRoarTimer, owner_->bigRoarRadialUpSampling));
		if(owner_->bigRoarRadialDownSampling.startTime < owner_->bigRoarTimer)
		{
			owner_->radialBlur->SetSamplingCount(Easing::GetNowParam(Easing::OutQuad<float>, owner_->bigRoarTimer, owner_->bigRoarRadialDownSampling));
		}


		// --- アニメーションが終わったら ---
		if (owner_->GetAnimationEndFlag())
		{
			step = 0;
			return BT_ActionState::Complete;
		}

		break;
	}


	return BT_ActionState::Run;
}



// ===== 軸合わせの行動 ======================================================================================================================================================
BT_ActionState EnemyAxisAlignmentAction::Run(float elapsedTime)
{
	// --- ダウン/死亡処理 ---
	if (owner_->IsDown() || owner_->IsDead())
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
				owner_->PlayAnimation(static_cast<int>(MonsterAnimation::TURN_LEFT), false);

			else
				owner_->PlayAnimation(static_cast<int>(MonsterAnimation::TURN_RIGHT), false);

			owner_->turnAngle = owner_->targetVec.Dot(vec);


			// --- 目標に向いていたら ---
			float dot = front.Dot(vec);
			if (dot > 0.99f)
			{
				step = 0;
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

		// --- 回転前に内積を求めて、補完量を決める ---
		Matrix R;
		R.MakeRotationFromQuaternion(owner_->quaternion_);
		Vector3 front = R.v_[2].xyz();

		// --- -1.0 ~ 1.0 を 0.0 ~ 0.9に ---
		float dot = front.Dot(targetVec);
		dot += 1.0f;
		dot /= 2.0f;
		dot -= 0.1f;
		dot *= 0.05f;

		// --- 目標の方へ回転 ---
		owner_->RotateToTargetVec(targetVec.vec_, 0.1f + dot/*, &owner_->front*/);


		R.MakeRotationFromQuaternion(owner_->quaternion_);
		front = R.v_[2].xyz();

		// --- 目標に向いていたら ---
		dot = front.Dot(targetVec);
		if (dot > 0.99f)
		{
			step = 0;
			return BT_ActionState::Complete;
		}


		if (owner_->GetAnimationEndFlag())
		{
			step = 0;
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



// ===== ブレスの行動 ======================================================================================================================================================
BT_ActionState EnemyBlessAction::Run(float elapsedTime)
{
	// --- ダウン/死亡処理 ---
	if (owner_->IsDown() || owner_->IsDead())
		return BT_ActionState::Failed;



	switch (step)
	{
	case 0:

		owner_->runTimer_ = 1.5f;
		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::BLESS), false);

		step++;
		break;

	case 1:

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

		BreathEffect::Instance().SetPosition(Enemy::Instance().GetBonePosition("sitaago"));

		// --- アニメーションが終わったら終了 ---
		if (owner_->GetAnimationEndFlag())
		{
			step = 0;
			return BT_ActionState::Complete;
		}

		break;
	}

	return BT_ActionState::Run;
}



// ===== 威嚇行動 ======================================================================================================================================================
BT_ActionState EnemyThreatAction::Run(float elapsedTime)
{
	// --- ダウン/死亡処理 ---
	if (owner_->IsDown() || owner_->IsDead())
		return BT_ActionState::Failed;



	switch (step)
	{
	case 0:

		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::ROAR), false);

		step++;
		break;

	case 1:

		// --- アニメーションが終わったら終了 ---
		if (owner_->GetAnimationEndFlag())
		{
			step = 0;
			return BT_ActionState::Complete;
		}

		break;
	}

	return BT_ActionState::Run;
}



// ===== 突進行動 ======================================================================================================================================================
BT_ActionState EnemyRushAction::Run(float elapsedTime)
{
	// --- ダウン/死亡処理 ---
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


		// --- 突進始めの溜めの時間 ---
	case 1:

		owner_->runTimer_ -= elapsedTime;

		if (owner_->runTimer_ < 0.0f)
		{
			owner_->runTimer_ = owner_->GetWhileRushTimer();
			step++;
		}

		break;


		// --- 突進中 ---
	case 2:
	{
		// --- 正面へ移動 ---
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


	// --- 突進終わり ---
	case 3:
	{
		// --- 正面へ移動 ---
		Vector3 position = owner_->GetPos();
		position += owner_->targetVec * owner_->GetRushEndSpeed() * elapsedTime;
		owner_->SetPos(position.vec_);

		break;
	}

	}

	// --- アニメーションが終わったら終了 ---
	if (owner_->GetAnimationEndFlag())
	{
		step = 0;
		owner_->runTimer_ = 0.0f;
		return BT_ActionState::Complete;
	}


	return BT_ActionState::Run;
}



// ===== 踏みつけ行動 ======================================================================================================================================================
BT_ActionState EnemyStampAction::Run(float elapsedTime)
{
	// --- ダウン/死亡処理 ---
	if (owner_->IsDown() || owner_->IsDead())
		return BT_ActionState::Failed;



	switch (step)
	{
	case 0:

		owner_->runTimer_ = 0.75f;
		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::STAMP), false);

		step++;
		break;

	case 1:

		owner_->runTimer_ -= elapsedTime;
		if (owner_->runTimer_ < 0.0f)
		{
			owner_->PlayRockEffect();
			owner_->runTimer_ = 0.0f;
			step++;
		}

		break;


	case 2:

		// --- アニメーションが終わったら終了 ---
		if (owner_->GetAnimationEndFlag())
		{
			step = 0;
			return BT_ActionState::Complete;
		}

		break;
	}

	return BT_ActionState::Run;
}



// ===== 噛みつき行動 ======================================================================================================================================================
BT_ActionState EnemyBiteAction::Run(float elapsedTime)
{
	// --- ダウン/死亡処理 ---
	if (owner_->IsDown() || owner_->IsDead())
		return BT_ActionState::Failed;



	switch (step)
	{
	case 0:

		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::BITE_1), false);

		step++;
		break;

	case 1:

		// --- アニメーションが終わったら終了 ---
		if (owner_->GetAnimationEndFlag())
		{
			owner_->PlayAnimation(static_cast<int>(MonsterAnimation::BITE_2), false);
			step++;
		}

		break;

	case 2:

		// --- アニメーションが終わったら終了 ---
		if (owner_->GetAnimationEndFlag())
		{
			step = 0;
			return BT_ActionState::Complete;
		}

		break;
	}

	return BT_ActionState::Run;
}



// ===== 踏み込み噛みつき行動 ======================================================================================================================================================
BT_ActionState EnemyRushingBiteAction::Run(float elapsedTime)
{
	// --- ダウン/死亡処理 ---
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

	// --- アニメーションが終わったら終了 ---
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

	// --- アニメーションが終わったら終了 ---
	if (owner_->GetAnimationEndFlag())
	{
		step = 0;
		return BT_ActionState::Complete;
	}

	break;
	}

	return BT_ActionState::Run;
}



// ===== 踏み込み噛みつき後の行動 ======================================================================================================================================================
BT_ActionState EnemyAfterRushingBiteAction::Run(float elapsedTime)
{
	// --- ダウン/死亡処理 ---
	if (owner_->IsDown() || owner_->IsDead())
		return BT_ActionState::Failed;



	switch (step)
	{
	case 0:

		// --- この時点で 踏みつけ 左右タックル 威嚇 何もしない を決める ---
	{
		// --- 正面と右方向の取得 ---
		Matrix R;
		R.MakeRotationFromQuaternion(owner_->quaternion_);
		Vector3 front = R.v_[2].xyz();
		Vector3 right = R.v_[0].xyz();

		Vector3 playerPos = Player::Instance().GetPos();
		Vector3 vec = playerPos - owner_->GetPos();
		float length = vec.Length();
		vec.Normalize();


		// --- 距離が近かったら ---
		if (length < 10.0f)
		{
			owner_->PlayAnimation(static_cast<int>(MonsterAnimation::STAMP), false);
			step++;
			break;
		}

		// --- 右方向との内積 ---
		float dotR = right.Dot(vec);
		if (dotR > 0.5f)	// 横にいたらタックル
		{
			owner_->PlayAnimation(static_cast<int>(MonsterAnimation::TACKLE_RIGHT), false);
			step = 2;
			owner_->runTimer_ = 0.25f;
			break;
		}

		else if (dotR < -0.5f)
		{
			// --- 右タックル ---
			owner_->PlayAnimation(static_cast<int>(MonsterAnimation::TACKLE_LEFT), false);
			step = 3;
			owner_->runTimer_ = 0.25f;
			break;
		}


		float dotF = front.Dot(vec);
		if (dotF > 0.5f)	// 正面にいたら
		{
			return BT_ActionState::Complete;
		}

		else if (dotF < -0.5f)	// 後ろにいたら
		{
			owner_->PlayAnimation(static_cast<int>(MonsterAnimation::ROAR), false);
			step++;
			break;
		}
	}

	break;


	case 1:

		// --- アニメーションが終わったら終了 ---
		if (owner_->GetAnimationEndFlag())
		{
			step = 0;
			return BT_ActionState::Complete;
		}

		break;


	case 2:	// 右タックル
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

		// --- アニメーションが終わったら終了 ---
		if (owner_->GetAnimationEndFlag())
		{
			step = 0;
			owner_->runTimer_ = 0.0f;
			return BT_ActionState::Complete;
		}

		break;
	}


	case 3:	// 右タックル
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

		// --- アニメーションが終わったら終了 ---
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



// ===== 怯み行動 ======================================================================================================================================================
BT_ActionState EnemyFlinchAction::Run(float elapsedTime)
{
	// --- 死亡処理 ---
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
			owner_->SetFlinchValue(100.0f); // Todo : 怯み値仮
			return BT_ActionState::Complete;
		}

		break;
	}


	return BT_ActionState::Run;
}



// ===== 死亡行動 ======================================================================================================================================================
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



// ===== 尻尾回転行動 ======================================================================================================================================================
BT_ActionState EnemyTailAttack::Run(float elapsedTime)
{
	// --- ダウン/死亡処理 ---
	if (owner_->IsDown() || owner_->IsDead())
		return BT_ActionState::Failed;

	switch (step)
	{
	case 0:
		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::ROTATION), false);

		step++;
		break;


	case 1:

		if (owner_->GetAnimationEndFlag())
		{
			step = 0;
			return BT_ActionState::Complete;
		}

		break;
	}

	return BT_ActionState::Run;
}



// ===== 掬い上げ行動 ======================================================================================================================================================
BT_ActionState EnemyScoopUpAction::Run(float elapsedTime)
{	
	// --- ダウン/死亡処理 ---
	if (owner_->IsDown() || owner_->IsDead())
		return BT_ActionState::Failed;

	switch (step)
	{
	case 0:
		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::SCOOP_UP), false);
		owner_->runTimer_ = 0.5f;
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

		if (owner_->GetAnimationEndFlag())
		{
			step = 0;
			return BT_ActionState::Complete;
		}
		break;
	}

	}


	return BT_ActionState::Run;
}

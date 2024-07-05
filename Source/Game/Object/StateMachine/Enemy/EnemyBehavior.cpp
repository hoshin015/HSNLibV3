#include "EnemyBehavior.h"

#include "../../Library/Math/Math.h"

#include "../Player/Player.h"



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




// ===== 大咆哮の行動 ======================================================================================================================================================
BT_ActionState EnemyBigRoarAction::Run(float elapsedTime)
{
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
	switch(step)
	{
	case 0:

		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::IDLE), true);
		owner_->SetCurrentAnimationSeconds(0.0f);
		owner_->SetAnimationEndFlag(false);

		// --- 回転のために正面を取得 ---
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

		// --- 目標の方へ回転 ---
		owner_->RotateToTargetVec(targetVec.vec_, 0.1f/*, &owner_->front*/);


		Matrix R;
		R.MakeRotationFromQuaternion(owner_->quaternion_);
		Vector3 front = R.v_[2].xyz();

		// --- 目標に向いていたら ---
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



// ===== ブレスの行動 ======================================================================================================================================================
BT_ActionState EnemyBlessAction::Run(float elapsedTime)
{
	switch (step)
	{
	case 0:

		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::BLESS), false);	

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



// ===== 威嚇行動 ======================================================================================================================================================
BT_ActionState EnemyThreatAction::Run(float elapsedTime)
{
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
		position += owner_->front * owner_->GetRushSpeed() * elapsedTime;
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
		position += owner_->front * owner_->GetRushEndSpeed() * elapsedTime;
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
	switch (step)
	{
	case 0:

		owner_->PlayAnimation(static_cast<int>(MonsterAnimation::STAMP), false);

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



// ===== 噛みつき行動 ======================================================================================================================================================
BT_ActionState EnemyBiteAction::Run(float elapsedTime)
{
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

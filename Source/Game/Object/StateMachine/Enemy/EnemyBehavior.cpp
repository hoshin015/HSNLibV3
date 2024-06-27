#include "EnemyBehavior.h"

#include "../../Library/Math/Math.h"
#include "../../Library/Math/OperatorXMFloat3.h"

#include "../Player/Player.h"



// --- 追跡行動の判定 ---
bool EnemyPursuitJudgment::Judgment()
{
	DirectX::XMFLOAT3 playerPosition = Player::Instance().GetPos();
	DirectX::XMFLOAT3 enemyPosition = owner_->GetPos();

	// --- プレイヤーとの距離をとる ---
	// TODO : 視界とか再現したい
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

		//owner_->PlayAnimation(9/*Run*/, true);	// アニメーションの設定
		owner_->PlayAnimation(13/*Run*/, true);	// アニメーションの設定

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
	return owner_->SearchPlayer();
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
	if (length < 10.0f)
		return true;

	else
		return false;
}


// --- 攻撃行動 ---
BT_ActionState EnemyAttackAction::Run(float elapsedTime)
{
	switch (step)
	{
		// --- 初期設定 ---
	case 0:

		//owner_->PlayAnimation(0/*Attack*/, false);	// アニメーションの設定
		owner_->PlayAnimation(0/*Attack*/, false);	// アニメーションの設定
		owner_->SetCurrentAnimationSeconds(0.0f);
		owner_->SetAnimationEndFlag(false);

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


BT_ActionState EnemyAttackAction2::Run(float elapsedTime)
{
	switch (step)
	{
		// --- 初期設定 ---
	case 0:

		//owner_->PlayAnimation(1/*Attack2*/, false);	// アニメーションの設定
		owner_->PlayAnimation(1/*Attack2*/, false);	// アニメーションの設定
		owner_->SetCurrentAnimationSeconds(0.0f);
		owner_->SetAnimationEndFlag(false);

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



// --- 探索行動の判定 ---
bool EnemyScoutJudgment::Judgment()
{
	return !owner_->SearchPlayer();
}


// --- 徘徊行動の判定 ---
bool EnemyWanderJudgment::Judgment()
{
	// --- 目標へのベクトル ---
	DirectX::XMFLOAT3 moveVec = owner_->moveTargetPosition_ - owner_->GetPos();

	// --- 目標の距離が遠かったら ---
	if (Float3Length(moveVec) > 1.0f)
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

		//owner_->PlayAnimation(12/*Walk*/, true);
		owner_->PlayAnimation(17/*Walk*/, true);
		step++;
		break;


		// --- メイン処理 ---
	case 1:

		// --- 目標に向かって移動 ---
		DirectX::XMFLOAT3 moveVec = owner_->moveTargetPosition_ - owner_->GetPos();

		// --- 目標との距離が近くなったら ---
		if (Float3Length(moveVec) < 1.0f)
		{
			step = 0;
			return BT_ActionState::Complete;
		}


		// --- 位置の更新 ---
		moveVec = Math::XMFloat3Normalize(moveVec);
		owner_->RotateToTargetVec(moveVec, 0.3f);
		moveVec *= owner_->walkSpeed_ * elapsedTime;
		DirectX::XMFLOAT3 position = owner_->GetPos() + moveVec;
		owner_->SetPos(position);


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
		//owner_->PlayAnimation(6/*Idle*/, true);
		owner_->PlayAnimation(10/*Idle*/, true);

			step++;
		break;


		// --- メイン処理 ---
	case 1:

		owner_->runTimer_ -= elapsedTime;

		// --- 待機時間を過ぎたら ---
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

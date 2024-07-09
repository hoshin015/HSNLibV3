#include "Enemy.h"

#include "../../External/ImGui/imgui.h"

#include "../../Library/3D/DebugPrimitive.h"
#include "../../Library/3D/CameraManager.h"

#include "../../Library/Timer.h"
#include "../../Library/Math/Math.h"

#include "../../Library/Input/InputManager.h"

#include "../../Effect/Rock/RockEffect.h"

#include "EnemyBehavior.h"

#include "../Player/Player.h"


Enemy::~Enemy()
{
}

void Enemy::Initialize()
{
	// --- ビヘイビアツリーの初期化   子ノードの追加やアクションノードの設定など ---
	InitializeBehaviorTree();

	position = { 0, 0, 0 };
	Vector3 s = scale;
	s *= 1.15f;
	scale = s.vec_;

	foundPlayer = false;

	float theta = DirectX::XMConvertToRadians(rand() % 360);
	moveTargetPosition_ = { cosf(theta) * wanderRange, 0.0f, sinf(theta) * wanderRange };
	targetVec = Vector3::Zero_;
	turnAngle = 0.0f;


	// --- ステータスの設定 ---
	hp = 1000.0f;
	flinchValue = 100.0f;

	alive = true;


	PlayAnimation(static_cast<int>(MonsterAnimation::WALK_FOWARD), true);
}

void Enemy::Finalize()
{
	FinalizeBehaviorTree();
}


void Enemy::Update()
{
	// --- ビヘイビアツリーの更新 ---
	UpdateBehaviorTree(Timer::Instance().DeltaTime());

	// アニメーション更新
	UpdateAnimation();

	// 姿勢行列更新
	//UpdateTransform();

	// --- 位置の制限 ---
	ClampPosition(75.0f);

	if (alive && hp < 0.0f)
	{
		OnDead();
	}


	if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::P))
	{
		PlayRockEffect();
	}


	// スケール変更
	Matrix MS;
	Vector3 modelScale = Vector3::Unit_ * model->GetModelResource()->GetScale();
	MS.MakeScaling(modelScale);

	// 軸変換行列
	Matrix C = model->GetModelResource()->GetCoordinateSystemTransform();

	// スケール行列を作成
	Matrix S;
	S.MakeScaling(GetScale());
	// 回転行列を作成
	Matrix R;
	R.MakeRotationFromQuaternion(quaternion_);
	// 位置行列を作成
	Matrix T;
	T.MakeTranslation(GetPos());

	// ４つの行列を組み合わせ、ワールド行列を作成
	Matrix W = MS * S * R * T * C;
	transform = W.mat_;
}


void Enemy::Render(bool isShadow)
{
	model->Render(transform, &keyFrame, isShadow);
}


void Enemy::ShowNode(NodeBase<Enemy>* node, std::string nodeName)
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode(nodeName.c_str()))
	{
		for (auto& childNode : node->children)
		{
			std::string name = childNode->GetName();
			if (activeNode_)
			{
				if (name == activeNode_->GetName())
					name += u8" : 実行中";
			}

			ShowNode(childNode, name);
		}

		ImGui::TreePop();
	}
}


// --- 死亡したときに呼ばれる ---
void Enemy::OnDead()
{
	CameraManager::Instance().SetCurrentCamera("EnemyDeadCamera");

	alive = false;
}


// --- デバッグGui描画 ---
void Enemy::DrawDebugGui()
{
	ImGui::Begin(u8"ビヘイビアツリー");

	// --- 実行中のノード ---
	ImGui::Text(activeNode_ ? activeNode_->GetName().c_str() : u8"ノードなし");


	// --- ビヘイビアツリーのデバッグ ---
	auto* root = aiTree_->GetRoot();
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode(root->GetName().c_str()))
	{
		for (auto& node : root->children)
		{
			ShowNode(node, node->GetName());
		}

		ImGui::TreePop();
	}

	ImGui::End();


	ImGui::Begin(u8"敵");

	ImGui::DragFloat(u8"HP", &hp, 1.0f);
	ImGui::DragFloat(u8"怯み値", &flinchValue, 1.0f);
	ImGui::Separator();

	ImGui::DragFloat(u8"索敵距離", &searchRange_);
	ImGui::Checkbox(u8"発見", &foundPlayer);
	ImGui::DragFloat3(u8"スケール", &scale.x);
	ImGui::DragFloat(u8"歩く速度", &walkSpeed_, 0.01f);
	ImGui::DragFloat(u8"走る速度", &runSpeed_, 0.01f);
	ImGui::DragFloat3(u8"徘徊目標", &moveTargetPosition_.x);

	ImGui::Separator();
	ImGui::DragFloat(u8"突進スピード", &rushSpeed, 0.01f);
	ImGui::DragFloat(u8"突進終わりスピード", &rushEndSpeed, 0.01f);
	ImGui::DragFloat(u8"突進溜め時間", &rushChargeTimer, 0.01f);
	ImGui::DragFloat(u8"突進中の時間", &whileRushTimer, 0.01f);

	ImGui::Separator();
	ImGui::DragFloat(u8"遠距離範囲", &longRange, 0.01f);

	ImGui::End();
}


void Enemy::DrawDebug()
{
	DebugPrimitive::Instance().AddCylinder(position, searchRange_, 1.0f, { 1.0f, 0.0f, 1.0f, 1.0f });
	DebugPrimitive::Instance().AddCylinder({}, wanderRange, 5.0f, { 0.3f, 0.3f, 1.0f, 1.0f });
	DebugPrimitive::Instance().AddCylinder(position, shortRange, 5.0f, { 1.0f, 0.0f, 0.0f, 1.0f });
	DebugPrimitive::Instance().AddCylinder(position, longRange, 5.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
	DebugPrimitive::Instance().AddSphere(moveTargetPosition_.vec_, 1.0f, { 0.3f, 0.3f, 1.0f, 1.0f });
}


// --- プレイヤーを見つけたか ---
bool Enemy::SearchPlayer()
{
	// --- プレイヤーとの距離の計算 ---
	const Vector3& playerPosition = Player::Instance().GetPos();
	Vector3 toPlayerVec = playerPosition - position;
	float length = toPlayerVec.Length();

	// --- 距離が探知範囲内なら ---
	if (length < searchRange_)
	{
		// --- 正面ベクトルを取得 ---
		Matrix R;
		R.MakeRotationFromQuaternion(quaternion_);
		Vector3 front = R.v_[2].xyz();
		Vector3 vec = Vector3::Normalize(toPlayerVec);
		float dot = front.Dot(vec);

		if (dot > 0.0f)
		{
			return true;
		}
	}

	// --- 見つからなかった ---
	return false;
}


// --- 目標のベクトルへ回転 ---
void Enemy::RotateToTargetVec(const DirectX::XMFLOAT3& targetVec, float t, const Vector3* tempFront)
{
	Matrix R;
	R.MakeRotationFromQuaternion(quaternion_);
	Vector3 front = R.v_[2].xyz();	// 正面を取得

	if (tempFront)
		front = *tempFront;
	
	Vector3 cross = front.Cross(targetVec);

	float dot = front.Dot(targetVec);
	if (dot != 1.0f)
	{
		float angle = acosf(dot);

		Quaternion rot;
		rot.SetRotation(cross, angle);
		if (angle > 1e-8f)
		{
			Quaternion dst = rot * quaternion_;
			quaternion_ = quaternion_.Lerp(quaternion_, dst, t);
		}
	}
}


// --- 位置の制限 ---
void Enemy::ClampPosition(float range)
{
	Vector3 pos = position;
	float length = pos.Length();
	if (length > range)
	{
		pos.Normalize();
		pos *= range;
		position = pos.vec_;
	}
}


void Enemy::PlayRockEffect()
{
	for (int i = 0; i < 10; i++)
	{
		DirectX::XMFLOAT3 rPos = { (rand() % 10) - 5.0f, 0, rand() % 10 - 5.0f };
		DirectX::XMFLOAT3 rVec = { (rand() % 10) - 5.0f, 5, rand() % 10 - 5.0f };
		Vector3 position = GetPos();
		position += rPos;
		RockEffect::Instance().Emit(position.vec_, rVec, { 0.1, 1 });
	}
}



// --- ビヘイビアツリーの初期化 ---
void Enemy::InitializeBehaviorTree()
{
	behaviorData_ = new BehaviorData<Enemy>();
	aiTree_ = new BehaviorTree<Enemy>();


	// --- 大元のノード ---
	aiTree_->AddNode("", "Root", 0, BT_SelectRule::Priority, nullptr, nullptr);


	// --- 最初の発見時 ---
	aiTree_->AddNode("Root", "Find", 0, BT_SelectRule::Sequence, new EnemyBattleInitJudgment(this), nullptr);
	{
		aiTree_->AddNode("Find", "AxisAlignment", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
		aiTree_->AddNode("Find", "BigRoar", 0, BT_SelectRule::Non, nullptr, new EnemyBigRoarAction(this));
	}

	// --- 死亡 ---
	aiTree_->AddNode("Root", "Dead", 1, BT_SelectRule::Non, new EnemyDeadJudgment(this), new EnemyDeadAction(this));

	// --- 怯み ---
	aiTree_->AddNode("Root", "Flinch", 2, BT_SelectRule::Non, new EnemyFlinchJudgment(this), new EnemyFlinchAction(this));


	// --- 戦闘ノード ---
	aiTree_->AddNode("Root", "Battle", 3, BT_SelectRule::Priority, new EnemyBattleJudgment(this), nullptr);
	{
		// --- 攻撃ノード ---
		aiTree_->AddNode("Battle", "Attack", 0, BT_SelectRule::Random, nullptr, nullptr);
		{
			// --- 遠距離 ---
			aiTree_->AddNode("Attack", "LongRange", 0, BT_SelectRule::Random, /*new EnemyLongRangeJudgment(this)*/nullptr, nullptr);
			{
				// --- ブレス → 威嚇 ---
				aiTree_->AddNode("LongRange", "Bless_Threat", 0, BT_SelectRule::Sequence, nullptr, nullptr);
				{
					aiTree_->AddNode("Bless_Threat", "AxisAlignment", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
					aiTree_->AddNode("Bless_Threat", "Bless", 0, BT_SelectRule::Non, nullptr, new EnemyBlessAction(this));
					aiTree_->AddNode("Bless_Threat", "Threat", 0, BT_SelectRule::Non, nullptr, new EnemyThreatAction(this));
				}

				// --- 突進 → 威嚇 ---
				aiTree_->AddNode("LongRange", "Rush_Threat", 0, BT_SelectRule::Sequence, nullptr, nullptr);
				{
					aiTree_->AddNode("Rush_Threat", "AxisAlignment", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
					aiTree_->AddNode("Rush_Threat", "Rush", 0, BT_SelectRule::Non, nullptr, new EnemyRushAction(this));
					aiTree_->AddNode("Rush_Threat", "Threat", 0, BT_SelectRule::Non, nullptr, new EnemyThreatAction(this));
				}

				// --- 普通に接近 ---
				aiTree_->AddNode("LongRange", "Pursuit", 0, BT_SelectRule::Non, nullptr, new EnemyPursuitAction(this));
			}


			// --- 中距離 ---
			aiTree_->AddNode("Attack", "MiddleRange", 0, BT_SelectRule::Random, new EnemyMiddleRangeJudgment(this), nullptr);
			{
				// --- 軸合わせ → 踏み込み噛みつき → 踏みつけ or タックル or 威嚇 ---
				aiTree_->AddNode("MiddleRange", "Turn_RushingBite_Any", 0, BT_SelectRule::Sequence, nullptr, nullptr);
				{
					aiTree_->AddNode("Turn_RushingBite_Any", "Turn", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
					aiTree_->AddNode("Turn_RushingBite_Any", "RushingBite", 0, BT_SelectRule::Non, nullptr, new EnemyRushingBiteAction(this));
					aiTree_->AddNode("Turn_RushingBite_Any", "AfterAction", 0, BT_SelectRule::Non, nullptr, new EnemyAfterRushingBiteAction(this));
				}
			}


			// --- 近距離 ---
			aiTree_->AddNode("Attack", "ShortRange", 0, BT_SelectRule::Random, new EnemyShortRangeJudgment(this), nullptr);
			{
				// --- 踏みつけ → 威嚇 ---
				aiTree_->AddNode("ShortRange", "Stamp_Threat", 0, BT_SelectRule::Sequence, nullptr, nullptr);
				{
					aiTree_->AddNode("Stamp_Threat", "Stamp", 0, BT_SelectRule::Non, nullptr, new EnemyStampAction(this));
					aiTree_->AddNode("Stamp_Threat", "Threat", 0, BT_SelectRule::Non, nullptr, new EnemyThreatAction(this));
				}

				// --- 軸合わせ → 噛みつき ---
				aiTree_->AddNode("ShortRange", "AxisAlignment_Bite", 0, BT_SelectRule::Sequence, nullptr, nullptr);
				{
					aiTree_->AddNode("AxisAlignment_Bite", "AxisAlignment", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
					aiTree_->AddNode("AxisAlignment_Bite", "Bite", 0, BT_SelectRule::Non, nullptr, new EnemyBiteAction(this));
				}

				// --- 尻尾回転 ---
				aiTree_->AddNode("ShortRange", "TailAttack", 0, BT_SelectRule::Non, nullptr, new EnemyTailAttack(this));
			}
		}


		// --- 追跡ノード ---
		aiTree_->AddNode("Battle", "Pursuit", 1, BT_SelectRule::Non, nullptr, new EnemyPursuitAction(this));
	}


	// --- 未発見時ノード ---
	aiTree_->AddNode("Root", "Scout", 4, BT_SelectRule::Sequence, new EnemyScoutJudgment(this), nullptr);
	{
		// --- 徘徊 ---
		aiTree_->AddNode("Scout", "Wander", 0, BT_SelectRule::Non, new EnemyWanderJudgment(this), new EnemyWanderAction(this));
		
		// --- 待機 ---
		aiTree_->AddNode("Scout", "Idle", 0, BT_SelectRule::Non, nullptr, new EnemyIdleAction(this));
	}
}

// --- 終了処理 ---
void Enemy::FinalizeBehaviorTree()
{
	if(behaviorData_)
	{
		delete behaviorData_;
		behaviorData_ = nullptr;
	}

	if(aiTree_)
	{
		delete aiTree_;
		aiTree_ = nullptr;
	}

	activeNode_ = nullptr;
}


void Enemy::UpdateBehaviorTree(float elapsedTime)
{
	// --- 実行中のノードがなければ ---
	if (!activeNode_)
	{
		// --- 次の実行ノードを推論 ---
		activeNode_ = aiTree_->ActiveNodeInference(behaviorData_);
	}

	// --- 実行ノードがあれば ---
	if (activeNode_)
	{
		activeNode_ = aiTree_->Run(activeNode_, behaviorData_, elapsedTime);
	}
}

void Enemy::DrawDebugImGui(int number)
{
	ImGui::Begin("Enemy");
	{
		ImGui::DragFloat("emissive", &GetModel()->data.emissivePower, 0.01f);
		ImGui::SliderFloat("roughness", &GetModel()->data.roughnessPower, -1.0f, 1.0f);
		ImGui::SliderFloat("metalness", &GetModel()->data.metalnessPower, -1.0f, 1.0f);
	}
	ImGui::End();
}
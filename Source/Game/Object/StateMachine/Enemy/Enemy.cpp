#include "Enemy.h"

#include "../../External/ImGui/imgui.h"

#include "../../Library/3D/DebugPrimitive.h"

#include "../../Library/Timer.h"
#include "../../Library/Math/Math.h"

#include "EnemyBehavior.h"

#include "../Player/Player.h"


Enemy::~Enemy()
{
	// --- ビヘイビアツリーの終了処理 ---
	FinalizeBehaviorTree();
}


void Enemy::Initialize()
{
	// --- ビヘイビアツリーの初期化   子ノードの追加やアクションノードの設定など ---
	InitializeBehaviorTree();

	position = { 0, 0, -15 };

	//PlayAnimation(12/*Walk*/, true);
	PlayAnimation(0/*Walk*/, true);
}


void Enemy::Update()
{
	// --- ビヘイビアツリーの更新 ---
	UpdateBehaviorTree(Timer::Instance().DeltaTime());

	// アニメーション更新
	UpdateAnimation();

	// 姿勢行列更新
	//UpdateTransform();



		// スケール変更
	DirectX::XMMATRIX MS = DirectX::XMMatrixScaling(model->GetModelResource()->GetScale(),
		model->GetModelResource()->GetScale(),
		model->GetModelResource()->GetScale());

	// 軸変換行列
	DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&model->GetModelResource()->GetCoordinateSystemTransform());

	// スケール行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(GetScaleX(), GetScaleY(), GetScaleZ());
	// 回転行列を作成
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&quaternion_));
	// 位置行列を作成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(GetPosX(), GetPosY(), GetPosZ());

	// ４つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX W = MS * S * R * T * C;
	DirectX::XMStoreFloat4x4(&transform, W);
}


void Enemy::Render(bool isShadow)
{
	model->Render(transform, &keyFrame, isShadow);
}


// --- デバッグGui描画 ---
void Enemy::DrawDebugGui()
{
	ImGui::Begin(u8"敵");

	// --- 実行中のノード ---
	ImGui::Text(activeNode_ ? activeNode_->GetName().c_str() : u8"ノードなし");

	// --- ビヘイビアツリーのデバッグ ---
	auto* root = aiTree_->GetRoot();
	if (ImGui::TreeNode(root->GetName().c_str()))
	{
		for (auto& node : root->children)
		{
			std::string nodeName = node->GetName();

			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::TreeNode(nodeName.c_str()))
			{
				for (auto* childNode : node->children)
				{
					std::string childNodeName = childNode->GetName();
					if (activeNode_)
						if (childNodeName == activeNode_->GetName())
							childNodeName += u8" : 実行中";

					ImGui::SetNextItemOpen(true, ImGuiCond_Once);
					if (ImGui::TreeNode(childNodeName.c_str()))
					{
						for (auto* c : childNode->children)
						{
							std::string n = c->GetName();
							if (activeNode_)
								if (n == activeNode_->GetName())
									n += u8" : 実行中";

							ImGui::SetNextItemOpen(true, ImGuiCond_Once);
							if (ImGui::TreeNode(n.c_str()))
							{
								ImGui::TreePop();
							}
						}

						ImGui::TreePop();
					}

				}

				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}

	ImGui::DragFloat(u8"索敵距離", &searchRange_);
	bool IsSearch = SearchPlayer();
	ImGui::Checkbox(u8"発見", &IsSearch);
	ImGui::DragFloat3(u8"スケール", &scale.x);
	ImGui::DragFloat(u8"歩く速度", &walkSpeed_, 0.01f);
	ImGui::DragFloat(u8"走る速度", &runSpeed_, 0.01f);

	ImGui::End();
}


void Enemy::DrawDebug()
{
	DebugPrimitive::Instance().AddCylinder(position, searchRange_, 1.0f, { 0.3f, 0.3f, 1.0f, 1.0f });
	DebugPrimitive::Instance().AddSphere(moveTargetPosition_, 1.0f, { 0.3f, 0.3f, 1.0f, 1.0f });
}


// --- プレイヤーを見つけたか ---
bool Enemy::SearchPlayer()
{
	// --- プレイヤーとの距離の計算 ---
	const DirectX::XMFLOAT3& playerPosition = Player::Instance().GetPos();
	DirectX::XMFLOAT3 toPlayerVec = playerPosition - position;
	float length = Float3Length(toPlayerVec);

	// --- 距離が探知範囲内なら ---
	if (length < searchRange_)
	{
		// --- 正面ベクトルを取得 ---
		DirectX::XMVECTOR F = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&quaternion_)).r[2];
		DirectX::XMVECTOR V = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&toPlayerVec));
		float dot = DirectX::XMVector3Dot(F, V).m128_f32[0];

		if (dot > 0.0f)
		{
			return true;
		}
	}

	return false;
}


void Enemy::RotateToTargetVec(const DirectX::XMFLOAT3& targetVec, float t)
{
	DirectX::XMMATRIX R;
	R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&quaternion_));
	DirectX::XMVECTOR F = R.r[2];	// 正面を取得

	DirectX::XMVECTOR V = DirectX::XMLoadFloat3(&targetVec);
	DirectX::XMVECTOR C = DirectX::XMVector3Cross(F, V);

	float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(F, V));
	if (dot != 1.0f)
	{
		float angle = acosf(dot);

		DirectX::XMVECTOR rot = DirectX::XMQuaternionRotationAxis(C, angle);
		DirectX::XMVECTOR Q = DirectX::XMLoadFloat4(&quaternion_);
		if (angle > 1e-8f)
		{
			DirectX::XMVECTOR rotation = DirectX::XMQuaternionMultiply(rot, Q);
			DirectX::XMStoreFloat4(&quaternion_, DirectX::XMQuaternionSlerp(Q, rotation, t));
		}
	}
}



// --- ビヘイビアツリーの初期化 ---
void Enemy::InitializeBehaviorTree()
{
	behaviorData_ = new BehaviorData<Enemy>();
	aiTree_ = new BehaviorTree<Enemy>();

	// --- ツリーの構築 ---
	// 								     |---Attack1
	//		   				|---Attack---|
	//		   |---Battle---|		     |---Attack2
	// 		   |			|
	// 		   |			|
	//		   |			|---Pursuit
	// 		   |
	// Root ---|
	// 		   |		   |---Wander
	//		   |---Scout---|
	// 		   			   |---Idle
	//


	// --- 大元のノード ---
	aiTree_->AddNode("", "Root", 0, BT_SelectRule::Priority, nullptr, nullptr);

	// --- 戦闘ノード ---
	{
		aiTree_->AddNode("Root", "Battle", 0, BT_SelectRule::Priority, new EnemyBattleJudgment(this), nullptr);

		// --- 攻撃ノード ---
		{
			aiTree_->AddNode("Battle", "Attack", 0, BT_SelectRule::Random, new EnemyAttackJudgment(this), nullptr);

			// --- 攻撃の種類 ---
			{
				aiTree_->AddNode("Attack", "Attack1", 0, BT_SelectRule::Non, nullptr, new EnemyAttackAction(this));
				aiTree_->AddNode("Attack", "Attack2", 0, BT_SelectRule::Non, nullptr, new EnemyAttackAction2(this));
			}
		}

		// --- 追跡ノード ---
		{
			aiTree_->AddNode("Battle", "Pursuit", 1, BT_SelectRule::Non, nullptr, new EnemyPursuitAction(this));
		}
	}

	// --- 探索ノード ---
	{
		aiTree_->AddNode("Root", "Scout", 0, BT_SelectRule::Sequence, new EnemyScoutJudgment(this), nullptr);

		{
			aiTree_->AddNode("Scout", "Wander", 0, BT_SelectRule::Non, new EnemyWanderJudgment(this), new EnemyWanderAction(this));
			aiTree_->AddNode("Scout", "Idle", 0, BT_SelectRule::Non, nullptr, new EnemyIdleAction(this));
		}
	}
}

// --- 終了処理 ---
void Enemy::FinalizeBehaviorTree()
{
	delete behaviorData_;
	delete aiTree_;
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

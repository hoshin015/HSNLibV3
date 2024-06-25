#include "Enemy.h"

#include "../../External/ImGui/imgui.h"

#include "../../Library/3D/DebugPrimitive.h"

#include "../../Library/Timer.h"
#include "../../Library/Math/Math.h"

#include "EnemyBehavior.h"

#include "../Player/Player.h"


Enemy::~Enemy()
{
	// --- �r�w�C�r�A�c���[�̏I������ ---
	FinalizeBehaviorTree();
}


void Enemy::Initialize()
{
	// --- �r�w�C�r�A�c���[�̏�����   �q�m�[�h�̒ǉ���A�N�V�����m�[�h�̐ݒ�Ȃ� ---
	InitializeBehaviorTree();

	position = { 0, 0, -15 };

	//PlayAnimation(12/*Walk*/, true);
	PlayAnimation(0/*Walk*/, true);
}


void Enemy::Update()
{
	// --- �r�w�C�r�A�c���[�̍X�V ---
	UpdateBehaviorTree(Timer::Instance().DeltaTime());

	// �A�j���[�V�����X�V
	UpdateAnimation();

	// �p���s��X�V
	//UpdateTransform();



		// �X�P�[���ύX
	DirectX::XMMATRIX MS = DirectX::XMMatrixScaling(model->GetModelResource()->GetScale(),
		model->GetModelResource()->GetScale(),
		model->GetModelResource()->GetScale());

	// ���ϊ��s��
	DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&model->GetModelResource()->GetCoordinateSystemTransform());

	// �X�P�[���s����쐬
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(GetScaleX(), GetScaleY(), GetScaleZ());
	// ��]�s����쐬
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&quaternion_));
	// �ʒu�s����쐬
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(GetPosX(), GetPosY(), GetPosZ());

	// �S�̍s���g�ݍ��킹�A���[���h�s����쐬
	DirectX::XMMATRIX W = MS * S * R * T * C;
	DirectX::XMStoreFloat4x4(&transform, W);
}


void Enemy::Render(bool isShadow)
{
	model->Render(transform, &keyFrame, isShadow);
}


// --- �f�o�b�OGui�`�� ---
void Enemy::DrawDebugGui()
{
	ImGui::Begin(u8"�G");

	// --- ���s���̃m�[�h ---
	ImGui::Text(activeNode_ ? activeNode_->GetName().c_str() : u8"�m�[�h�Ȃ�");

	// --- �r�w�C�r�A�c���[�̃f�o�b�O ---
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
							childNodeName += u8" : ���s��";

					ImGui::SetNextItemOpen(true, ImGuiCond_Once);
					if (ImGui::TreeNode(childNodeName.c_str()))
					{
						for (auto* c : childNode->children)
						{
							std::string n = c->GetName();
							if (activeNode_)
								if (n == activeNode_->GetName())
									n += u8" : ���s��";

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

	ImGui::DragFloat(u8"���G����", &searchRange_);
	bool IsSearch = SearchPlayer();
	ImGui::Checkbox(u8"����", &IsSearch);
	ImGui::DragFloat3(u8"�X�P�[��", &scale.x);
	ImGui::DragFloat(u8"�������x", &walkSpeed_, 0.01f);
	ImGui::DragFloat(u8"���鑬�x", &runSpeed_, 0.01f);

	ImGui::End();
}


void Enemy::DrawDebug()
{
	DebugPrimitive::Instance().AddCylinder(position, searchRange_, 1.0f, { 0.3f, 0.3f, 1.0f, 1.0f });
	DebugPrimitive::Instance().AddSphere(moveTargetPosition_, 1.0f, { 0.3f, 0.3f, 1.0f, 1.0f });
}


// --- �v���C���[���������� ---
bool Enemy::SearchPlayer()
{
	// --- �v���C���[�Ƃ̋����̌v�Z ---
	const DirectX::XMFLOAT3& playerPosition = Player::Instance().GetPos();
	DirectX::XMFLOAT3 toPlayerVec = playerPosition - position;
	float length = Float3Length(toPlayerVec);

	// --- �������T�m�͈͓��Ȃ� ---
	if (length < searchRange_)
	{
		// --- ���ʃx�N�g�����擾 ---
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
	DirectX::XMVECTOR F = R.r[2];	// ���ʂ��擾

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



// --- �r�w�C�r�A�c���[�̏����� ---
void Enemy::InitializeBehaviorTree()
{
	behaviorData_ = new BehaviorData<Enemy>();
	aiTree_ = new BehaviorTree<Enemy>();

	// --- �c���[�̍\�z ---
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


	// --- �匳�̃m�[�h ---
	aiTree_->AddNode("", "Root", 0, BT_SelectRule::Priority, nullptr, nullptr);

	// --- �퓬�m�[�h ---
	{
		aiTree_->AddNode("Root", "Battle", 0, BT_SelectRule::Priority, new EnemyBattleJudgment(this), nullptr);

		// --- �U���m�[�h ---
		{
			aiTree_->AddNode("Battle", "Attack", 0, BT_SelectRule::Random, new EnemyAttackJudgment(this), nullptr);

			// --- �U���̎�� ---
			{
				aiTree_->AddNode("Attack", "Attack1", 0, BT_SelectRule::Non, nullptr, new EnemyAttackAction(this));
				aiTree_->AddNode("Attack", "Attack2", 0, BT_SelectRule::Non, nullptr, new EnemyAttackAction2(this));
			}
		}

		// --- �ǐՃm�[�h ---
		{
			aiTree_->AddNode("Battle", "Pursuit", 1, BT_SelectRule::Non, nullptr, new EnemyPursuitAction(this));
		}
	}

	// --- �T���m�[�h ---
	{
		aiTree_->AddNode("Root", "Scout", 0, BT_SelectRule::Sequence, new EnemyScoutJudgment(this), nullptr);

		{
			aiTree_->AddNode("Scout", "Wander", 0, BT_SelectRule::Non, new EnemyWanderJudgment(this), new EnemyWanderAction(this));
			aiTree_->AddNode("Scout", "Idle", 0, BT_SelectRule::Non, nullptr, new EnemyIdleAction(this));
		}
	}
}

// --- �I������ ---
void Enemy::FinalizeBehaviorTree()
{
	delete behaviorData_;
	delete aiTree_;
}


void Enemy::UpdateBehaviorTree(float elapsedTime)
{
	// --- ���s���̃m�[�h���Ȃ���� ---
	if (!activeNode_)
	{
		// --- ���̎��s�m�[�h�𐄘_ ---
		activeNode_ = aiTree_->ActiveNodeInference(behaviorData_);
	}

	// --- ���s�m�[�h������� ---
	if (activeNode_)
	{
		activeNode_ = aiTree_->Run(activeNode_, behaviorData_, elapsedTime);
	}
}

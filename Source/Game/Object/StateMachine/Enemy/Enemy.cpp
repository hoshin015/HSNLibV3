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
	// --- �r�w�C�r�A�c���[�̏�����   �q�m�[�h�̒ǉ���A�N�V�����m�[�h�̐ݒ�Ȃ� ---
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


	// --- �X�e�[�^�X�̐ݒ� ---
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
	// --- �r�w�C�r�A�c���[�̍X�V ---
	UpdateBehaviorTree(Timer::Instance().DeltaTime());

	// �A�j���[�V�����X�V
	UpdateAnimation();

	// �p���s��X�V
	//UpdateTransform();

	// --- �ʒu�̐��� ---
	ClampPosition(75.0f);

	if (alive && hp < 0.0f)
	{
		OnDead();
	}


	if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::P))
	{
		PlayRockEffect();
	}


	// �X�P�[���ύX
	Matrix MS;
	Vector3 modelScale = Vector3::Unit_ * model->GetModelResource()->GetScale();
	MS.MakeScaling(modelScale);

	// ���ϊ��s��
	Matrix C = model->GetModelResource()->GetCoordinateSystemTransform();

	// �X�P�[���s����쐬
	Matrix S;
	S.MakeScaling(GetScale());
	// ��]�s����쐬
	Matrix R;
	R.MakeRotationFromQuaternion(quaternion_);
	// �ʒu�s����쐬
	Matrix T;
	T.MakeTranslation(GetPos());

	// �S�̍s���g�ݍ��킹�A���[���h�s����쐬
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
					name += u8" : ���s��";
			}

			ShowNode(childNode, name);
		}

		ImGui::TreePop();
	}
}


// --- ���S�����Ƃ��ɌĂ΂�� ---
void Enemy::OnDead()
{
	CameraManager::Instance().SetCurrentCamera("EnemyDeadCamera");

	alive = false;
}


// --- �f�o�b�OGui�`�� ---
void Enemy::DrawDebugGui()
{
	ImGui::Begin(u8"�r�w�C�r�A�c���[");

	// --- ���s���̃m�[�h ---
	ImGui::Text(activeNode_ ? activeNode_->GetName().c_str() : u8"�m�[�h�Ȃ�");


	// --- �r�w�C�r�A�c���[�̃f�o�b�O ---
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


	ImGui::Begin(u8"�G");

	ImGui::DragFloat(u8"HP", &hp, 1.0f);
	ImGui::DragFloat(u8"���ݒl", &flinchValue, 1.0f);
	ImGui::Separator();

	ImGui::DragFloat(u8"���G����", &searchRange_);
	ImGui::Checkbox(u8"����", &foundPlayer);
	ImGui::DragFloat3(u8"�X�P�[��", &scale.x);
	ImGui::DragFloat(u8"�������x", &walkSpeed_, 0.01f);
	ImGui::DragFloat(u8"���鑬�x", &runSpeed_, 0.01f);
	ImGui::DragFloat3(u8"�p�j�ڕW", &moveTargetPosition_.x);

	ImGui::Separator();
	ImGui::DragFloat(u8"�ːi�X�s�[�h", &rushSpeed, 0.01f);
	ImGui::DragFloat(u8"�ːi�I���X�s�[�h", &rushEndSpeed, 0.01f);
	ImGui::DragFloat(u8"�ːi���ߎ���", &rushChargeTimer, 0.01f);
	ImGui::DragFloat(u8"�ːi���̎���", &whileRushTimer, 0.01f);

	ImGui::Separator();
	ImGui::DragFloat(u8"�������͈�", &longRange, 0.01f);

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


// --- �v���C���[���������� ---
bool Enemy::SearchPlayer()
{
	// --- �v���C���[�Ƃ̋����̌v�Z ---
	const Vector3& playerPosition = Player::Instance().GetPos();
	Vector3 toPlayerVec = playerPosition - position;
	float length = toPlayerVec.Length();

	// --- �������T�m�͈͓��Ȃ� ---
	if (length < searchRange_)
	{
		// --- ���ʃx�N�g�����擾 ---
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

	// --- ������Ȃ����� ---
	return false;
}


// --- �ڕW�̃x�N�g���։�] ---
void Enemy::RotateToTargetVec(const DirectX::XMFLOAT3& targetVec, float t, const Vector3* tempFront)
{
	Matrix R;
	R.MakeRotationFromQuaternion(quaternion_);
	Vector3 front = R.v_[2].xyz();	// ���ʂ��擾

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


// --- �ʒu�̐��� ---
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



// --- �r�w�C�r�A�c���[�̏����� ---
void Enemy::InitializeBehaviorTree()
{
	behaviorData_ = new BehaviorData<Enemy>();
	aiTree_ = new BehaviorTree<Enemy>();


	// --- �匳�̃m�[�h ---
	aiTree_->AddNode("", "Root", 0, BT_SelectRule::Priority, nullptr, nullptr);


	// --- �ŏ��̔����� ---
	aiTree_->AddNode("Root", "Find", 0, BT_SelectRule::Sequence, new EnemyBattleInitJudgment(this), nullptr);
	{
		aiTree_->AddNode("Find", "AxisAlignment", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
		aiTree_->AddNode("Find", "BigRoar", 0, BT_SelectRule::Non, nullptr, new EnemyBigRoarAction(this));
	}

	// --- ���S ---
	aiTree_->AddNode("Root", "Dead", 1, BT_SelectRule::Non, new EnemyDeadJudgment(this), new EnemyDeadAction(this));

	// --- ���� ---
	aiTree_->AddNode("Root", "Flinch", 2, BT_SelectRule::Non, new EnemyFlinchJudgment(this), new EnemyFlinchAction(this));


	// --- �퓬�m�[�h ---
	aiTree_->AddNode("Root", "Battle", 3, BT_SelectRule::Priority, new EnemyBattleJudgment(this), nullptr);
	{
		// --- �U���m�[�h ---
		aiTree_->AddNode("Battle", "Attack", 0, BT_SelectRule::Random, nullptr, nullptr);
		{
			// --- ������ ---
			aiTree_->AddNode("Attack", "LongRange", 0, BT_SelectRule::Random, /*new EnemyLongRangeJudgment(this)*/nullptr, nullptr);
			{
				// --- �u���X �� �Њd ---
				aiTree_->AddNode("LongRange", "Bless_Threat", 0, BT_SelectRule::Sequence, nullptr, nullptr);
				{
					aiTree_->AddNode("Bless_Threat", "AxisAlignment", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
					aiTree_->AddNode("Bless_Threat", "Bless", 0, BT_SelectRule::Non, nullptr, new EnemyBlessAction(this));
					aiTree_->AddNode("Bless_Threat", "Threat", 0, BT_SelectRule::Non, nullptr, new EnemyThreatAction(this));
				}

				// --- �ːi �� �Њd ---
				aiTree_->AddNode("LongRange", "Rush_Threat", 0, BT_SelectRule::Sequence, nullptr, nullptr);
				{
					aiTree_->AddNode("Rush_Threat", "AxisAlignment", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
					aiTree_->AddNode("Rush_Threat", "Rush", 0, BT_SelectRule::Non, nullptr, new EnemyRushAction(this));
					aiTree_->AddNode("Rush_Threat", "Threat", 0, BT_SelectRule::Non, nullptr, new EnemyThreatAction(this));
				}

				// --- ���ʂɐڋ� ---
				aiTree_->AddNode("LongRange", "Pursuit", 0, BT_SelectRule::Non, nullptr, new EnemyPursuitAction(this));
			}


			// --- ������ ---
			aiTree_->AddNode("Attack", "MiddleRange", 0, BT_SelectRule::Random, new EnemyMiddleRangeJudgment(this), nullptr);
			{
				// --- �����킹 �� ���ݍ��݊��݂� �� ���݂� or �^�b�N�� or �Њd ---
				aiTree_->AddNode("MiddleRange", "Turn_RushingBite_Any", 0, BT_SelectRule::Sequence, nullptr, nullptr);
				{
					aiTree_->AddNode("Turn_RushingBite_Any", "Turn", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
					aiTree_->AddNode("Turn_RushingBite_Any", "RushingBite", 0, BT_SelectRule::Non, nullptr, new EnemyRushingBiteAction(this));
					aiTree_->AddNode("Turn_RushingBite_Any", "AfterAction", 0, BT_SelectRule::Non, nullptr, new EnemyAfterRushingBiteAction(this));
				}
			}


			// --- �ߋ��� ---
			aiTree_->AddNode("Attack", "ShortRange", 0, BT_SelectRule::Random, new EnemyShortRangeJudgment(this), nullptr);
			{
				// --- ���݂� �� �Њd ---
				aiTree_->AddNode("ShortRange", "Stamp_Threat", 0, BT_SelectRule::Sequence, nullptr, nullptr);
				{
					aiTree_->AddNode("Stamp_Threat", "Stamp", 0, BT_SelectRule::Non, nullptr, new EnemyStampAction(this));
					aiTree_->AddNode("Stamp_Threat", "Threat", 0, BT_SelectRule::Non, nullptr, new EnemyThreatAction(this));
				}

				// --- �����킹 �� ���݂� ---
				aiTree_->AddNode("ShortRange", "AxisAlignment_Bite", 0, BT_SelectRule::Sequence, nullptr, nullptr);
				{
					aiTree_->AddNode("AxisAlignment_Bite", "AxisAlignment", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
					aiTree_->AddNode("AxisAlignment_Bite", "Bite", 0, BT_SelectRule::Non, nullptr, new EnemyBiteAction(this));
				}

				// --- �K����] ---
				aiTree_->AddNode("ShortRange", "TailAttack", 0, BT_SelectRule::Non, nullptr, new EnemyTailAttack(this));
			}
		}


		// --- �ǐՃm�[�h ---
		aiTree_->AddNode("Battle", "Pursuit", 1, BT_SelectRule::Non, nullptr, new EnemyPursuitAction(this));
	}


	// --- ���������m�[�h ---
	aiTree_->AddNode("Root", "Scout", 4, BT_SelectRule::Sequence, new EnemyScoutJudgment(this), nullptr);
	{
		// --- �p�j ---
		aiTree_->AddNode("Scout", "Wander", 0, BT_SelectRule::Non, new EnemyWanderJudgment(this), new EnemyWanderAction(this));
		
		// --- �ҋ@ ---
		aiTree_->AddNode("Scout", "Idle", 0, BT_SelectRule::Non, nullptr, new EnemyIdleAction(this));
	}
}

// --- �I������ ---
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
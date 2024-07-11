#include "Enemy.h"

#include "../../External/ImGui/imgui.h"

#include "../../Library/3D/DebugPrimitive.h"
#include "../../Library/3D/CameraManager.h"

#include "../../Library/Timer.h"
#include "../../Library/Math/Math.h"
#include "../../Library/Math/Collision.h"

#include "../../Library/Input/InputManager.h"

#include "../../Effect/Rock/RockEffect.h"
#include "../../Effect/Breath/BreathEffect.h"
#include "../../Library/Particle/EmitterManager.h"

#include "EnemyBehavior.h"

#include "../../../../Camera/CameraDerived.h"

#include "../Player/Player.h"


Enemy::~Enemy()
{
}


const Vector3 Enemy::GetFrontVec()
{
	Matrix R;
	R.MakeRotationFromQuaternion(quaternion_);
	return R.v_[2].xyz();
}


void Enemy::Initialize()
{
	// --- �r�w�C�r�A�c���[�̏�����   �q�m�[�h�̒ǉ���A�N�V�����m�[�h�̐ݒ�Ȃ� ---
	InitializeBehaviorTree();

	position = { 0, 0, 0 };
	velocity = { 0.0f, 0.0f, 0.0f };
	moveVec = { 0.0f, 0.0f, 0.0f };

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
	float elapsedTime = Timer::Instance().DeltaTime();

	// --- �r�w�C�r�A�c���[�̍X�V ---
	UpdateBehaviorTree(elapsedTime);

	// --- �ړ����� ---
	UpdateMove(elapsedTime);

	// �A�j���[�V�����X�V
	UpdateAnimation();

	// --- �ʒu�̐��� ---
	ClampPosition(75.0f);

	if (alive && hp < 0.0f)
	{
		OnDead();
	}


	// --- �p���s��쐬 ---
	Transform();
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

	if (ImGui::TreeNode(u8"�ړ��֘A"))
	{
		ImGui::DragFloat3(u8"���x", &velocity.x);
		ImGui::DragFloat3(u8"�ړ��x�N�g��", &moveVec.x);
		ImGui::DragFloat(u8"�����x", &acceleration);
		ImGui::DragFloat(u8"���C", &friction);
		ImGui::DragFloat(u8"�ő呬�x", &maxSpeed);
		ImGui::TreePop();
	}

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

	Matrix R;
	R.MakeRotationFromQuaternion(quaternion_);
	Vector3 front = R.v_[2].xyz();
	float atan = atan2(front.z, front.x);
	float theta = DirectX::XMConvertToDegrees(atan);
	ImGui::Text(u8"�p�x :%f", theta);

	Vector3 playerPosition = Player::Instance().GetPos();
	Vector3 vec = playerPosition - position;
	vec.Normalize();

	float cross = (front.z * vec.x) - (front.x * vec.z);
	ImGui::Text(u8"���E���� : %f", cross);

	float dot = front.Dot(vec);
	ImGui::Text(u8"���� : %f", dot);


	ImGui::End();
}


void Enemy::DrawDebug()
{
	DebugPrimitive::Instance().AddCylinder(position, searchRange_, 1.0f, { 1.0f, 0.0f, 1.0f, 1.0f });
	DebugPrimitive::Instance().AddCylinder({}, wanderRange, 5.0f, { 0.3f, 0.3f, 1.0f, 1.0f });
	DebugPrimitive::Instance().AddCylinder(position, shortRange, 2.0f, { 1.0f, 0.0f, 0.0f, 1.0f });
	DebugPrimitive::Instance().AddCylinder(position, middleRange, 2.0f, { 0.0f, 1.0f, 0.0f, 1.0f });
	DebugPrimitive::Instance().AddCylinder(position, longRange, 5.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
	DebugPrimitive::Instance().AddSphere(moveTargetPosition_.vec_, 1.0f, { 0.3f, 0.3f, 1.0f, 1.0f });
}


void Enemy::Transform()
{
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


void Enemy::CollisionVSPlayer()
{
	// --- �G�̍U���̋� ---
	for (auto& enemySphere : model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex).animSphereCollisions)
	{
		// --- �A�j���[�V�������͈͊O�������玟�� ---
		if (currentKeyFrame < enemySphere.startFrame || currentKeyFrame > enemySphere.endFrame) continue;

		if (enemySphere.isDamaged)
			break;


		// --- �v���C���[�̋� ---
		for (auto& playerSphere : Player::Instance().GetModel()->GetModelResource()->GetSkeletonSphereCollisions())
		{
			// --- �G�̋��̍��W���擾 ---
			Vector3 enemyBonePosition = enemySphere.position;
			enemyBonePosition = (enemySphere.bindBoneName == "") ? enemyBonePosition + position : GetBonePosition(enemySphere.bindBoneName);

			// --- �v���C���[�̋��̍��W���擾 ---
			Vector3 playerBonePosition = Player::Instance().GetPos();
			playerBonePosition = (playerSphere.name == "") ? playerBonePosition + playerSphere.position : Player::Instance().GetBonePosition(playerSphere.name);


			Vector3 dummy;
			if (Collision::IntersectSphereVsSphere(enemyBonePosition.vec_, enemySphere.radius, playerBonePosition.vec_, playerSphere.radius, dummy.vec_))
			{
				EnableAnimSphereCollisionDamagedFlag();

				// --- �����ɓ����������̏��������� ---
				auto camera = std::dynamic_pointer_cast<PlayerCamera>(CameraManager::Instance().GetCamera());
				if (camera)
					camera->SetShakeTimer(1.0f);
				break;
			}
		}
	}
}


// --- �ړ����� ---
void Enemy::UpdateMove(float elapsedTime)
{
	float elapsedFrame = (60.0f / (1.0f / elapsedTime));

	// --- �������͏��� ---
	UpdateHorizontalVelocity(elapsedTime, elapsedFrame);

	// --- �����ړ����� ---
	UpdateHorizontalMove(elapsedTime, elapsedFrame);
}


// --- �������͏��� ---
void Enemy::UpdateHorizontalVelocity(float elapsedTime, float elapsedFrame)
{
	Vector2 velocity = this->velocity.xz();
	float length = velocity.Length();

	// --- ���x������Ό��� ---
	if (length > 0.0f)
	{
		// --- ��R�� ---
		float friction = this->friction * elapsedFrame;
		if (length > friction)
		{
			velocity.Normalize();

			// --- �������� ---
			this->velocity.x -= velocity.x * friction;
			this->velocity.z -= velocity.y * friction;
		}

		else
		{
			this->velocity.x = 0.0f;
			this->velocity.z = 0.0f;
		}
	}


	// --- �ő呬�x���z���Ă��Ȃ��Ȃ���� ---
	if (length <= maxSpeed)
	{
		Vector2 moveVec = this->moveVec.xz();
		float moveVecLength = moveVec.Length();
		if (moveVecLength > 0.0f)
		{
			// --- �����x ---
			float acceleration = this->acceleration * elapsedFrame;

			// --- �ړ��x�N�g�������ɉ��� ---
			this->velocity.x += moveVec.x * acceleration;
			this->velocity.z += moveVec.y * acceleration;
		}


		// --- �ő呬�x���� ---
		Vector2 updatedVelocity = this->velocity.xz();
		float updatedLength = updatedVelocity.Length();

		// --- �ő呬�x���z���Ă����琧�� ---
		if (updatedLength > maxSpeed)
		{
			this->velocity.x = (this->velocity.x / updatedLength) * maxSpeed;
			this->velocity.z = (this->velocity.z / updatedLength) * maxSpeed;
		}
	}


	moveVec.x = moveVec.z = 0.0f;
}


// --- �����ړ����� ---
void Enemy::UpdateHorizontalMove(float elapsedTime, float elapsedFrame)
{
	Vector2 vec = velocity.xz();
	float length = vec.Length();

	// --- �ړ��ʂ������ ---
	if (length > 0.0f)
	{
		Vector3 updatedPosition;
		Vector3 position = this->position;
		updatedPosition = position + velocity * elapsedTime;

		this->position = updatedPosition.vec_;
	}
}


// --- �ړ� ---
void Enemy::Move(const Vector3& vec, const float speed)
{
	moveVec = vec;
	maxSpeed = speed;
}


// --- �v���C���[���������� ---
bool Enemy::SearchPlayer()
{
	// --- �v���C���[�Ƃ̋����̌v�Z ---
	Vector3 playerPosition = Player::Instance().GetPos();
	Vector3 toPlayerVec = playerPosition - position;
	float length = toPlayerVec.Length();

	// --- �������痣��Ă����� ---
	float playerLength = playerPosition.Length();
	if (playerLength > 80.0f)
		return false;

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

		else
		{
			Quaternion dst = rot * quaternion_;
			quaternion_ = quaternion_.Lerp(quaternion_, dst, 1.0f);
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


// --- ��G�t�F�N�g ---
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


// --- ���u���X ---
void Enemy::PlayFireBress(float angle)
{
	BreathEffect::Instance().SetPosition(Enemy::Instance().GetBonePosition("sitaago"));
	BreathEffect::Instance().SetAngle(angle);
	BreathEffect::Instance().Emit();
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
				//// --- �u���X �� �Њd ---
				//aiTree_->AddNode("LongRange", "Bless_Threat", 0, BT_SelectRule::Sequence, nullptr, nullptr);
				//{
				//	aiTree_->AddNode("Bless_Threat", "AxisAlignment", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
				//	aiTree_->AddNode("Bless_Threat", "Bless", 0, BT_SelectRule::Non, nullptr, new EnemyBlessAction(this));
				//	aiTree_->AddNode("Bless_Threat", "Threat", 0, BT_SelectRule::Non, nullptr, new EnemyThreatAction(this));
				//}

				//// --- �ːi �� �Њd ---
				//aiTree_->AddNode("LongRange", "Rush_Threat", 0, BT_SelectRule::Sequence, nullptr, nullptr);
				//{
				//	aiTree_->AddNode("Rush_Threat", "AxisAlignment", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
				//	aiTree_->AddNode("Rush_Threat", "Rush", 0, BT_SelectRule::Non, nullptr, new EnemyRushAction(this));
				//	aiTree_->AddNode("Rush_Threat", "Threat", 0, BT_SelectRule::Non, nullptr, new EnemyThreatAction(this));
				//}

				// --- ���ʂɐڋ� ---
				aiTree_->AddNode("LongRange", "Pursuit", 0, BT_SelectRule::Non, nullptr, new EnemyPursuitAction(this));
			}


			//// --- ������ ---
			//aiTree_->AddNode("Attack", "MiddleRange", 0, BT_SelectRule::Random, new EnemyMiddleRangeJudgment(this), nullptr);
			//{
			//	// --- �����킹 �� ���ݍ��݊��݂� �� ���݂� or �^�b�N�� or �Њd ---
			//	aiTree_->AddNode("MiddleRange", "Turn_RushingBite_Any", 0, BT_SelectRule::Sequence, nullptr, nullptr);
			//	{
			//		aiTree_->AddNode("Turn_RushingBite_Any", "Turn", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
			//		aiTree_->AddNode("Turn_RushingBite_Any", "RushingBite", 0, BT_SelectRule::Non, nullptr, new EnemyRushingBiteAction(this));
			//		aiTree_->AddNode("Turn_RushingBite_Any", "AfterAction", 0, BT_SelectRule::Non, nullptr, new EnemyAfterRushingBiteAction(this));
			//	}

			//	// --- �����킹 �� �d���グ ---
			//	aiTree_->AddNode("MiddleRange", "Turn_ScoopUp", 0, BT_SelectRule::Sequence, nullptr, nullptr);
			//	{
			//		aiTree_->AddNode("Turn_ScoopUp", "Turn", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
			//		aiTree_->AddNode("Turn_ScoopUp", "ScoopUp", 0, BT_SelectRule::Non, nullptr, new EnemyScoopUpAction(this));
			//	}
			//}


			// --- �ߋ��� ---
			aiTree_->AddNode("Attack", "ShortRange", 0, BT_SelectRule::Random, new EnemyShortRangeJudgment(this), nullptr);
			{
				//// --- ���݂� �� �Њd ---
				//aiTree_->AddNode("ShortRange", "Stamp_Threat", 0, BT_SelectRule::Sequence, nullptr, nullptr);
				//{
				//	aiTree_->AddNode("Stamp_Threat", "Stamp", 0, BT_SelectRule::Non, nullptr, new EnemyStampAction(this));
				//	aiTree_->AddNode("Stamp_Threat", "Threat", 0, BT_SelectRule::Non, nullptr, new EnemyThreatAction(this));
				//}

				// --- �����킹 �� ���݂� ---
				aiTree_->AddNode("ShortRange", "AxisAlignment_Bite", 0, BT_SelectRule::Sequence, nullptr, nullptr);
				{
					aiTree_->AddNode("AxisAlignment_Bite", "AxisAlignment", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
					aiTree_->AddNode("AxisAlignment_Bite", "Bite", 0, BT_SelectRule::Non, nullptr, new EnemyBiteAction(this));
				}

				//// --- �K����] ---
				//aiTree_->AddNode("ShortRange", "TailAttack", 0, BT_SelectRule::Non, nullptr, new EnemyTailAttack(this));

				// --- ���݂� ---
				//aiTree_->AddNode("ShortRange", "Bite", 0, BT_SelectRule::Non, new EnemyFrontJudgment(this), new EnemyBiteAction(this));
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
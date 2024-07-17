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

#include "EnemyAwakeBehavior.h"

#include "../../../../Camera/CameraDerived.h"

#include "../Player/Player.h"

#include "../../../../UserInterface/UiClearAfter.h"


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
	maxHP = 1000.0f;
	hp = maxHP;
	flinchValue = 100.0f;

	alive = true;
	awake = false;
	endRushingBite = false;
	wasAttacked = false;

	actionCount = 0;
	roarNeededActionCount = 25;
	hissatuCount = 0;

	// Animator
	if (animator.GetCurrentState()) return;
	enum class Motion : int {
		ASIDON,
		BURESU,
		DOWN,
		DOWN_TYU,
		HISSATU_1,
		HISSATU_2,
		HISSATU_3,
		HOERU,
		HOERU_BIG,
		IDLE,
		KAITEN,
		KAMITUKI_1,
		KAMITUKI_2,
		OKIRU,
		SINU,
		SUKUIAGE,
		TAKKURU,
		TAKKURU_RIGHT,
		TOKOTOKO_LEFT,
		TOKOTOKO_RIGHT,
		TOSSIN,
		WALK_LEFT,
		WALK_MAE,
		WALK_RIGHT,
		WALK_USIRO,
		SIZE
	};

	constexpr auto IntCast = [](auto v) ->int {return static_cast<int>(v); };

	std::vector<ModelResource::Animation>& animations = GetModel()->GetModelResource()->GetAnimationClips();

	std::vector<Animator::Motion> motions;
	int i = 0;
	for (auto&& animation: animations) {
		Animator::Motion motion;
		motion.motion = &animation;
		motion.animationIndex = i;
		motion.loop = false;
		motions.emplace_back(motion);

		i++;
	}

	motions[IntCast(Motion::IDLE)].loop = true;
	motions[IntCast(Motion::WALK_MAE)].loop = true;
	motions[IntCast(Motion::WALK_USIRO)].loop = true;
	motions[IntCast(Motion::WALK_LEFT)].loop = true;
	motions[IntCast(Motion::WALK_RIGHT)].loop = true;
	motions[IntCast(Motion::DOWN_TYU)].loop = true;
	motions[IntCast(Motion::HISSATU_2)].loop = true;
	motions[IntCast(Motion::TOKOTOKO_LEFT)].loop = true;
	motions[IntCast(Motion::TOKOTOKO_RIGHT)].loop = true;
	motions[IntCast(Motion::HISSATU_2)].loop = true;

	motions[IntCast(Motion::IDLE)].threshold = {0,0};
	motions[IntCast(Motion::WALK_MAE)].threshold = {0,1};
	motions[IntCast(Motion::WALK_USIRO)].threshold = {0,-1};
	motions[IntCast(Motion::WALK_LEFT)].threshold = {-1,0};
	motions[IntCast(Motion::WALK_RIGHT)].threshold = {1,0};

	Animator::BlendTree walkTree;
	walkTree.motions.emplace_back(motions[IntCast(Motion::IDLE)]);
	walkTree.motions.emplace_back(motions[IntCast(Motion::WALK_MAE)]);
	walkTree.motions.emplace_back(motions[IntCast(Motion::WALK_USIRO)]);
	walkTree.motions.emplace_back(motions[IntCast(Motion::WALK_LEFT)]);
	walkTree.motions.emplace_back(motions[IntCast(Motion::WALK_RIGHT)]);
	walkTree.parameters[0] = "moveX";
	walkTree.parameters[1] = "moveY";
	walkTree.maxSeconds = 1.833333f;

	Animator::State walkState;
	walkState.object = Animator::MakeObjPointer(walkTree);
	walkState.type = Animator::State::BLEND_TREE;
	//walkState.transitions;

	std::vector<Animator::State> states;
	states.resize(IntCast(Motion::SIZE));
	i = 0;
	for (auto&& state: states) {
		state.object = Animator::MakeObjPointer(motions[i]);
		state.type = Animator::State::MOTION;
		state.transitionTime = 0.4f;
		i++;
	}

	animator.AddState("asidon", states[IntCast(Motion::ASIDON)]);
	animator.AddState("buresu", states[IntCast(Motion::BURESU)]);
	animator.AddState("down", states[IntCast(Motion::DOWN)]);
	animator.AddState("down_tyu", states[IntCast(Motion::DOWN_TYU)]);
	animator.AddState("hissatu_1", states[IntCast(Motion::HISSATU_1)]);
	animator.AddState("hissatu_2", states[IntCast(Motion::HISSATU_2)]);
	animator.AddState("hissatu_3", states[IntCast(Motion::HISSATU_3)]);
	animator.AddState("hoeru", states[IntCast(Motion::HOERU)]);
	animator.AddState("hoeru_big", states[IntCast(Motion::HOERU_BIG)]);
	animator.AddState("idle", states[IntCast(Motion::IDLE)]);
	animator.AddState("kaiten", states[IntCast(Motion::KAITEN)]);
	animator.AddState("kamituki_1", states[IntCast(Motion::KAMITUKI_1)]);
	animator.AddState("kamituki_2", states[IntCast(Motion::KAMITUKI_2)]);
	animator.AddState("okiru", states[IntCast(Motion::OKIRU)]);
	animator.AddState("sinu", states[IntCast(Motion::SINU)]);
	animator.AddState("sukuiage", states[IntCast(Motion::SUKUIAGE)]);
	animator.AddState("takkuru_left", states[IntCast(Motion::TAKKURU)]);
	animator.AddState("takkuru_right", states[IntCast(Motion::TAKKURU_RIGHT)]);
	animator.AddState("tokotoko_left", states[IntCast(Motion::TOKOTOKO_LEFT)]);
	animator.AddState("tokotoko_right", states[IntCast(Motion::TOKOTOKO_RIGHT)]);
	animator.AddState("tossin", states[IntCast(Motion::TOSSIN)]);
	animator.AddState("walk_left", states[IntCast(Motion::WALK_LEFT)]);
	animator.AddState("walk_mae", states[IntCast(Motion::WALK_MAE)]);
	animator.AddState("walk_right", states[IntCast(Motion::WALK_RIGHT)]);
	animator.AddState("walk_usiro", states[IntCast(Motion::WALK_USIRO)]);
	//animator.AddState("walk", walkState);
	animator.SetEntryState("idle");
	animator.SetModelSceneView(&GetModel()->GetModelResource()->GetSceneView());
	animator.EnableRootMotion("joint1");

	//PlayAnimation(static_cast<int>(MonsterAnimation::WALK_FOWARD), true);
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
	//UpdateAnimation();

	animatorKeyFrame = animator.PlayAnimation(elapsedTime);
	currentKeyFrame = animator.GetKeyFrameIndex();
	currentAnimationIndex = animator.GetMotionIndex();

	// --- �ʒu�̐��� ---
	ClampPosition(75.0f);

	// �����Ă�Ƃ���HP���O�����������
	if (alive && hp < 0.0f)
	{
		OnDead();
	}


	// --- �p���s��쐬 ---
	Transform();
}


void Enemy::Render(bool isShadow)
{
	model->Render(transform, &animatorKeyFrame, isShadow);
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

	CameraManager::Instance().clearTimer = 40.0f;
	//CameraManager::Instance().clear = true;
	UiClearAfter::Instance().OnClear();

	Player::Instance().SetCamera(CameraManager::Instance().GetCamera("PlayerCamera").get());

	alive = false;
}

// --- �U�����ꂽ�Ƃ� ---
void Enemy::OnAttacked(const float attackPower)
{
	hp -= attackPower;

	//if (hp < 0.0f)
	//{
	//	hp = 0.0f;
	//}
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

	ImGui::DragFloat(u8"�^�C�}�[", &runTimer_);
	if (ImGui::TreeNode(u8"�ړ��֘A"))
	{
		ImGui::DragFloat3(u8"���x", &velocity.x);
		ImGui::DragFloat3(u8"�ړ��x�N�g��", &moveVec.x);
		ImGui::DragFloat(u8"�����x", &acceleration);
		ImGui::DragFloat(u8"���C", &friction);
		ImGui::DragFloat(u8"�ő呬�x", &maxSpeed);
		ImGui::Checkbox(u8"����", &foundPlayer);
		ImGui::DragFloat(u8"���G����", &searchRange_);
		ImGui::DragFloat(u8"�������x", &walkSpeed_, 0.01f);
		ImGui::DragFloat(u8"���鑬�x", &runSpeed_, 0.01f);
		ImGui::DragFloat3(u8"�p�j�ڕW", &moveTargetPosition_.x);
		ImGui::DragFloat(u8"�ːi�X�s�[�h", &rushSpeed, 0.01f);
		ImGui::DragFloat(u8"�ːi�I���X�s�[�h", &rushEndSpeed, 0.01f);
		ImGui::DragFloat(u8"�ːi���ߎ���", &rushChargeTimer, 0.01f);
		ImGui::DragFloat(u8"�ːi���̎���", &whileRushTimer, 0.01f);
		ImGui::TreePop();
	}

	ImGui::Checkbox(u8"�U�����ꂽ��", &wasAttacked);
	ImGui::Text(u8"�s���� : %d", actionCount);
	ImGui::Text(u8"���K�ɕK�v�ȍs���� : %d", roarNeededActionCount);
	ImGui::Text(u8"�U���� : %d", attackCount);
	ImGui::DragFloat(u8"HP", &hp, 1.0f);
	ImGui::DragFloat(u8"���ݒl", &flinchValue, 1.0f);
	Vector3 playerPosition = Player::Instance().GetPos();
	float playerLength = playerPosition.Length();
	ImGui::Text(u8"�v���C���[�̐^�񒆂���̋��� : %f", playerLength);
	ImGui::Separator();

	ImGui::DragFloat3(u8"�X�P�[��", &scale.x);


	ImGui::Separator();
	ImGui::DragFloat(u8"�������͈�", &longRange, 0.01f);

	Matrix R;
	R.MakeRotationFromQuaternion(quaternion_);
	Vector3 front = R.v_[2].xyz();
	float atan = atan2(front.z, front.x);
	float theta = DirectX::XMConvertToDegrees(atan);
	ImGui::Text(u8"�p�x :%f", theta);

	Vector3 vec = playerPosition - position;
	vec.Normalize();

	float cross = (front.z * vec.x) - (front.x * vec.z);
	ImGui::Text(u8"���E���� : %f", cross);

	float dot = front.Dot(vec);
	ImGui::Text(u8"���� : %f", dot);

	if (ImGui::Button(u8"���S�J�����؂�ւ�", { 200.0f, 30.0f }))
	{
		CameraManager::Instance().SetCurrentCamera("EnemyDeadCamera");
	}

	if (ImGui::Button(u8"�v���C���[���S�J�����؂�ւ�", { 200.0f, 30.0f }))
	{
		CameraManager::Instance().SetCurrentCamera("PlayerDeadCamera");
	}


	ImGui::End();

	animator.AnimationEditor();
}


void Enemy::DrawDebug()
{
//	DebugPrimitive::Instance().AddCylinder(position, 3.0f, 1.0f, { 1.0f, 0.0f, 0.0f, 1.0f });
	DebugPrimitive::Instance().AddCylinder(position, searchRange_, 1.0f, { 1.0f, 0.0f, 1.0f, 1.0f });
//	DebugPrimitive::Instance().AddCylinder({}, wanderRange, 5.0f, { 0.3f, 0.3f, 1.0f, 1.0f });
//	DebugPrimitive::Instance().AddCylinder(position, shortRange, 2.0f, { 1.0f, 0.0f, 0.0f, 1.0f });
//	DebugPrimitive::Instance().AddCylinder(position, middleRange, 2.0f, { 0.0f, 1.0f, 0.0f, 1.0f });
//	DebugPrimitive::Instance().AddCylinder(position, longRange, 5.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
//	DebugPrimitive::Instance().AddSphere(moveTargetPosition_.vec_, 1.0f, { 0.3f, 0.3f, 1.0f, 1.0f });
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
	S.MakeScaling(Vector3(GetScale()));
	// ��]�s����쐬
	Matrix R;
	R.MakeRotationFromQuaternion(quaternion_);
	// �ʒu�s����쐬
	Matrix T;
	T.MakeTranslation(Vector3(GetPos()));

	// �S�̍s���g�ݍ��킹�A���[���h�s����쐬
	Matrix W =C * MS * S * R * T;
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
				CameraManager::Instance().shakeTimer = 1.0f;
				CameraManager::Instance().shakePower = 100.0f;
				attackCount++;

				// �q�b�g�X�g�b�v����
				Player::Instance().hitStopTimer = 0.05f;
				Timer::Instance().SetTimeScale(0.0f);

				// TODO::Player�_���[�W�֌W
				Player& player = Player::Instance();
				//float currentHP = player.AStatus().hp;
				bool frying = false;

				Vector3 pPos = player.GetPos();
				Vector3 ePos = position;
				Vector3 fryVec = pPos - ePos;
				fryVec.y = 0;
				fryVec.Normalize();
				fryVec *= 10;
				const Animator::State* current = animator.GetCurrentState();

				if (current == &animator.GetState("asidon")) frying = true;

				player.HitDamaged(attackPower,false,frying,fryVec);

				// --- ���̍U���Ńv���C���[�����S�����Ƃ� ---
				// if (player.AStatus().hp <= 0.0f && currentHP > 0.0f)
				// {
				// 	CameraManager::Instance().SetCurrentCamera("PlayerDeadCamera");
				// }

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
	if (playerLength > 81.0f)
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

// --- �v���C���[�����񂾂Ƃ��ɌĂ� ---
void Enemy::OnPlayerDead()
{
	foundPlayer = false;
	wasAttacked = false;
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



enum
{
	FIND,
	DEAD,
	FLINCH,
	AWAKE,
	END_RUSHING_BITE,
	BIG_ROAR,
	BATTLE,
	SCOUT,
};



// --- �r�w�C�r�A�c���[�̏����� ---
void Enemy::InitializeBehaviorTree()
{
	behaviorData_ = new BehaviorData<Enemy>();
	aiTree_ = new BehaviorTree<Enemy>();


	// --- �匳�̃m�[�h ---
	aiTree_->AddNode("", "Root", 0, BT_SelectRule::Priority, nullptr, nullptr);


	// --- �ŏ��̔����� ---
	aiTree_->AddNode("Root", "Find", FIND, BT_SelectRule::Sequence, new EnemyBattleInitJudgment(this), nullptr);
	{
		aiTree_->AddNode("Find", "AxisAlignment", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
		aiTree_->AddNode("Find", "BigRoar", 0, BT_SelectRule::Non, nullptr, new EnemyBigRoarAction(this));
	}

	// --- ���S ---
	aiTree_->AddNode("Root", "Dead", DEAD, BT_SelectRule::Non, new EnemyDeadJudgment(this), new EnemyDeadAction(this));

	// --- ���� ---
	aiTree_->AddNode("Root", "Flinch", FLINCH, BT_SelectRule::Non, new EnemyFlinchJudgment(this), new EnemyFlinchAction(this));


	// --- �o���ڍs ---
	aiTree_->AddNode("Root", "Awake", AWAKE, BT_SelectRule::Sequence, new EnemyAwakeJudgment(this), nullptr);
	{
		aiTree_->AddNode("Awake", "BigRoar", 0, BT_SelectRule::Non, nullptr, new EnemyBigRoarAction(this));
		aiTree_->AddNode("Awake", "Move", 0, BT_SelectRule::Non, nullptr, new EnemyMoveCenterAction(this));
		aiTree_->AddNode("Awake", "Turn", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
		aiTree_->AddNode("Awake", "Awake", 0, BT_SelectRule::Non, nullptr, new EnemyDeathBlowAction(this));
	}


	// --- ���ݍ��݊��݂��I���s�� ---
	aiTree_->AddNode("Root", "EndRushingBite", END_RUSHING_BITE, BT_SelectRule::Priority, new EnemyEndRushingBiteJudgment(this), nullptr);
	{
		// --- �����ɂ����瓥�݂� ---
		aiTree_->AddNode("EndRushingBite", "Stamp", 0, BT_SelectRule::Non, new EnemyFootJudgment(this), new EnemyStampAction(this));

		// --- ���ɂ�����^�b�N�� ---
		aiTree_->AddNode("EndRushingBite", "Tackle", 1, BT_SelectRule::Non, new EnemyRightJudgment(this), new EnemyTackleAction(this));

		// --- ���ɂ����� ---
		aiTree_->AddNode("EndRushingBite", "TailAttack", 2, BT_SelectRule::Non, new EnemyBehindJudgment(this), new EnemyTailAttack(this));
	}

	// --- ����K ---
	aiTree_->AddNode("Root", "BigRoar", BIG_ROAR, BT_SelectRule::Non, new EnemyBigRoarJudgment(this), new EnemyBigRoarAction(this));

	// --- �퓬�m�[�h ---
	aiTree_->AddNode("Root", "Battle", BATTLE, BT_SelectRule::Priority, new EnemyBattleJudgment(this), nullptr);
	{
		// --- �U���m�[�h ---
		aiTree_->AddNode("Battle", "Attack", 0, BT_SelectRule::Priority, nullptr, nullptr);
		{
			// --- �����U�� ---
			aiTree_->AddNode("Attack", "FootAttack", 0, BT_SelectRule::Random, new EnemyFootJudgment(this), nullptr);
			{
				// --- ���݂� ---
				aiTree_->AddNode("FootAttack", "Stamp", 0, BT_SelectRule::Non, nullptr, new EnemyStampAction(this));

				// --- �K����] ---
				aiTree_->AddNode("FootAttack", "TailAttack", 0, BT_SelectRule::Non, nullptr, new EnemyTailAttack(this));
			}

			// --- ����ȊO ---
			aiTree_->AddNode("Attack", "NormalAttack", 1, BT_SelectRule::Random, nullptr, nullptr);
			{
				// --- ������ ---
				aiTree_->AddNode("NormalAttack", "LongRange", 0, BT_SelectRule::Random, new EnemyLongRangeJudgment(this)/*nullptr*/, nullptr);
				{
					// --- �u���X �� �Њd ---
					aiTree_->AddNode("LongRange", "L_Bless_Threat", 0, BT_SelectRule::Sequence, new EnemyAwakedJudgment(this)/*nullptr*/, nullptr);
					{
						aiTree_->AddNode("L_Bless_Threat", "L_AxisAlignment", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
						aiTree_->AddNode("L_Bless_Threat", "L_Bless", 0, BT_SelectRule::Non, nullptr, new EnemyBlessAction(this));
						aiTree_->AddNode("L_Bless_Threat", "L_Threat", 0, BT_SelectRule::Non, nullptr, new EnemyThreatAction(this));
					}

					// --- �����K ---
					aiTree_->AddNode("LongRange", "L_Turn_LightningRoar", 0, BT_SelectRule::Sequence, new EnemyAwakedJudgment(this), nullptr);
					{
						aiTree_->AddNode("L_Turn_LightningRoar", "L_Turn", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
						aiTree_->AddNode("L_Turn_LightningRoar", "L_LightningRoar", 0, BT_SelectRule::Non, nullptr, new EnemyLightningRoarAction(this));
					}

					// --- ������K ---
					aiTree_->AddNode("LongRange", "L_Turn_LightningBigRoar", 0, BT_SelectRule::Sequence, new EnemyAwakedJudgment(this), nullptr);
					{
						aiTree_->AddNode("L_Turn_LightningRoar", "L_Turn", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
						aiTree_->AddNode("L_Turn_LightningRoar", "L_LightningBigRoar", 0, BT_SelectRule::Non, nullptr, new EnemyLightningBigRoarAction(this));
					}


					// --- �����킹 �� �d���グ ---
					aiTree_->AddNode("LongRange", "L_Turn_ScoopUp", 0, BT_SelectRule::Sequence, nullptr, nullptr);
					{
						aiTree_->AddNode("L_Turn_ScoopUp", "L_Turn", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
						aiTree_->AddNode("L_Turn_ScoopUp", "L_ScoopUp", 0, BT_SelectRule::Non, nullptr, new EnemyScoopUpAction(this));
					}


					// --- �ːi �� �Њd ---
					aiTree_->AddNode("LongRange", "L_Rush_Threat", 0, BT_SelectRule::Sequence, nullptr, nullptr);
					{
						aiTree_->AddNode("L_Rush_Threat", "L_AxisAlignment", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
						aiTree_->AddNode("L_Rush_Threat", "L_Rush", 0, BT_SelectRule::Non, nullptr, new EnemyRushAction(this));
						aiTree_->AddNode("L_Rush_Threat", "L_Threat", 0, BT_SelectRule::Non, nullptr, new EnemyThreatAction(this));
					}

					// --- ���ʂɐڋ� ---
					aiTree_->AddNode("LongRange", "L_Turn_Pursuit", 0, BT_SelectRule::Sequence, nullptr, nullptr);
					{
						aiTree_->AddNode("L_Turn_Pursuit", "L_Turn", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
						aiTree_->AddNode("L_Turn_Pursuit", "L_Pursuit", 0, BT_SelectRule::Non, nullptr, new EnemyPursuitAction(this));
					}
				}


				// --- ������ ---
				aiTree_->AddNode("NormalAttack", "MiddleRange", 0, BT_SelectRule::Random, new EnemyMiddleRangeJudgment(this), nullptr);
				{
					// --- �����킹 �� ���ݍ��݊��݂� ---
					aiTree_->AddNode("MiddleRange", "M_Turn_RushingBite", 0, BT_SelectRule::Sequence, nullptr, nullptr);
					{
						aiTree_->AddNode("M_Turn_RushingBite", "M_Turn", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
						aiTree_->AddNode("M_Turn_RushingBite", "M_RushingBite", 0, BT_SelectRule::Non, nullptr, new EnemyRushingBiteAction(this));
					}

					// --- �����킹 �� �d���グ ---
					aiTree_->AddNode("MiddleRange", "M_Turn_ScoopUp", 0, BT_SelectRule::Sequence, nullptr, nullptr);
					{
						aiTree_->AddNode("M_Turn_ScoopUp", "M_Turn", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
						aiTree_->AddNode("M_Turn_ScoopUp", "M_ScoopUp", 0, BT_SelectRule::Non, nullptr, new EnemyScoopUpAction(this));
					}

					// --- �ːi �� �Њd ---
					aiTree_->AddNode("MiddleRange", "M_Rush_Threat", 0, BT_SelectRule::Sequence, nullptr, nullptr);
					{
						aiTree_->AddNode("M_Rush_Threat", "M_AxisAlignment", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
						aiTree_->AddNode("M_Rush_Threat", "M_Rush", 0, BT_SelectRule::Non, nullptr, new EnemyRushAction(this));
						aiTree_->AddNode("M_Rush_Threat", "M_Threat", 0, BT_SelectRule::Non, nullptr, new EnemyThreatAction(this));
					}

					// --- �^�b�N�� ---
					aiTree_->AddNode("MiddleRange", "M_Tackle", 0, BT_SelectRule::Non, new EnemyRightJudgment(this), new EnemyTackleAction(this));

					// --- �K����] ---
					aiTree_->AddNode("MiddleRange", "M_TailAttack", 0, BT_SelectRule::Non, nullptr, new EnemyTailAttack(this));
				

					// --- �����K ---
					aiTree_->AddNode("MiddleRange", "M_Turn_LightningRoar", 0, BT_SelectRule::Sequence, new EnemyAwakedJudgment(this), nullptr);
					{
						aiTree_->AddNode("M_Turn_LightningRoar", "M_Turn", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
						aiTree_->AddNode("M_Turn_LightningRoar", "M_LightningRoar", 0, BT_SelectRule::Non, nullptr, new EnemyLightningRoarAction(this));
					}

					// --- ������K ---
					aiTree_->AddNode("LongRange", "M_Turn_LightningBigRoar", 0, BT_SelectRule::Sequence, new EnemyAwakedJudgment(this), nullptr);
					{
						aiTree_->AddNode("M_Turn_LightningRoar", "M_Turn", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
						aiTree_->AddNode("M_Turn_LightningRoar", "M_LightningBigRoar", 0, BT_SelectRule::Non, nullptr, new EnemyLightningBigRoarAction(this));
					}
				}


				// --- �ߋ��� ---
				aiTree_->AddNode("NormalAttack", "ShortRange", 0, BT_SelectRule::Random, new EnemyShortRangeJudgment(this), nullptr);
				{
					// --- ���݂� �� �Њd ---
					aiTree_->AddNode("ShortRange", "S_Stamp_Threat", 0, BT_SelectRule::Sequence, nullptr, nullptr);
					{
						aiTree_->AddNode("S_Stamp_Threat", "S_Stamp", 0, BT_SelectRule::Non, nullptr, new EnemyStampAction(this));
						aiTree_->AddNode("S_Stamp_Threat", "S_Threat", 0, BT_SelectRule::Non, nullptr, new EnemyThreatAction(this));
					}

					// --- �����킹 �� ���݂� ---
					aiTree_->AddNode("ShortRange", "S_AxisAlignment_Bite", 0, BT_SelectRule::Sequence, nullptr, nullptr);
					{
						aiTree_->AddNode("S_AxisAlignment_Bite", "S_AxisAlignment", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
						aiTree_->AddNode("S_AxisAlignment_Bite", "S_Bite", 0, BT_SelectRule::Non, nullptr, new EnemyBiteAction(this));
					}

					// --- �K����] ---
					aiTree_->AddNode("ShortRange", "S_TailAttack", 0, BT_SelectRule::Non, nullptr, new EnemyTailAttack(this));

					// --- �^�b�N�� ---
					aiTree_->AddNode("ShortRange", "S_Tackle", 0, BT_SelectRule::Non, new EnemyRightJudgment(this), new EnemyTackleAction(this));
				}
			}
		}


		// --- �ǐՃm�[�h ---
		aiTree_->AddNode("Battle", "Pursuit", 1, BT_SelectRule::Non, nullptr, new EnemyPursuitAction(this));
	}


	// --- ���������m�[�h ---
	aiTree_->AddNode("Root", "Scout", SCOUT, BT_SelectRule::Sequence, new EnemyScoutJudgment(this), nullptr);
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
#include "Player.h"

#include "PlayerState.h"
#include "../../Library/Input/InputManager.h"
#include "../../Library/3D/Camera.h"
#include "../../Library/Timer.h"
#include "../../Library/Math/Math.h"
#include "../../Library/Math/Collision.h"
#include "../../Source/Game/Object/Stage/StageManager.h"
#include "../../ImGui/imgui.h"
#include "../Enemy/Enemy.h"
#include "../../Library/ImGui/ConsoleData.h"
#include "../../../../../Library/Particle/EmitterManager.h"
#include "../../../../../Library/Resource/Model/Animator.h"
#include "../../../../UserInterface/DamageTextManager.h"

Player::Player(const char* filePath) : AnimatedObject(filePath)
{
	stateMachine = std::make_unique<StateMachine<Player>>();
	// �P�w�ڃX�e�[�g�o�^
	stateMachine->RegisterState(new PlayerNormalState(this));
	// �Q�w�ڃX�e�[�g�o�^
	stateMachine->RegisterSubState(static_cast<int>(State::Normal), new PlayerIdleState(this));
	stateMachine->RegisterSubState(static_cast<int>(State::Normal), new PlayerWalkState(this));
	stateMachine->RegisterSubState(static_cast<int>(State::Normal), new PlayerRunState(this));
	stateMachine->RegisterSubState(static_cast<int>(State::Normal), new PlayerAttackState(this));
	stateMachine->RegisterSubState(static_cast<int>(State::Normal), new PlayerDodgeState(this));

	auto& animation = model->GetModelResource()->GetAnimationClips();

	Animator::Motion idle;
	idle.motion = &animation[2];
	idle.animationSpeed = 0.15f;

	Animator::Motion walkMotion;
	walkMotion.motion = &animation[4];
	walkMotion.animationSpeed = 0.2f;
	walkMotion.threshold = { 1,0 };

	Animator::Motion runMotion;
	runMotion.motion = &animation[3];
	runMotion.animationSpeed = 0.2f;
	runMotion.threshold = { 1,0 };

	Animator::BlendTree walkTree;
	walkTree.motions.emplace_back(idle);
	walkTree.motions.emplace_back(walkMotion);
	walkTree.maxSeconds = 1;
	walkTree.parameters[0] = "moveX";
	walkTree.parameters[1] = "moveY";

	Animator::BlendTree runTree;
	runTree.motions.emplace_back(idle);
	runTree.motions.emplace_back(runMotion);
	runTree.maxSeconds = 1;
	runTree.parameters[0] = "moveX";
	runTree.parameters[1] = "moveY";

	Animator::State walkState;
	walkState.object = Animator::MakeObjPointer(walkTree);
	walkState.type = Animator::State::BLEND_TREE;
	walkState.transitions.emplace_back(
		STATE_FUNC(animator) {
		if (animator.GetParameter<bool>("run"))
			return &animator.GetState("run");
		return nullptr;
		}
	);

	Animator::State runState;
	runState.object = Animator::MakeObjPointer(runTree);
	runState.type = Animator::State::BLEND_TREE;
	runState.transitions.emplace_back(
		STATE_FUNC(animator) {
		if (!animator.GetParameter<bool>("run"))
			return &animator.GetState("walk");
		return nullptr;
		}
	);

	animator.AddState("walk", walkState);
	animator.AddState("run", runState);
	animator.SetParameter("run", false);
	animator.SetParameter("moveX", 0.f);
	animator.SetParameter("moveY", 0.f);
	animator.SetModelSceneView(&model->GetModelResource()->GetSceneView());
	animator.SetEntryState("walk");
}

void Player::Initialize()
{
	// �X�e�[�g�Z�b�g
	stateMachine->SetState(static_cast<int>(State::Normal));

	// idle �A�j���[�V�����Đ�
	PlayAnimation(static_cast<int>(PlayerAnimNum::Idle), true);
}

void Player::Update()
{
	// ���̓f�[�^�擾
	Input();


	// �X�e�[�g�}�V���X�V
	stateMachine->Update();

	// �G�Ƃ̓����蔻��
	CollisionVsEnemy();

	// �A�j���[�V�����X�V
	UpdateAnimation();

	// �p���s��X�V
	UpdateTransform();
}

void Player::Render(bool isShadow)
{
	// Animator���g�������[�V����
	ModelResource::KeyFrame keyFrame = animator.PlayAnimation(Timer::Instance().DeltaTime());
	model->Render(transform, &keyFrame, isShadow);
}

void Player::DrawDebugImGui(int number)
{
	ImGui::Begin("Player");
	{
		ImGui::DragFloat("emissive", &GetModel()->data.emissivePower, 0.01f);
		ImGui::SliderFloat("roughness", &GetModel()->data.roughnessPower, -1.0f, 1.0f);
		ImGui::SliderFloat("metalness", &GetModel()->data.metalnessPower, -1.0f, 1.0f);

		//ImGui::ShowDemoWindow();
		if(ImGui::CollapsingHeader(u8"�X�e�[�^�X")) {
			if(ImGui::TreeNode(u8"�\�͒l")) {
				ImGui::DragFloat(u8"�̗�", &ability.hp, 1);
				ImGui::DragFloat(u8"�U����", &ability.strength, 0.1f);
				ImGui::DragFloat(u8"�ړ����x", &ability.moveSpeed, 0.1f);

				ImGui::TreePop();
			}

			if(ImGui::TreeNode(u8"�萔�l")) {
				ImGui::DragFloat(u8"�ő�̗�", &constant.maxHp);
				ImGui::DragFloat(u8"�_�b�V���ڍs����", &constant.shiftDashTimer,0.01f);
				ImGui::DragFloat(u8"�_�b�V���{��", &constant.dashPower,0.01f);

				static float rate = constant.dashDeadZone * 100;
				if(ImGui::SliderFloat(u8"�_�b�V���f�b�g�]�[��", &rate,0,100,"%.0f%%"))
					constant.dashDeadZone = rate / 100;

				ImGui::TreePop();
			}

		}
	}
	ImGui::End();

	animator.AnimationEditor();
}

// ���̓f�[�^�擾
void Player::Input()
{
	InputManager& input = InputManager::Instance();

	// --- �ړ� ---
	Vector2 inputMoveData;
	inputMoveData.x = input.GetKeyPress(Keyboard::D) - input.GetKeyPress(Keyboard::A);
	inputMoveData.y = input.GetKeyPress(Keyboard::W) - input.GetKeyPress(Keyboard::S);

	// �R���g���[���[�Ή�
	if (input.IsGamePadConnected() && inputMoveData.x == 0 && inputMoveData.y == 0)
	{
		inputMoveData.x = input.GetThumSticksLeftX();
		inputMoveData.y = input.GetThumSticksLeftY();
	}

	if (inputMoveData.LengthSq() > 1)inputMoveData.Normalize();
	/*if (inputMoveData.LengthSq() <= FLT_EPSILON)*/ {
		if(XMFLOAT2* move = std::get_if<XMFLOAT2>(&inputMap["Move"])) {
			XMVECTOR v1 = XMLoadFloat2(move);
			XMVECTOR v2 = XMLoadFloat2(&inputMoveData.vec_);
			XMStoreFloat2(&inputMoveData.vec_, v1 * XMVector3Length(XMVectorLerp(v1, v2, Timer::Instance().DeltaTime() * 10)));
		}
	}

	inputMap["Move"] = inputMoveData.vec_;
	animator.SetParameter("moveX", inputMoveData.Length());

	// --- ���� ---
	static bool  inputRunData = false;
	static float timer        = 0;

	if (inputMoveData.Length() > constant.dashDeadZone) timer += Timer::Instance().DeltaTime();
	else timer = 0;

	inputRunData = timer >= constant.shiftDashTimer;
	inputMap["Run"] = inputRunData;
	animator.SetParameter("run", inputRunData);

	// --- �U�� ---
	bool inputAttackData = input.GetKeyPressed(DirectX::Keyboard::Enter);

	// �R���g���[���[�Ή�
	if (input.IsGamePadConnected() && !inputAttackData)
	{
		inputAttackData = input.GetGamePadButtonPressed(GAMEPADBUTTON_STATE::y);
	}
	inputMap["Attack"] = inputAttackData;

	// --- ��� ---
	bool dodge = input.GetKeyPressed(DirectX::Keyboard::Space);

	if (input.IsGamePadConnected() && !dodge)
		dodge = input.GetGamePadButtonPress(GAMEPADBUTTON_STATE::a);

	inputMap["Dodge"] = dodge;
	if (dodge)inputMap["DodgeMove"] = inputMoveData.vec_;

	// // --- �h�����N ---
	// bool inputDrinkData = InputManager::Instance().GetKeyPressed(DirectX::Keyboard::E);
	//
	// // �R���g���[���[�Ή�
	// if (input.IsGamePadConnected() && !inputDrinkData)
	// {
	// 	inputDrinkData = input.GetGamePadButtonPressed(GAMEPADBUTTON_STATE::x);
	// }
	// inputMap["Drink"] = inputDrinkData;
}

// �ړ��ʌv�Z
void Player::CalcWalkVelocity()
{
	// �J�����̕����ɑΉ�����ړ�
	float deltaTime = Timer::Instance().DeltaTime();

#if 0
	velocity.x += Camera::Instance().GetFront().x * GetInputMap<DirectX::XMFLOAT2>("Move").y * ability.moveSpeed * deltaTime;
	velocity.z += Camera::Instance().GetFront().z * GetInputMap<DirectX::XMFLOAT2>("Move").y * ability.moveSpeed * deltaTime;

	velocity.x += Camera::Instance().GetRight().x * GetInputMap<DirectX::XMFLOAT2>("Move").x * ability.moveSpeed * deltaTime;
	velocity.z += Camera::Instance().GetRight().z * GetInputMap<DirectX::XMFLOAT2>("Move").x * ability.moveSpeed * deltaTime;
#else
	const XMFLOAT2 move = GetInputMap<XMFLOAT2>("Move");
	velocity.x += camera->GetFrontVec().x * move.y * ability.moveSpeed * deltaTime;
	velocity.z += camera->GetFrontVec().z * move.y * ability.moveSpeed * deltaTime;

	velocity.x += camera->GetRightVec().x * move.x * ability.moveSpeed * deltaTime;
	velocity.z += camera->GetRightVec().z * move.x * ability.moveSpeed * deltaTime;
#endif
}

// ����ړ��ʌv�Z
void Player::CalcRunVelocity()
{
	float runPower = constant.dashPower;

	// �J�����̕����ɑΉ�����ړ�
	float deltaTime = Timer::Instance().DeltaTime();
#if 0
	velocity.x += Camera::Instance().GetFront().x * GetInputMap<DirectX::XMFLOAT2>("Move").y * ability.moveSpeed * runPower *
		deltaTime;
	velocity.z += Camera::Instance().GetFront().z * GetInputMap<DirectX::XMFLOAT2>("Move").y * ability.moveSpeed * runPower *
		deltaTime;

	velocity.x += Camera::Instance().GetRight().x * GetInputMap<DirectX::XMFLOAT2>("Move").x * ability.moveSpeed * runPower *
		deltaTime;
	velocity.z += Camera::Instance().GetRight().z * GetInputMap<DirectX::XMFLOAT2>("Move").x * ability.moveSpeed * runPower *
		deltaTime;

#else
	const XMFLOAT2& move = GetInputMap<XMFLOAT2>("Move");
	velocity.x += camera->GetFrontVec().x * move.y * ability.moveSpeed * runPower * deltaTime;
	velocity.z += camera->GetFrontVec().z * move.y * ability.moveSpeed * runPower * deltaTime;

	velocity.x += camera->GetRightVec().x * move.x * ability.moveSpeed * runPower * deltaTime;
	velocity.z += camera->GetRightVec().z * move.x * ability.moveSpeed * runPower * deltaTime;
#endif
}

void Player::CalcDodgeVelocity() {
#if 1 // TODO::RootMotion���g���Ȃ����
	float dt = Timer::Instance().DeltaTime();
	const XMFLOAT2& move = GetInputMap<XMFLOAT2>("DodgeMove");
	Vector3 vec;

	vec.x += camera->GetFrontVec().x * move.y;
	vec.z += camera->GetFrontVec().z * move.y;
	vec.x += camera->GetRightVec().x * move.x;
	vec.z += camera->GetRightVec().z * move.x;

	vec *= ability.moveSpeed * constant.dodgePower * dt;

	velocity += vec.vec_;
#else
	velocity += animator.GetVelocity();

#endif
}

// �ړ����Ă�������Ɍ���
void Player::Turn()
{
	// �ړ����Ă��Ȃ���� return 
	if (Float3Length(velocity) < 0.0001f) return;

	// �v���C���[�̑O�����x�N�g�����擾(sinf, cosf ���g�p���Ă��邽�ߏ���ɒP�ʃx�N�g���ɂȂ��Ă���)
	DirectX::XMFLOAT3 front =
	{
		sinf(DirectX::XMConvertToRadians(angle.y)),
		0,
		cosf(DirectX::XMConvertToRadians(angle.y))
	};
	// �ړ������̒P�ʃx�N�g���擾
	DirectX::XMVECTOR N_VELOCITY = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&velocity));
	DirectX::XMFLOAT3 normalizeVelocity;
	DirectX::XMStoreFloat3(&normalizeVelocity, N_VELOCITY);

	// ���ς��v�Z���ĉ�]�p�x�����߂�
	float dot      = (front.x * normalizeVelocity.x) + (front.z * normalizeVelocity.z);
	float rotPower = (1.0f - dot) * rot;
	if (rotPower > rot) rotPower = rot;

	// �O�όv�Z
	float cross = (front.x * normalizeVelocity.z) - (front.z * normalizeVelocity.x);

	// ��]
	float deltaTime = Timer::Instance().DeltaTime();
	angle.y += (cross > 0) ? (-rotPower * deltaTime) : (rotPower * deltaTime);
}

// �ړ�
void Player::Move()
{
	position.x += velocity.x;
	position.z += velocity.z;
	velocity *= 0.01f;
}

// �G�Ƃ̏Փˏ���
void Player::CollisionVsEnemy()
{
	for (auto& eBoneSphere : Enemy::Instance().GetModel()->GetModelResource()->GetSkeletonSphereCollisions())
	{
		// ===== �̓��m�̓����蔻�� =====
		for (auto& boneSphere : model->GetModelResource()->GetSkeletonSphereCollisions())
		{
			// ��������Q�̃{�[���̍��W�v�Z
			DirectX::XMFLOAT3 bonePos;
			DirectX::XMFLOAT3 eBonePos;
			if (boneSphere.name != "")
			{
				bonePos = GetBonePosition(boneSphere.name);
			}
			else
			{
				DirectX::XMVECTOR BONE_POS = DirectX::XMLoadFloat3(&boneSphere.position);
				DirectX::XMVECTOR POS      = DirectX::XMLoadFloat3(&position);
				DirectX::XMStoreFloat3(&bonePos, DirectX::XMVectorAdd(BONE_POS, POS));
			}
			if (eBoneSphere.name != "")
			{
				eBonePos = Enemy::Instance().GetBonePosition(eBoneSphere.name);
			}
			else
			{
				DirectX::XMFLOAT3 ePos     = Enemy::Instance().GetPos();
				DirectX::XMVECTOR BONE_POS = DirectX::XMLoadFloat3(&eBoneSphere.position);
				DirectX::XMVECTOR POS      = DirectX::XMLoadFloat3(&ePos);
				DirectX::XMStoreFloat3(&eBonePos, DirectX::XMVectorAdd(BONE_POS, POS));
			}

			// �Փˏ���
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsSphere(
					eBonePos,
					eBoneSphere.radius,
					bonePos,
					boneSphere.radius,
					outPosition)
			)
			{
				// �����o����̈ʒu�ݒ�
				outPosition.y = 0;
				SetPos(outPosition);
			}
		}

		// ===== �����̍U���ƓG�̑̂̓����蔻�� =====
		for(auto& playerAnimSphereCollision : model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex).animSphereCollisions)
		{
			// ���Ƀ_���[�W��^���Ă��邩�`�F�b�N
			if (playerAnimSphereCollision.isDamaged) continue;
			// �t���[���͈͓����`�F�b�N
			if (currentKeyFrame < playerAnimSphereCollision.startFrame || currentKeyFrame > playerAnimSphereCollision.endFrame) continue;

			// ��������Q�̃{�[���̍��W�v�Z
			DirectX::XMFLOAT3 bonePos;
			DirectX::XMFLOAT3 eBonePos;
			if (playerAnimSphereCollision.bindBoneName != "")
			{
				bonePos = GetBonePosition(playerAnimSphereCollision.bindBoneName);
			}
			else
			{
				DirectX::XMVECTOR BONE_POS = DirectX::XMLoadFloat3(&playerAnimSphereCollision.position);
				DirectX::XMVECTOR POS = DirectX::XMLoadFloat3(&position);
				DirectX::XMStoreFloat3(&bonePos, DirectX::XMVectorAdd(BONE_POS, POS));
			}
			if (eBoneSphere.name != "")
			{
				eBonePos = Enemy::Instance().GetBonePosition(eBoneSphere.name);
			}
			else
			{
				DirectX::XMFLOAT3 ePos = Enemy::Instance().GetPos();
				DirectX::XMVECTOR BONE_POS = DirectX::XMLoadFloat3(&eBoneSphere.position);
				DirectX::XMVECTOR POS = DirectX::XMLoadFloat3(&ePos);
				DirectX::XMStoreFloat3(&eBonePos, DirectX::XMVectorAdd(BONE_POS, POS));
			}

			// �Փˏ���
			DirectX::XMFLOAT3 collisionPoint;
			if (Collision::SphereVsSphereCollisionPoint(
				eBonePos,
				eBoneSphere.radius,
				bonePos,
				playerAnimSphereCollision.radius,
				collisionPoint)
				)
			{
				playerAnimSphereCollision.isDamaged = true;

				ConsoleData::Instance().logs.push_back("Damage!");

				Emitter* emitter = new Emitter();
				emitter->position = collisionPoint;
				emitter->emitterData.duration = 2.0;
				emitter->emitterData.looping = false;
				emitter->emitterData.burstsTime = 0.0;
				emitter->emitterData.burstsCount = 256;
				emitter->emitterData.particleKind = pk_PlayerAttackSpark;
				emitter->emitterData.particleLifeTimeMin = 0.4f;
				emitter->emitterData.particleLifeTimeMax = 0.6f;
				emitter->emitterData.particleSpeedMin = 15.0f;
				emitter->emitterData.particleSpeedMax = 30.0f;
				emitter->emitterData.particleSizeMin = { 0.25f, 0.005f };
				emitter->emitterData.particleSizeMax = { 2.0f, 0.1f };
				emitter->emitterData.particleColorMin = { 3.5, 3.5, 0.8, 1 };
				emitter->emitterData.particleColorMax = { 6.0, 6.0, 0.8, 1 };
				emitter->emitterData.particleGravity = 20;
				emitter->emitterData.particleBillboardType = 1;
				emitter->emitterData.particleTextureType = 0;
				emitter->emitterData.burstsOneShot = true;
				EmitterManager::Instance().Register(emitter);

				Emitter* emitter1 = new Emitter();
				emitter1->position = collisionPoint;
				emitter1->emitterData.duration = 2.0;
				emitter1->emitterData.looping = false;
				emitter1->emitterData.burstsTime = 0.03;
				emitter1->emitterData.burstsCount = 1;
				emitter1->emitterData.particleKind = pk_PlayerAttackHit;
				emitter1->emitterData.particleLifeTimeMin = 0.1f;
				emitter1->emitterData.particleLifeTimeMax = 0.1f;
				emitter1->emitterData.particleSpeedMin = 0.0f;
				emitter1->emitterData.particleSpeedMax = 0.0f;
				emitter1->emitterData.particleSizeMin = { 50.0f, 5.0f };
				emitter1->emitterData.particleSizeMax = { 50.0f, 5.0f };
				emitter1->emitterData.particleColorMin = { 1.9, 1.9, 8.8, 1 };
				emitter1->emitterData.particleColorMax = { 1.9, 1.9, 10.8, 1 };
				emitter1->emitterData.particleAngleMin = 0;
				emitter1->emitterData.particleAngleMax = 359;
				emitter1->emitterData.particleGravity = 20;
				emitter1->emitterData.particleBillboardType = 2;
				emitter1->emitterData.particleTextureType = 1;
				emitter1->emitterData.burstsOneShot = 2;

				emitter1->emitRateTimer = emitter1->emitterData.burstsTime;	// �������ɔ�������悤�ɂ���
				EmitterManager::Instance().Register(emitter1);

				Emitter* emitter2 = new Emitter();
				emitter2->position = collisionPoint;
				emitter2->emitterData.duration = 2.0;
				emitter2->emitterData.looping = false;
				emitter2->emitterData.burstsTime = 0.0;
				emitter2->emitterData.burstsCount = 1;
				emitter2->emitterData.particleKind = 3;
				emitter2->emitterData.particleLifeTimeMin = 0.05f;
				emitter2->emitterData.particleLifeTimeMax = 0.05f;
				emitter2->emitterData.particleSpeedMin = 0.0f;
				emitter2->emitterData.particleSpeedMax = 0.0f;
				emitter2->emitterData.particleSizeMin = { 5.0f, 5.0f };
				emitter2->emitterData.particleSizeMax = { 5.0f, 5.0f };
				emitter2->emitterData.particleColorMin = { 8.5, 3.0, 0.8, 1 };
				emitter2->emitterData.particleColorMax = { 8.5, 3.0, 0.8, 1 };
				emitter2->emitterData.particleAngleMin = 0;
				emitter2->emitterData.particleAngleMax = 359;
				emitter2->emitterData.particleGravity = 20;
				emitter2->emitterData.particleBillboardType = 2;
				emitter2->emitterData.particleTextureType = 2;
				emitter2->emitterData.burstsOneShot = 1;
				EmitterManager::Instance().Register(emitter2);

				int dmg = rand() % 20 + 1;
				std::string dmgText = std::to_string(dmg);
				DamageTextManager::Instance().Register({ dmgText, collisionPoint });
			}
		}
	}
}

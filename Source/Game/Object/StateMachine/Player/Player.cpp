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
	stateMachine->RegisterSubState(static_cast<int>(State::Normal), new PlayerDrinkState(this));
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
	model->Render(transform, &keyFrame, isShadow);
}

void Player::DrawDebugImGui(int number)
{
}

// ���̓f�[�^�擾
void Player::Input()
{
	InputManager* input = &InputManager::Instance();

	// --- �ړ� ---
	DirectX::XMFLOAT2 inputMoveData;
	inputMoveData.x = input->GetKeyPress(Keyboard::D) - input->GetKeyPress(Keyboard::A);
	inputMoveData.y = input->GetKeyPress(Keyboard::W) - input->GetKeyPress(Keyboard::S);

	// �R���g���[���[�Ή�
	if (input->IsGamePadConnected() && inputMoveData.x == 0 && inputMoveData.y == 0)
	{
		inputMoveData.x = input->GetThumSticksLeftX();
		inputMoveData.y = input->GetThumSticksLeftY();
	}
	inputMap["Move"] = inputMoveData;

	// --- ���� ---
	bool inputRunData;
	inputRunData = InputManager::Instance().GetKeyPress(DirectX::Keyboard::LeftShift);

	// �R���g���[���[�Ή�
	if (input->IsGamePadConnected() && !inputRunData)
	{
		inputRunData = input->GetGamePadButtonPress(GAMEPADBUTTON_STATE::rightShoulder);
	}
	inputMap["Run"] = inputRunData;

	// --- �U�� ---
	bool inputAttackData;
	inputAttackData = InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Enter);

	// �R���g���[���[�Ή�
	if (input->IsGamePadConnected() && !inputAttackData)
	{
		inputAttackData = input->GetGamePadButtonPressed(GAMEPADBUTTON_STATE::y);
	}
	inputMap["Attack"] = inputAttackData;

	// --- �h�����N ---
	bool inputDrinkData;
	inputDrinkData = InputManager::Instance().GetKeyPressed(DirectX::Keyboard::E);

	// �R���g���[���[�Ή�
	if (input->IsGamePadConnected() && !inputDrinkData)
	{
		inputDrinkData = input->GetGamePadButtonPressed(GAMEPADBUTTON_STATE::x);
	}
	inputMap["Drink"] = inputDrinkData;
}

// �ړ��ʌv�Z
void Player::CalcWalkVelocity()
{
	// �J�����̕����ɑΉ�����ړ�
	float deltaTime = Timer::Instance().DeltaTime();
	velocity.x += Camera::Instance().GetFront().x * GetInputMap<DirectX::XMFLOAT2>("Move").y * moveSpeed * deltaTime;
	velocity.z += Camera::Instance().GetFront().z * GetInputMap<DirectX::XMFLOAT2>("Move").y * moveSpeed * deltaTime;

	velocity.x += Camera::Instance().GetRight().x * GetInputMap<DirectX::XMFLOAT2>("Move").x * moveSpeed * deltaTime;
	velocity.z += Camera::Instance().GetRight().z * GetInputMap<DirectX::XMFLOAT2>("Move").x * moveSpeed * deltaTime;
}

// ����ړ��ʌv�Z
void Player::CalcRunVelocity()
{
	float runPower = 3.0f;

	// �J�����̕����ɑΉ�����ړ�
	float deltaTime = Timer::Instance().DeltaTime();
	velocity.x += Camera::Instance().GetFront().x * GetInputMap<DirectX::XMFLOAT2>("Move").y * moveSpeed * runPower *
		deltaTime;
	velocity.z += Camera::Instance().GetFront().z * GetInputMap<DirectX::XMFLOAT2>("Move").y * moveSpeed * runPower *
		deltaTime;

	velocity.x += Camera::Instance().GetRight().x * GetInputMap<DirectX::XMFLOAT2>("Move").x * moveSpeed * runPower *
		deltaTime;
	velocity.z += Camera::Instance().GetRight().z * GetInputMap<DirectX::XMFLOAT2>("Move").x * moveSpeed * runPower *
		deltaTime;
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
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsSphere(
				eBonePos,
				eBoneSphere.radius,
				bonePos,
				playerAnimSphereCollision.radius,
				outPosition)
				)
			{
				playerAnimSphereCollision.isDamaged = true;

				ConsoleData::Instance().logs.push_back("Damage!");

				Emitter* emitter = new Emitter();
				emitter->position = outPosition;
				emitter->emitterData.duration = 2.0;
				emitter->emitterData.looping = false;
				emitter->emitterData.burstsTime = 0.0;
				emitter->emitterData.burstsCount = 30;
				emitter->emitterData.particleKind = 1;
				emitter->emitterData.particleLifeTimeMin = 0.4f;
				emitter->emitterData.particleLifeTimeMax = 0.6f;
				emitter->emitterData.particleSpeedMin = 15.0f;
				emitter->emitterData.particleSpeedMax = 30.0f;
				emitter->emitterData.particleSizeMin = { 0.25f, 0.005f };
				emitter->emitterData.particleSizeMax = { 2.0f, 0.1f };
				emitter->emitterData.particleColorMin = { 3.5, 3.5, 0.8, 1 };
				emitter->emitterData.particleColorMax = { 6.0, 6.0, 0.8, 1 };
				emitter->emitterData.particleFrictionMin = 0;
				emitter->emitterData.particleFrictionMax = 0.01;
				emitter->emitterData.particleGravity = 20;
				emitter->emitterData.particleBillboardType = 1;
				emitter->emitterData.particleTextureType = 0;
				emitter->emitterData.burstsOneShot = true;
				EmitterManager::Instance().Register(emitter);

				Emitter* emitter1 = new Emitter();
				emitter1->position = outPosition;
				emitter1->emitterData.duration = 2.0;
				emitter1->emitterData.looping = false;
				emitter1->emitterData.burstsTime = 0.03;
				emitter1->emitterData.burstsCount = 1;
				emitter1->emitterData.particleKind = 2;
				emitter1->emitterData.particleLifeTimeMin = 0.1f;
				emitter1->emitterData.particleLifeTimeMax = 0.1f;
				emitter1->emitterData.particleSpeedMin = 0.0f;
				emitter1->emitterData.particleSpeedMax = 0.0f;
				emitter1->emitterData.particleSizeMin = { 50.0f, 5.0f };
				emitter1->emitterData.particleSizeMax = { 50.0f, 5.0f };
				emitter1->emitterData.particleColorMin = { 1.9, 1.9, 8.8, 1 };
				emitter1->emitterData.particleColorMax = { 1.9, 1.9, 10.8, 1 };
				emitter1->emitterData.particleFrictionMin = 0;
				emitter1->emitterData.particleFrictionMax = 0.01;
				emitter1->emitterData.particleAngleMin = 0;
				emitter1->emitterData.particleAngleMax = 359;
				emitter1->emitterData.particleGravity = 20;
				emitter1->emitterData.particleBillboardType = 2;
				emitter1->emitterData.particleTextureType = 1;
				emitter1->emitterData.burstsOneShot = 2;

				emitter1->emitRateTimer = emitter1->emitterData.burstsTime;	// �������ɔ�������悤�ɂ���
				EmitterManager::Instance().Register(emitter1);

				Emitter* emitter2 = new Emitter();
				emitter2->position = outPosition;
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
				emitter2->emitterData.particleFrictionMin = 0;
				emitter2->emitterData.particleFrictionMax = 0.01;
				emitter2->emitterData.particleAngleMin = 0;
				emitter2->emitterData.particleAngleMax = 359;
				emitter2->emitterData.particleGravity = 20;
				emitter2->emitterData.particleBillboardType = 2;
				emitter2->emitterData.particleTextureType = 2;
				emitter2->emitterData.burstsOneShot = 1;
				EmitterManager::Instance().Register(emitter2);

				int dmg = rand() % 20 + 1;
				std::string dmgText = std::to_string(dmg);
				DamageTextManager::Instance().Register({ dmgText, outPosition });
			}
		}
	}
}

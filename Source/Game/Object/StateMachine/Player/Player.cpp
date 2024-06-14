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

Player::Player(const char* filePath) : AnimatedObject(filePath)
{
	stateMachine = std::make_unique<StateMachine<Player>>();
	// １層目ステート登録
	stateMachine->RegisterState(new PlayerNormalState(this));
	// ２層目ステート登録
	stateMachine->RegisterSubState(static_cast<int>(State::Normal), new PlayerIdleState(this));
	stateMachine->RegisterSubState(static_cast<int>(State::Normal), new PlayerWalkState(this));
	stateMachine->RegisterSubState(static_cast<int>(State::Normal), new PlayerRunState(this));
	stateMachine->RegisterSubState(static_cast<int>(State::Normal), new PlayerAttackState(this));
	stateMachine->RegisterSubState(static_cast<int>(State::Normal), new PlayerDrinkState(this));
}

void Player::Initialize()
{
	// ステートセット
	stateMachine->SetState(static_cast<int>(State::Normal));

	// idle アニメーション再生
	PlayAnimation(static_cast<int>(PlayerAnimNum::Idle), true);
}

void Player::Update()
{
	// 入力データ取得
	Input();


	// ステートマシン更新
	stateMachine->Update();

	// 敵との当たり判定
	CollisionVsEnemy();

	// アニメーション更新
	UpdateAnimation();

	// 姿勢行列更新
	UpdateTransform();
}

void Player::Render(bool isShadow)
{
	model->Render(transform, &keyFrame, isShadow);
}

void Player::DrawDebugImGui(int number)
{
}

// 入力データ取得
void Player::Input()
{
	InputManager* input = &InputManager::Instance();

	// --- 移動 ---
	DirectX::XMFLOAT2 inputMoveData;
	inputMoveData.x = input->GetKeyPress(Keyboard::D) - input->GetKeyPress(Keyboard::A);
	inputMoveData.y = input->GetKeyPress(Keyboard::W) - input->GetKeyPress(Keyboard::S);

	// コントローラー対応
	if (input->IsGamePadConnected() && inputMoveData.x == 0 && inputMoveData.y == 0)
	{
		inputMoveData.x = input->GetThumSticksLeftX();
		inputMoveData.y = input->GetThumSticksLeftY();
	}
	inputMap["Move"] = inputMoveData;

	// --- 走り ---
	bool inputRunData;
	inputRunData = InputManager::Instance().GetKeyPress(DirectX::Keyboard::LeftShift);

	// コントローラー対応
	if (input->IsGamePadConnected() && !inputRunData)
	{
		inputRunData = input->GetGamePadButtonPress(GAMEPADBUTTON_STATE::rightShoulder);
	}
	inputMap["Run"] = inputRunData;

	// --- 攻撃 ---
	bool inputAttackData;
	inputAttackData = InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Enter);

	// コントローラー対応
	if (input->IsGamePadConnected() && !inputAttackData)
	{
		inputAttackData = input->GetGamePadButtonPressed(GAMEPADBUTTON_STATE::y);
	}
	inputMap["Attack"] = inputAttackData;

	// --- ドリンク ---
	bool inputDrinkData;
	inputDrinkData = InputManager::Instance().GetKeyPressed(DirectX::Keyboard::E);

	// コントローラー対応
	if (input->IsGamePadConnected() && !inputDrinkData)
	{
		inputDrinkData = input->GetGamePadButtonPressed(GAMEPADBUTTON_STATE::x);
	}
	inputMap["Drink"] = inputDrinkData;
}

// 移動量計算
void Player::CalcWalkVelocity()
{
	// カメラの方向に対応する移動
	float deltaTime = Timer::Instance().DeltaTime();
	velocity.x += Camera::Instance().GetFront().x * GetInputMap<DirectX::XMFLOAT2>("Move").y * moveSpeed * deltaTime;
	velocity.z += Camera::Instance().GetFront().z * GetInputMap<DirectX::XMFLOAT2>("Move").y * moveSpeed * deltaTime;

	velocity.x += Camera::Instance().GetRight().x * GetInputMap<DirectX::XMFLOAT2>("Move").x * moveSpeed * deltaTime;
	velocity.z += Camera::Instance().GetRight().z * GetInputMap<DirectX::XMFLOAT2>("Move").x * moveSpeed * deltaTime;
}

// 走り移動量計算
void Player::CalcRunVelocity()
{
	float runPower = 3.0f;

	// カメラの方向に対応する移動
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

// 移動している方向に向く
void Player::Turn()
{
	// 移動していなければ return 
	if (Float3Length(velocity) < 0.0001f) return;

	// プレイヤーの前方向ベクトルを取得(sinf, cosf を使用しているため勝手に単位ベクトルになっている)
	DirectX::XMFLOAT3 front =
	{
		sinf(DirectX::XMConvertToRadians(angle.y)),
		0,
		cosf(DirectX::XMConvertToRadians(angle.y))
	};
	// 移動方向の単位ベクトル取得
	DirectX::XMVECTOR N_VELOCITY = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&velocity));
	DirectX::XMFLOAT3 normalizeVelocity;
	DirectX::XMStoreFloat3(&normalizeVelocity, N_VELOCITY);

	// 内積を計算して回転角度を求める
	float dot      = (front.x * normalizeVelocity.x) + (front.z * normalizeVelocity.z);
	float rotPower = (1.0f - dot) * rot;
	if (rotPower > rot) rotPower = rot;

	// 外積計算
	float cross = (front.x * normalizeVelocity.z) - (front.z * normalizeVelocity.x);

	// 回転
	float deltaTime = Timer::Instance().DeltaTime();
	angle.y += (cross > 0) ? (-rotPower * deltaTime) : (rotPower * deltaTime);
}

// 移動
void Player::Move()
{
	position.x += velocity.x;
	position.z += velocity.z;
	velocity *= 0.01f;
}

// 敵との衝突処理
void Player::CollisionVsEnemy()
{
	for (auto& eBoneSphere : Enemy::Instance().GetModel()->GetModelResource()->GetSkeletonSphereCollisions())
	{
		// ===== 体同士の当たり判定 =====
		for (auto& boneSphere : model->GetModelResource()->GetSkeletonSphereCollisions())
		{
			// 処理する２つのボーンの座標計算
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

			// 衝突処理
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsSphere(
					eBonePos,
					eBoneSphere.radius,
					bonePos,
					boneSphere.radius,
					outPosition)
			)
			{
				// 押し出し後の位置設定
				outPosition.y = 0;
				SetPos(outPosition);
			}
		}

		// ===== 自分の攻撃と敵の体の当たり判定 =====
		for(auto& playerAnimSphereCollision : model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex).animSphereCollisions)
		{
			// 既にダメージを与えているかチェック
			if (playerAnimSphereCollision.isDamaged) continue;
			// フレーム範囲内かチェック
			if (currentKeyFrame < playerAnimSphereCollision.startFrame || currentKeyFrame > playerAnimSphereCollision.endFrame) continue;

			// 処理する２つのボーンの座標計算
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

			// 衝突処理
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
				emitter->emitterData.duration = 0.2;
				emitter->emitterData.looping = false;
				emitter->emitterData.burstsTime = 0.1;
				emitter->emitterData.burstsCount = 99;
				emitter->emitterData.particleKind = 1;
				emitter->emitterData.particleLifeTimeMin = 0.4f;
				emitter->emitterData.particleLifeTimeMax = 0.6f;
				emitter->emitterData.particleSpeedMin = 15.0f;
				emitter->emitterData.particleSpeedMax = 30.0f;
				emitter->emitterData.particleSizeMin = { 0.25f, 0.005f };
				emitter->emitterData.particleSizeMax = { 2.0f, 0.1f };
				emitter->emitterData.particleColorMin = { 2.5, 0.8, 0.8, 1 };
				emitter->emitterData.particleColorMax = { 4.0, 0.8, 0.8, 1 };
				emitter->emitterData.particleFrictionMin = 0;
				emitter->emitterData.particleFrictionMax = 0.01;
				emitter->emitterData.particleGravity = 20;
				emitter->emitterData.particleBillboardType = 1;
				EmitterManager::Instance().Register(emitter);
			}
		}
	}
}

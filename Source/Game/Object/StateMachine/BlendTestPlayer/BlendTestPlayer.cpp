#include "BlendTestPlayer.h"

#include "BlendTestPlayerState.h"
#include "../../Library/Input/InputManager.h"
#include "../../Library/3D/Camera.h"
#include "../../Library/Timer.h"
#include "../../Library/Math/Math.h"
#include "../../Library/Math/Collision.h"
#include "../../Source/Game/Object/Stage/StageManager.h"
#include "../../ImGui/imgui.h"

BlendTestPlayer::BlendTestPlayer(const char* filePath) : AnimatedObject(filePath)
{
	stateMachine = std::make_unique<StateMachine<BlendTestPlayer>>();
	// １層目ステート登録
	stateMachine->RegisterState(new BlendTestPlayerNormalState(this));
	// ２層目ステート登録
	stateMachine->RegisterSubState(static_cast<int>(State::Normal), new BlendTestPlayerTestState(this));
	// ステートセット
	stateMachine->SetState(static_cast<int>(State::Normal));
}

void BlendTestPlayer::Update()
{
	// 入力データ取得
	Input();


	// ステートマシン更新
	stateMachine->Update();


	// アニメーション更新
	UpdateBlendAnimation();

	// 姿勢行列更新
	UpdateTransform();
}

void BlendTestPlayer::Render(bool isShadow)
{
	model->Render(transform, &keyFrame, isShadow);
}

void BlendTestPlayer::DrawDebugImGui(int number)
{
	ImGui::Begin("Player");
	ImGui::InputFloat3("Velocity", &velocity.x);
	ImGui::End();
}

// 入力データ取得
void BlendTestPlayer::Input()
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
void BlendTestPlayer::CalcVelocity()
{
	float deltaTime = Timer::Instance().DeltaTime();

	if (Float3Length(velocity) > friction * deltaTime)
	{
		velocity.x -= velocity.x * (friction * deltaTime);
		velocity.z -= velocity.z * (friction * deltaTime);
	}
	else
	{
		velocity = { 0,0,0, };
	}

	// カメラの方向に対応する移動
	velocity.x += Camera::Instance().GetFront().x * GetInputMap<DirectX::XMFLOAT2>("Move").y * moveSpeed * deltaTime;
	velocity.z += Camera::Instance().GetFront().z * GetInputMap<DirectX::XMFLOAT2>("Move").y * moveSpeed * deltaTime;

	velocity.x += Camera::Instance().GetRight().x * GetInputMap<DirectX::XMFLOAT2>("Move").x * moveSpeed * deltaTime;
	velocity.z += Camera::Instance().GetRight().z * GetInputMap<DirectX::XMFLOAT2>("Move").x * moveSpeed * deltaTime;

	if (Float3Length(velocity) > maxMoveSpeed)
	{
		velocity.x = velocity.x / Float3Length(velocity) * maxMoveSpeed;
		velocity.z = velocity.z / Float3Length(velocity) * maxMoveSpeed;
	}
}


// 移動している方向に向く
void BlendTestPlayer::Turn()
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
	float dot = (front.x * normalizeVelocity.x) + (front.z * normalizeVelocity.z);
	float rotPower = (1.0f - dot) * rot;
	if (rotPower > rot) rotPower = rot;

	// 外積計算
	float cross = (front.x * normalizeVelocity.z) - (front.z * normalizeVelocity.x);

	// 回転
	float deltaTime = Timer::Instance().DeltaTime();
	angle.y += (cross > 0) ? (-rotPower * deltaTime) : (rotPower * deltaTime);
}

// 移動
void BlendTestPlayer::Move()
{
	


	

	position += velocity;
}

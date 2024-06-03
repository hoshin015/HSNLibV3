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
	// �P�w�ڃX�e�[�g�o�^
	stateMachine->RegisterState(new BlendTestPlayerNormalState(this));
	// �Q�w�ڃX�e�[�g�o�^
	stateMachine->RegisterSubState(static_cast<int>(State::Normal), new BlendTestPlayerTestState(this));
	// �X�e�[�g�Z�b�g
	stateMachine->SetState(static_cast<int>(State::Normal));
}

void BlendTestPlayer::Update()
{
	// ���̓f�[�^�擾
	Input();


	// �X�e�[�g�}�V���X�V
	stateMachine->Update();


	// �A�j���[�V�����X�V
	UpdateBlendAnimation();

	// �p���s��X�V
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

// ���̓f�[�^�擾
void BlendTestPlayer::Input()
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

	// �J�����̕����ɑΉ�����ړ�
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


// �ړ����Ă�������Ɍ���
void BlendTestPlayer::Turn()
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
	float dot = (front.x * normalizeVelocity.x) + (front.z * normalizeVelocity.z);
	float rotPower = (1.0f - dot) * rot;
	if (rotPower > rot) rotPower = rot;

	// �O�όv�Z
	float cross = (front.x * normalizeVelocity.z) - (front.z * normalizeVelocity.x);

	// ��]
	float deltaTime = Timer::Instance().DeltaTime();
	angle.y += (cross > 0) ? (-rotPower * deltaTime) : (rotPower * deltaTime);
}

// �ړ�
void BlendTestPlayer::Move()
{
	


	

	position += velocity;
}

#include "Player.h"
#include "../../Library/Input/InputManager.h"
#include "../../Library/3D/Camera.h"
#include "../../Library/Timer.h"
#include "../../Library/Math/Math.h"
#include "../../Library/Math/Collision.h"
#include "../../Source/Game/Object/Stage/StageManager.h"
#include "../../ImGui/imgui.h"

Player::Player(const char* filePath) : AnimatedObject(filePath)
{
	// idle �A�j���[�V�����Đ�
	PlayAnimation(static_cast<int>(PlayerAnimNum::Idle), true);
}

void Player::Update()
{
	// ���̓f�[�^�擾
	Input();
	// �ړ��ʌv�Z
	CalcMoveVelocity();
	// ��]
	Turn();


	position += velocity;
	velocity *= 0.01f;
	if (Float3Length(velocity) < 0.0001f)
	{
		if (GetCurrentAnimationIndex() != static_cast<int>(PlayerAnimNum::Idle))
		{
			PlayAnimation(static_cast<int>(PlayerAnimNum::Idle), true);
		}
			
		velocity = { 0,0,0 };
	}
	else
	{
		if(GetCurrentAnimationIndex() != static_cast<int>(PlayerAnimNum::Walk))
		{
			PlayAnimation(static_cast<int>(PlayerAnimNum::Walk), true);
		}
	}

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

	DirectX::XMFLOAT2 inputMoveData;
	inputMoveData.x = input->GetKeyPress(Keyboard::D) - input->GetKeyPress(Keyboard::A);
	inputMoveData.y = input->GetKeyPress(Keyboard::W) - input->GetKeyPress(Keyboard::S);

	// �R���g���[���[�Ή�
	if(input->IsGamePadConnected())
	{
		inputMoveData.x = input->GetThumSticksLeftX();
		inputMoveData.y = input->GetThumSticksLeftY();
	}
	inputMap["Move"] = inputMoveData;


}

// �ړ��ʌv�Z
void Player::CalcMoveVelocity()
{
	// �J�����̕����ɑΉ�����ړ�
	float deltaTime = Timer::Instance().DeltaTime();
	velocity.x += Camera::Instance().GetFront().x * GetInputMap<DirectX::XMFLOAT2>("Move").y * moveSpeed * deltaTime;
	velocity.z += Camera::Instance().GetFront().z * GetInputMap<DirectX::XMFLOAT2>("Move").y * moveSpeed * deltaTime;

	velocity.x += Camera::Instance().GetRight().x * GetInputMap<DirectX::XMFLOAT2>("Move").x * moveSpeed * deltaTime;
	velocity.z += Camera::Instance().GetRight().z * GetInputMap<DirectX::XMFLOAT2>("Move").x * moveSpeed * deltaTime;
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
	float dot = (front.x * normalizeVelocity.x) + (front.z * normalizeVelocity.z);
	float rotPower = (1.0f - dot) * rot;
	if (rotPower > rot) rotPower = rot;

	// �O�όv�Z
	float cross = (front.x * normalizeVelocity.z) - (front.z * normalizeVelocity.x);

	// ��]
	float deltaTime = Timer::Instance().DeltaTime();
	angle.y += (cross > 0) ? (-rotPower * deltaTime) : (rotPower * deltaTime);
}

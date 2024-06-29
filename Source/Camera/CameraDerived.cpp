#include "CameraDerived.h"

#include "../../External/ImGui/imgui.h"

#include "../../Library/Timer.h"

#include "../../Library/Input/InputManager.h"

#include "../Game/Object/StateMachine/Player/Player.h"
#include "../Game/Object/StateMachine/Enemy/Enemy.h"


// ===== �v���C���[�̃J���� ======================================================================================================================================================
void PlayerCamera::Initialize()
{
	verticalAngle = 45.0f;
	horizontalAngle = 0.0f;

	// ����� range �ς��Ă܂�
	//range = 13.0f;
	range = 26.0f;

	height = 3.0f;
	fixedCursor = true;
	
	// --- �J�����ʒu�ƖڕW�̐ݒ� ---
	currentPosition = position = Player::Instance().GetPos();
	Vector3 playerVelocity = Vector3::Normalize(Player::Instance().GetVelocity());	// �v���C���[�̈ړ��x�N�g��
	Vector3 offset = { playerVelocity.x, height, playerVelocity.z };
	target = position + offset;
}


void PlayerCamera::Update()
{
	switch (state)
	{
	case 0:
		Initialize();
		state++;
		[[fallthrough]];

	case 1:
	{
		float deltaTime = Timer::Instance().DeltaTime();

		Vector3 playerPos = Player::Instance().GetPos();	// �v���C���[�̍��W
		Vector3 offset = { 0.0f, height, 0.0f };
		target = playerPos + offset;


		// --- �J�[�\�����Œ肷�邩 ---
		if (fixedCursor)
		{
			OnFixedCursor(deltaTime);
		}

		// --- �p�x�Ńx�N�g��������ăJ�����̈ʒu�����߂� ---
		CalcPositionFromAngle(playerPos);

		// --- ���̈ʒu��{������ׂ��ʒu�֕⊮�������� ---
		currentPosition = Vector3::Lerp(currentPosition, position, t);


		// --- �J�������̍X�V ---
		CameraBase::Update(currentPosition, target, up, fov, nearZ, farZ, aspect);

		break;
	}

	}
}


void PlayerCamera::UpdateConstants()
{
	CameraBase::UpdateConstants(currentPosition);
}


void PlayerCamera::DrawDebugGui()
{
	ImGui::Begin(u8"�v���C���[�J����");

	InputManager& input = InputManager::Instance();

	ImGui::BulletText(u8"M�ŃJ�[�\���Œ��؂�ւ�");
	if (input.GetKeyPressed(DirectX::Keyboard::Keys::M))
		fixedCursor = !fixedCursor;


	ImGui::BulletText(u8"�J�[�\��");
	ImGui::Text(u8"x : %f / y : %f", input.GetCursorPosXFloat(), input.GetCursorPosYFloat());

	ImGui::BulletText(u8"�J�����̊p�x");
	ImGui::Text(u8"x : %f / y : %f", horizontalAngle, verticalAngle);

	ImGui::DragFloat(u8"���x", &sensitivity, 0.1f, 0.1f);
	ImGui::DragFloat(u8"�ڕW����̋���", &range, 0.1f, 0.1f);
	ImGui::DragFloat(u8"�ڕW�̍���", &height, 0.1f, 0.1f);

	ImGui::DragFloat3(u8"�ʒu", &position.x);
	ImGui::DragFloat3(u8"���̈ʒu", &currentPosition.x);
	ImGui::DragFloat(u8"�⊮��", &t, 0.01f);

	ImGui::DragFloat3(u8"�ڕW", &target.x);

	ImGui::End();
}


// --- �J�[�\�����Œ肵�Ă���Ƃ��ɌĂ΂�� ---
void PlayerCamera::OnFixedCursor(float deltaTime)
{	
	InputManager& input = InputManager::Instance();
	Vector2 curCursorPos;

	// --- �����ɌŒ� ---
		// Todo : �ʒu�����j�^�[�̍���̍��W�Ȃ̂łǂ��ɂ�����
	SetCursorPos(640, 360);

	// --- �J�[�\���̍��W���擾���Ē����Ƃ̍������Ƃ� ---
	curCursorPos = { input.GetCursorPosXFloat(), input.GetCursorPosYFloat() };
	const Vector2 cursorDelta = curCursorPos - Vector2{ 320.0f, 180.0f };

	// --- �c�̊p�x���N�����v ---
	verticalAngle = (std::max)(-90.0f, (std::min)(verticalAngle, 90.0f));

	// --- �J�[�\���̍��������p�x�����Z ---
	horizontalAngle += -cursorDelta.x * deltaTime * sensitivity;		// �E�����v���X�Ȃ̂ŕ����𔽓]������
	verticalAngle += cursorDelta.y * deltaTime * sensitivity * 2.0f;	// ��ʔ䗦���l������2�{

}


// --- �p�x����ʒu���v�Z ---
void PlayerCamera::CalcPositionFromAngle(const Vector3& position)
{
	float horizontalTheta = DirectX::XMConvertToRadians(horizontalAngle);
	float verticalTheta = DirectX::XMConvertToRadians(verticalAngle);
	Vector3 cameraVec = { cosf(horizontalTheta), sinf(verticalTheta), sinf(horizontalTheta) };
	this->position = position + cameraVec * range;
}



// ===== ���b�N�I���J���� ======================================================================================================================================================
void LockOnCamera::Initialize()
{
	height = 7.0f;
	range = 15.5f;
}

void LockOnCamera::Update()
{
	switch (state)
	{
	case 0:
		Initialize();
		state++;
		[[fallthrough]];

		
	case 1:
	{
		Vector3 playerPos = Player::Instance().GetPos();	// �v���C���[�̈ʒu
		Vector3 enemyPos = Enemy::Instance().GetPos();		// �ڕW�̈ʒu

		target = enemyPos;	// �ڕW

		// �G�ƃv���C���[�ƃJ�������꒼���ɕ��ׂ�
		Vector3 vec = playerPos - enemyPos;	// �G����v���C���[�ւ̃x�N�g��
		float length = vec.Length();		// �G�ƃv���C���[�̋���
		vec.Normalize();

		Vector3 offset = { 0.0f, height, 0.0f };
		position = target + offset + vec * (length + range);


		// --- ���̈ʒu��{������ׂ��ʒu�֕⊮�������� ---
		currentPosition = Vector3::Lerp(currentPosition, position, t);

		CameraBase::Update();

		break;
	}

	}
}

void LockOnCamera::UpdateConstants()
{
	CameraBase::UpdateConstants();
}

void LockOnCamera::DrawDebugGui()
{
	ImGui::Begin(u8"���b�N�I���J����");

	ImGui::DragFloat(u8"����", &height, 0.1f);
	ImGui::DragFloat(u8"����", &range, 0.1f);

	ImGui::End();
}

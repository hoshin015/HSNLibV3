#include "CameraDerived.h"

#include "../../External/ImGui/imgui.h"

#include "../../Library/Timer.h"

#include "../../Library/Input/InputManager.h"

#include "../Game/Object/StateMachine/Player/Player.h"


void PlayerCamera::Initialize()
{
	verticalAngle = 45.0f;
	horizontalAngle = 0.0f;
	range = 13.0f;
	fixedCursor = true;
	
	// --- �J�����ʒu�ƖڕW�̐ݒ� ---
	currentPosition = position = Player::Instance().GetPos();
	Vector3 playerVelocity = Vector3::Normalize(Player::Instance().GetVelocity());	// �v���C���[�̈ړ��x�N�g��
	Vector3 offset = { playerVelocity.x, 5.0f, playerVelocity.z };
	target = position + offset;

	positionComplementAmount = 0.3f;
}


void PlayerCamera::Update()
{
	float deltaTime = Timer::Instance().DeltaTime();

	Vector3 playerPos = Player::Instance().GetPos();	// �v���C���[�̍��W
	Vector3 offset = { 0.0f, 5.0f, 0.0f };
	target =  playerPos + offset;


	InputManager& input = InputManager::Instance();
	Vector2 curCursorPos;

	// --- �J�[�\�����Œ肷�邩 ---
	if (fixedCursor)
	{
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

	// --- �p�x�Ńx�N�g��������ăJ�����̈ʒu�����߂� ---
	float horizontalTheta = DirectX::XMConvertToRadians(horizontalAngle);
	float verticalTheta = DirectX::XMConvertToRadians(verticalAngle);
	Vector3 cameraVec = { cosf(horizontalTheta), sinf(verticalTheta), sinf(horizontalTheta) };
	position = playerPos + cameraVec * range;


	currentPosition = Vector3::Lerp(currentPosition, position, positionComplementAmount);


	// --- �J�������̍X�V ---
	CameraBase::Update(currentPosition, target, up, fov, nearZ, farZ, aspect);
}


void PlayerCamera::UpdateConstants()
{
	CameraBase::UpdateConstants(currentPosition);
}


void PlayerCamera::DrawDebugGui()
{
	ImGui::Begin(u8"�J����");

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

	ImGui::DragFloat3(u8"�ʒu", &position.x);
	ImGui::DragFloat3(u8"���̈ʒu", &currentPosition.x);
	ImGui::DragFloat(u8"�ʒu�̕⊮��", &positionComplementAmount, 0.01f);

	ImGui::DragFloat3(u8"�ڕW", &target.x);

	ImGui::End();
}

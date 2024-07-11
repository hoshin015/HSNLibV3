#include "CameraDerived.h"

#include "../../External/ImGui/imgui.h"

#include "../../Library/3D/CameraManager.h"

#include "../../Library/Timer.h"

#include "../../Library/Input/InputManager.h"

#include "../../Library/Math/Collision.h"

#include "../Game/Object/StateMachine/Player/Player.h"
#include "../Game/Object/StateMachine/Enemy/Enemy.h"
#include "../Game/Object/Stage/StageManager.h"


// ===== �v���C���[�̃J���� ======================================================================================================================================================
void PlayerCamera::Initialize()
{
	verticalAngle = 45.0f;
	horizontalAngle = 0.0f;

	// ����� range �ς��Ă܂�
	//range = 13.0f;
	range = 28.0f;

	height = 6.0f;
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



		Vector3 shakeOffset;
		Vector3 shakeIntensity = { 10.0f, 10.0f, 1.0f };
		if (shakeTimer > 0.0f)
		{
			shakeTimer -= deltaTime;
			shakeOffset = OnShake(shakeIntensity);
		}
		
		else
		{
			shakeTimer = 0.0f;
			shakeOffset = Vector3::Zero_;
		}


		// --- ���ɖ��܂�Ȃ��悤�ɐ��� ---
		if (currentPosition.y < 1.0f)
			currentPosition.y = 1.0f;

		if (position.y < 1.0f)
			position.y = 1.0f;

		if (verticalAngle < 0.0f)
			verticalAngle = 0.0f;


		// --- ���̕ǂƂ̃��C�L���X�g ---
		HitResult result;
		if (StageManager::Instance().RayCast(1, target.vec_, position.vec_, result))
		{
			currentPosition = result.position;
			position = result.position;
		}

		// --- �ҋ@�ꏊ ---
		result = {};
		if (StageManager::Instance().RayCast(3, target.vec_, position.vec_, result))
		{
			currentPosition = result.position;
			position = result.position;
		}


		// --- �J�������̍X�V ---
		CameraBase::Update(currentPosition + shakeOffset, target, up, fov, nearZ, farZ, aspect);

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

	if (ImGui::Button(u8"�V�F�C�N", { 100.0f, 30.0f }))
		shakeTimer = 0.3f;


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

	float atan = atan2(position.y, position.x);
	atan += DirectX::XM_PIDIV2;
	ImGui::Text("Atan : %f", DirectX::XMConvertToDegrees(atan));

	ImGui::End();
}


// --- �J������ݒ肵���Ƃ��ɌĂ΂�� ---
void PlayerCamera::OnSetCamera()
{
	// --- �^�[�Q�b�g�J�����͊p�x���g�p���Ă��Ȃ��̂ŁA�ǂ��ɂ����č��킹�� ---
	Vector3 enemyPos = Enemy::Instance().GetPos();
	Vector3 playerPos = Player::Instance().GetPos();
	Vector2 vec = enemyPos.xz() - playerPos.xz();
	float atan = atan2(vec.y, vec.x);
	atan = DirectX::XMConvertToDegrees(atan);
	atan += 180.0f; // -180.0 ~ 180.0��0.0 ~ 360.0��

	horizontalAngle = atan;
	verticalAngle = 30.0f;	// �����p�x�͑��30�x

	Vector3 offset = { 0.0f, height, 0.0f };
	float horizontalTheta = DirectX::XMConvertToRadians(horizontalAngle);
	float verticalTheta = DirectX::XMConvertToRadians(verticalAngle);
	Vector3 cameraVec = { cosf(horizontalTheta), sinf(verticalTheta), sinf(horizontalTheta) };
	position = playerPos + cameraVec * range;
}


// --- �J�[�\�����Œ肵�Ă���Ƃ��ɌĂ΂�� ---
void PlayerCamera::OnFixedCursor(float deltaTime)
{	
	InputManager& input = InputManager::Instance();
	Vector2 curCursorPos;

	// --- �����ɌŒ� ---
		// Todo : �ʒu�����j�^�[�̍���̍��W�Ȃ̂łǂ��ɂ�����
	//SetCursorPos(640, 360);
	input.SetCursorPosition(640, 360);
	
	// --- �J�[�\���̍��W���擾���Ē����Ƃ̍������Ƃ� ---
	curCursorPos = { input.GetCursorPosXFloat(), input.GetCursorPosYFloat() };
	const Vector2 cursorDelta = curCursorPos - Vector2{ 640.0f, 360.0f };

	// --- �c�̊p�x���N�����v ---
	verticalAngle = (std::max)(-90.0f, (std::min)(verticalAngle, 90.0f));

	// --- �J�[�\���̍��������p�x�����Z ---
	horizontalAngle += -cursorDelta.x * deltaTime * sensitivity;		// �E�����v���X�Ȃ̂ŕ����𔽓]������
	verticalAngle += cursorDelta.y * deltaTime * sensitivity * 2.0f;	// ��ʔ䗦���l������2�{


	// --- �p�x��360.0 ~ 0.0�̊Ԃɐ��� ---
	if (horizontalAngle > 360.0f)
		horizontalAngle -= 360.0f;

	else if (horizontalAngle < 0.0f)
		horizontalAngle += 360.0f;

}


Vector3 PlayerCamera::OnShake(const Vector3& intensity)
{
	Vector3 result;
	result = rightVec * (((rand() % static_cast<int>(shakePower)) / shakePower) - 0.5f) * intensity.x;
	result = upVec * (((rand() % static_cast<int>(shakePower)) / shakePower) - 0.5f) * intensity.y;
	result = frontVec * (((rand() % static_cast<int>(shakePower)) / shakePower) - 0.5f) * intensity.z;
	return result;
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
	range = 30.0f;
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
		prevTarget = target;
		//Vector3 enemyPos = Enemy::Instance().GetBonePosition("atama");

		// --- �O��Ƃ̈ړ��ʂ�������������X�V���Ȃ� ---
		//float l = Vector3(prevTarget - enemyPos).Length();
		//if (l < 0.005f)
		//	break;

		target = enemyPos;	// �ڕW

		// �G�ƃv���C���[�ƃJ�������꒼���ɕ��ׂ�
		Vector3 vec = playerPos - enemyPos;	// �G����v���C���[�ւ̃x�N�g��
		float length = vec.Length();		// �G�ƃv���C���[�̋���
		vec.Normalize();

		Vector3 offset = { 0.0f, height, 0.0f };
		position = target + offset + vec * (length + range);


		if (position.y < 1.0f)
			position.y = 1.0f;

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



void EnemyDeadCamera::Initialize()
{
}

void EnemyDeadCamera::Update()
{
	switch(state)
	{
	case 0:

		timer = 3.0f;

		state++;
		break;


	case 1:
	{
		Vector3 headPos = Enemy::Instance().GetBonePosition("atama");
		target = headPos;	// �ڕW

		Vector3 enemyPosition = Enemy::Instance().GetPos();
		Vector3 vec = headPos - enemyPosition;
		vec.Normalize();

		position = enemyPosition + vec * 25.0f;

		if (position.y < 1.0f)
			position.y = 1.0f;

		CameraBase::Update();


		float elapsedTime = Timer::Instance().DeltaTime();
		timer -= elapsedTime;
		if (timer < 0.0f)
		{
			CameraManager::Instance().SetCurrentCamera("PlayerCamera");
		}
		break;
	}


	}
}

void EnemyDeadCamera::UpdateConstants()
{
	CameraBase::UpdateConstants();
}

void EnemyDeadCamera::DrawDebugGui()
{
}

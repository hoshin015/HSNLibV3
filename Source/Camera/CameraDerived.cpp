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
	range = 18.0f;

	height = 6.0f;
	fixedCursor = true;
	
	// --- �J�����ʒu�ƖڕW�̐ݒ� ---
	currentPosition = position = Player::Instance().GetPos();
	Vector3 playerVelocity = Vector3::Normalize(Player::Instance().GetVelocity());	// �v���C���[�̈ړ��x�N�g��
	Vector3 offset = { playerVelocity.x, height, playerVelocity.z };
	target = position + offset;
	currentTarget = target;
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

		Player& player = Player::Instance();
		Vector3 playerPos = player.GetPos();	// �v���C���[�̍��W
		Vector3 offset = { 0.0f, height, 0.0f };
		Vector3 velocity = player.GetVelocity();
		target = playerPos + offset + Vector3::Normalize(velocity) * 2.0f/* * 500.0f * velocity.Length()*/;


		// --- �J�[�\�����Œ肷�邩 ---
		if (fixedCursor)
		{
			OnFixedCursor(deltaTime);
		}

		// --- �p�x�Ńx�N�g��������ăJ�����̈ʒu�����߂� ---
		CalcPositionFromAngle(playerPos);

		// --- ���̈ʒu��{������ׂ��ʒu�֕⊮�������� ---
		currentPosition = Vector3::Lerp(currentPosition, position, t);
		currentTarget = Vector3::Lerp(currentTarget, target, 0.1f);



		Vector3 shakeOffset;
		Vector3 shakeIntensity = { 10.0f, 10.0f, 1.0f };
		auto& manager = CameraManager::Instance();
		if (manager.shakeTimer > 0.0f)
		{
			shakeOffset = OnShake(shakeIntensity);
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

		//// --- �ҋ@�ꏊ ---
		//result = {};
		//if (StageManager::Instance().RayCast(3, target.vec_, position.vec_, result))
		//{
		//	currentPosition = result.position;
		//	position = result.position;
		//}


		// --- �J�������̍X�V ---
		CameraBase::Update(currentPosition/*position*/ + shakeOffset, currentTarget, up, fov, nearZ, farZ, aspect);

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
		CameraManager::Instance().shakeTimer = 0.3f;


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
	Vector2 cursorDelta = curCursorPos - Vector2{ 640.0f, 360.0f };

	// �R���g���[���Ή�
	if(input.IsGamePadConnected()) {
		cursorDelta = { input.GetThumSticksRightX(),-input.GetThumSticksRightY() };
		cursorDelta *= 200;
	}

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
	float shakePower = CameraManager::Instance().shakePower;
	if (static_cast<int>(shakePower) < 1) return { 0,0,0 };
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
	range = 18.0f;
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
		target.y += 3.0f;

		// �G�ƃv���C���[�ƃJ�������꒼���ɕ��ׂ�
		Vector3 vec = playerPos - enemyPos;	// �G����v���C���[�ւ̃x�N�g��
		float length = vec.Length();		// �G�ƃv���C���[�̋���
		vec.Normalize();

		Vector3 offset = { 0.0f, height, 0.0f };
		position = target + offset + vec * (length + range);


		if (position.y < 1.0f)
			position.y = 1.0f;



		// --- �J�����V�F�C�N ---
		Vector3 shakeOffset;
		Vector3 shakeIntensity = { 10.0f, 10.0f, 1.0f };
		auto& manager = CameraManager::Instance();
		if (manager.shakeTimer > 0.0f)
		{
			shakeOffset = OnShake(shakeIntensity);
		}


		// --- ���̕ǂƂ̃��C�L���X�g ---
		HitResult result;
		if (StageManager::Instance().RayCast(1, target.vec_, position.vec_, result))
		{
			currentPosition = result.position;
			position = result.position;
		}


		currentPosition = position;
		CameraBase::Update(currentPosition + shakeOffset, target, up, fov, nearZ, farZ, aspect);

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

	if (ImGui::Button(u8"�V�F�C�N", { 100.0f, 30.0f }))
		CameraManager::Instance().shakeTimer = 0.3f;

	ImGui::End();
}

Vector3 LockOnCamera::OnShake(const Vector3& intensity)
{
	Vector3 result;
	float shakePower = CameraManager::Instance().shakePower;
	if (static_cast<int>(shakePower) < 1) return { 0,0,0 };
	result = rightVec * (((rand() % static_cast<int>(shakePower)) / shakePower) - 0.5f) * intensity.x;
	result = upVec * (((rand() % static_cast<int>(shakePower)) / shakePower) - 0.5f) * intensity.y;
	result = frontVec * (((rand() % static_cast<int>(shakePower)) / shakePower) - 0.5f) * intensity.z;
	return result;
}




void EnemyDeadCamera::Initialize()
{
}

void EnemyDeadCamera::Update()
{
	auto lerp = [](const float start, const float end, const float t)
		{
			return start + (end - start) * t;
		};
	static bool enter;
	if (!enter)
	{
		{
			state1Data.time = 5.0f;
			state1Data.range = 25.0f;
			state1Data.height = 3.0f;
			state1Data.beginAngle = 0.0f;
			state1Data.endAngle = 60.0f;
		}

		{
			state2Data.time = 7.0f;
			state2Data.range = 25.0f;
			state2Data.height = 15.0f;
			state2Data.beginAngle = 0.0f;
			state2Data.endAngle = 360.0f;
		}

		{
			state3Data.time = 10.0f;
			state3Data.range = 10.0f;
			state3Data.height = 3.0f;
		}

		enter = true;
	}


	switch(state)
	{
	case 0:

		timer = state1Data.time;

		state++;
		break;


	case 1:
	{
		Vector3 headPos = Enemy::Instance().GetBonePosition("atama");
		target = headPos;	// �ڕW

		// --- �G�̈ʒu���瓪�̈ʒu�ւ̃x�N�g�����Z�o ---
		Vector3 enemyPosition = Enemy::Instance().GetPos();
		Vector3 vec = headPos - enemyPosition;
		vec.Normalize();

		// --- �x�N�g������] ---
		Quaternion rot;
		rot.SetRotationDegY(lerp(state1Data.beginAngle, state1Data.endAngle, timer / state1Data.time));
		vec = Quaternion(rot * Quaternion(vec.x, vec.y, vec.z, 0.0f) * Quaternion(-rot.x, -rot.y, -rot.z, rot.w)).xyz();

		// --- �x�N�g���ňʒu�����߂āA�������ړ������� ---
		position = enemyPosition + vec * state1Data.range;
		position.y += state1Data.height;


		float elapsedTime = Timer::Instance().DeltaTime();
		if (updateTimer)
			timer -= elapsedTime;
		if (timer < 0.0f)
		{
			timer = state2Data.time;
			state++;
		}
		break;
	}


	case 2:
	{
		Vector3 headPos = Enemy::Instance().GetBonePosition("atama");
		target = headPos;	// �ڕW

		Vector3 vec = Vector3::Front_;

		// --- �x�N�g������] ---
		Quaternion rot;
		rot.SetRotationDegY(lerp(state2Data.beginAngle, state2Data.endAngle, timer / state2Data.time));
		vec = Quaternion(rot * Quaternion(vec.x, vec.y, vec.z, 0.0f) * Quaternion(-rot.x, -rot.y, -rot.z, rot.w)).xyz();

		// --- �x�N�g���ňʒu�����߂āA�������ړ������� ---
		position = headPos + vec * state2Data.range;
		position.y += state2Data.height;


		float elapsedTime = Timer::Instance().DeltaTime();
		if (updateTimer)
			timer -= elapsedTime;
		if (timer < 0.0f)
		{
			timer = state3Data.time;
			state3Data.range = 10.0f;
			state3Data.height = 3.0f;
			state++;
		}

		break;
	}


	case 3:
	{
		// --- �^�񒆂ւ̃x�N�g�� ---
		Vector3 vec = Vector3::Zero_ - Enemy::Instance().GetPos();
		vec.Normalize();

		//// --- �x�N�g������] ---
		//Quaternion rot;
		//rot.SetRotationDegY(lerp(state2Data.beginAngle, state2Data.endAngle, timer / state2Data.time));
		//vec = Quaternion(rot * Quaternion(vec.x, vec.y, vec.z, 0.0f) * Quaternion(-rot.x, -rot.y, -rot.z, rot.w)).xyz();

		// --- �x�N�g���ňʒu�����߂āA�������ړ������� ---
		position = Vector3::Zero_ + vec * state3Data.range;
		position.y += state3Data.height;

		float elapsedTime = Timer::Instance().DeltaTime();
		state3Data.range += elapsedTime;
		state3Data.height += elapsedTime * 3.0f;


		if (updateTimer)
			timer -= elapsedTime;
		if (timer < 0.0f)
		{
			CameraManager::Instance().SetCurrentCamera("PlayerCamera");
			state = 0;
		}

		break;
	}

	}

	if (position.y < 1.0f)
		position.y = 1.0f;

	CameraBase::Update();
}

void EnemyDeadCamera::UpdateConstants()
{
	CameraBase::UpdateConstants();
}

void EnemyDeadCamera::DrawDebugGui()
{
	ImGui::Begin(u8"�G�̎��S�J����");

	ImGui::InputInt(u8"�X�e�[�g", &state);
	ImGui::DragFloat(u8"�^�C�}�[", &timer, 0.1f);
	ImGui::Checkbox(u8"�^�C�}�[�X�V", &updateTimer);
	if (ImGui::Button(u8"���Z�b�g", { 200.0f, 30.0f }))
	{
		state = 0;
	}
	if (ImGui::TreeNode(u8"�X�e�[�g1"))
	{
		ImGui::BulletText(u8"�߂��Ŋ�̎������]");
		ImGui::DragFloat(u8"����", &state1Data.time, 0.1f);
		ImGui::DragFloat(u8"����", &state1Data.range);
		ImGui::DragFloat(u8"����", &state1Data.height);
		ImGui::DragFloat(u8"�J�n�p�x", &state1Data.beginAngle);
		ImGui::DragFloat(u8"�I���p�x", &state1Data.endAngle);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode(u8"�X�e�[�g2"))
	{
		ImGui::BulletText(u8"������Ɨ���đS�̂��邮��");
		ImGui::DragFloat(u8"����", &state2Data.time, 0.1f);
		ImGui::DragFloat(u8"����", &state2Data.range);
		ImGui::DragFloat(u8"����", &state2Data.height);
		ImGui::DragFloat(u8"�J�n�p�x", &state2Data.beginAngle);
		ImGui::DragFloat(u8"�I���p�x", &state2Data.endAngle);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode(u8"�X�e�[�g3"))
	{
		ImGui::BulletText(u8"��ʂ��񂾂�����Ă���");
		ImGui::DragFloat(u8"����", &state3Data.time, 0.1f);
		ImGui::DragFloat(u8"����", &state3Data.range);
		ImGui::DragFloat(u8"����", &state3Data.height);
		ImGui::TreePop();
	}


	ImGui::End();
}



// ===== �v���C���[�̎��S���̃J���� ======================================================================================================================================================
void PlayerDeadCamera::Initialize()
{
	timer = time;
}

void PlayerDeadCamera::Update()
{
	auto lerp = [](const float start, const float end, const float t)
		{
			return start + (end - start) * t;
		};

	switch (state)
	{
	case 0:
		Initialize();
		state++;
		break;


	case 1:
	{
		Quaternion rot;
		rot.SetRotationDegY(lerp(0.0f, 360.0f, timer / time));

		Matrix R;
		R.MakeRotation(Player::Instance().GetAngle());
		Vector3 vec = R.v_[2].xyz();
		vec = Quaternion(rot * Quaternion(vec.x, vec.y, vec.z, 0.0f) * Quaternion(-rot.x, -rot.y, -rot.z, rot.w)).xyz();

		target = Player::Instance().GetBonePosition("pelvis");
		position = target + vec * range;
		position.y += height;

		float elapsedTime = Timer::Instance().DeltaTime();
		timer -= elapsedTime;
		if (timer < 0.0f)
		{
			state = 0;
			CameraManager::Instance().SetCurrentCamera("PlayerCamera");
		}

		break;
	}

	}


	if (position.y < 1.0f)
		position.y = 1.0f;

	CameraBase::Update();
}

void PlayerDeadCamera::UpdateConstants()
{
	CameraBase::UpdateConstants();
}

void PlayerDeadCamera::DrawDebugGui()
{
	ImGui::DragFloat(u8"����", &range);
	ImGui::DragFloat(u8"����", &height);
}

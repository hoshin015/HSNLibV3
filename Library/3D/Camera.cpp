#include "Camera.h"
#include "../../External/ImGui/imgui.h"
#include "../Graphics/Graphics.h"
#include "../Input/InputManager.h"
#include "../Timer.h"
#include "../ImGui/ConsoleData.h"
#include "../ErrorLogger.h"
#include "../RegisterNum.h"


Camera::Camera()
{
	// --- Graphics �擾 ---
	Graphics* gfx = &Graphics::Instance();
	ID3D11Device* device = gfx->GetDevice();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	// --- �v���W�F�N�V�������W�ϊ��s��쐬 ---
	D3D11_VIEWPORT viewport;
	UINT numViewports{ 1 };
	dc->RSGetViewports(&numViewports, &viewport);

	float aspectRatio{ viewport.Width / viewport.Height };
	SetPerspectiveFov(XMConvertToRadians(fovY), aspectRatio, nearZ, farZ);

	// --- �r���[���W�ϊ��s��쐬 ---
	SetLookAt(
		DirectX::XMFLOAT3(0, 5, 20),		// �J�������W
		DirectX::XMFLOAT3(-30, 0, -30),		// �^�[�Q�b�g(�ݒ肵�Ă��Ӗ��Ȃ�)
		DirectX::XMFLOAT3(0, 1, 0)			// ������x�N�g��
	);

	// --- angle �ݒ� ---
	angle = { DirectX::XMConvertToRadians(15),DirectX::XMConvertToRadians(180), 0 };
}

void Camera::Update()
{
	// --- ���͏��� ---
	InputManager& inputManager = InputManager::Instance();
	inputManager.ResetScrollWheelValue();
	
	switch (cameraType)
	{
	case CAMERA::TARGET_PLAYER:
		TargetCameraUpdate();
		break;
	case CAMERA::FREE:
		FreeCameraUpdate();
		break;
	case CAMERA::MODEL_EDITOR:
		ModelEditorCameraUpdate();
		break;
	case CAMERA::LOCK:
		LockCameraUpdate();
		break;
	}
}

void Camera::FreeCameraUpdate()
{
	// --- ���͏��� ---
	InputManager& inputManager = InputManager::Instance();

	float ax = 0;
	float ay = 0;
	float az = 0;

	difX = oldCursorX - inputManager.GetCursorPosX();
	difY = oldCursorY - inputManager.GetCursorPosY();
	oldCursorX = inputManager.GetCursorPosX();
	oldCursorY = inputManager.GetCursorPosY();

	if (inputManager.GetMousePress(MOUSEBUTTON_STATE::rightButton))
	{
		angle.y -= difX * Timer::Instance().DeltaTime();
		angle.x -= difY * Timer::Instance().DeltaTime();
	}
	if (inputManager.GetMousePress(MOUSEBUTTON_STATE::middleButton))
	{
		ax += difX * Timer::Instance().DeltaTime() * 10;
		az -= difY * Timer::Instance().DeltaTime() * 10;
	}
	ay += inputManager.GetScrollWheelValue() * Timer::Instance().DeltaTime() * 10;
	inputManager.ResetScrollWheelValue();

	const int keyAngleSpeed = 1;
	if (inputManager.GetKeyPress(Keyboard::J))
	{
		angle.y -= keyAngleSpeed * Timer::Instance().DeltaTime();
	}
	if (inputManager.GetKeyPress(Keyboard::L))
	{
		angle.y += keyAngleSpeed * Timer::Instance().DeltaTime();
	}
	if (inputManager.GetKeyPress(Keyboard::I))
	{
		angle.x -= keyAngleSpeed * Timer::Instance().DeltaTime();
	}
	if (inputManager.GetKeyPress(Keyboard::K))
	{
		angle.x += keyAngleSpeed * Timer::Instance().DeltaTime();
	}


	const int keyMoveSpeed = 5;
	if (inputManager.GetKeyPress(Keyboard::W))
	{
		ay += keyMoveSpeed * Timer::Instance().DeltaTime();
	}

	if (inputManager.GetKeyPress(Keyboard::S))
	{
		ay -= keyMoveSpeed * Timer::Instance().DeltaTime();
	}
	if (inputManager.GetKeyPress(Keyboard::D))
	{
		ax += keyMoveSpeed * Timer::Instance().DeltaTime();
	}
	if (inputManager.GetKeyPress(Keyboard::A))
	{
		ax -= keyMoveSpeed * Timer::Instance().DeltaTime();
	}
	if (inputManager.GetKeyPress(Keyboard::Q))
	{
		az += keyMoveSpeed * Timer::Instance().DeltaTime();
	}
	if (inputManager.GetKeyPress(Keyboard::Z))
	{
		az -= keyMoveSpeed * Timer::Instance().DeltaTime();
	}
	

	// X���̉�]�l��-3.14�`3.14�Ɏ��܂�悤�ɂ���
	if (angle.x < -DirectX::XM_PI)
	{
		angle.x += DirectX::XM_2PI;
	}
	if (angle.x > DirectX::XM_PI)
	{
		angle.x -= DirectX::XM_2PI;
	}
	// Y���̉�]�l��-3.14�`3.14�Ɏ��܂�悤�ɂ���
	if (angle.y < -DirectX::XM_PI)
	{
		angle.y += DirectX::XM_2PI;
	}
	if (angle.y > DirectX::XM_PI)
	{
		angle.y -= DirectX::XM_2PI;
	}


	// --- �^�[�Q�b�g�̍��W���J�����̉�]���狁�߂�@---

	// �J������]�l����]�s��ɕϊ�
	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	// ��]�s�񂩂�O�����x�N�g�������o��
	DirectX::XMVECTOR Front = Transform.r[2];
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, Front);



	// �J�������W����O�x�N�g�������Ɉ�苗�����ꂽ�����_�����߂�
	focus.x = eye.x + front.x * 10;
	focus.y = eye.y + front.y * 10;
	focus.z = eye.z + front.z * 10;

	// --- �J�����̈ړ��x�N�g���v�Z ---

	// �J�����̕������擾
	DirectX::XMFLOAT3& cameraRight = right;
	DirectX::XMFLOAT3& cameraFront = front;
	DirectX::XMFLOAT3& cameraUp = up;

	// �J�����E�����x�N�g����P�ʃx�N�g���ɕϊ�
	DirectX::XMVECTOR cameraVectorRight = DirectX::XMLoadFloat3(&cameraRight);
	DirectX::XMVector3Normalize(cameraVectorRight);
	DirectX::XMStoreFloat3(&cameraRight, cameraVectorRight);

	// �J�����O�����x�N�g����P�ʃx�N�g���ɕϊ�
	DirectX::XMVECTOR cameraVectorFront = DirectX::XMLoadFloat3(&cameraFront);
	DirectX::XMVector3Normalize(cameraVectorFront);
	DirectX::XMStoreFloat3(&cameraFront, cameraVectorFront);

	// �J����������x�N�g����P�ʃx�N�g���ɕϊ�
	DirectX::XMVECTOR cameraVectorUp = DirectX::XMLoadFloat3(&cameraUp);
	DirectX::XMVector3Normalize(cameraVectorUp);
	DirectX::XMStoreFloat3(&cameraUp, cameraVectorUp);

	// �i�s�x�N�g�����v�Z����
	DirectX::XMFLOAT3 vec;
	vec.x = ax * cameraRight.x + ay * cameraFront.x + az * cameraUp.x;
	vec.y = ax * cameraRight.y + ay * cameraFront.y + az * cameraUp.y;
	vec.z = ax * cameraRight.z + ay * cameraFront.z + az * cameraUp.z;

	// --- �^�[�Q�b�g�ƃJ�����𓯂����������� ---
	focus.x += vec.x;
	focus.y += vec.y;
	focus.z += vec.z;

	eye.x += vec.x;
	eye.y += vec.y;
	eye.z += vec.z;

	// --- view �ݒ� ---
	SetLookAt(eye, focus, DirectX::XMFLOAT3(0, 1, 0));
}

void Camera::TargetCameraUpdate()
{
	// --- ���͏��� ---
	InputManager& inputManager = InputManager::Instance();

	float ax = 0;
	float ay = 0;

	if (inputManager.GetKeyPress(DirectX::Keyboard::J)) ax--;
	if (inputManager.GetKeyPress(DirectX::Keyboard::L)) ax++;
	if (inputManager.GetKeyPress(DirectX::Keyboard::I)) ay--;
	if (inputManager.GetKeyPress(DirectX::Keyboard::K)) ay++;

	difX = oldCursorX - inputManager.GetCursorPosX();
	difY = oldCursorY - inputManager.GetCursorPosY();
	oldCursorX = inputManager.GetCursorPosX();
	oldCursorY = inputManager.GetCursorPosY();

	if (inputManager.GetMousePress(MOUSEBUTTON_STATE::rightButton))
	{
		angle.y -= difX * Timer::Instance().DeltaTime() * 3;
		angle.x -= difY * Timer::Instance().DeltaTime() * 3;
	}

	// �X�e�B�b�N���͒l�ɍ��킹��X����Y������]
	angle.x += ay * 2 * Timer::Instance().DeltaTime();
	angle.y += ax * 2 * Timer::Instance().DeltaTime();

	// X���̃J������]�𐧌�
	if (angle.x > maxAngleX) angle.x = maxAngleX;
	if (angle.x < minAngleX) angle.x = minAngleX;

	// Y���̉�]�l��-3.14�`3.14�Ɏ��܂�悤�ɂ���
	if (angle.y < -DirectX::XM_PI)
	{
		angle.y += DirectX::XM_2PI;
	}
	if (angle.y > DirectX::XM_PI)
	{
		angle.y -= DirectX::XM_2PI;
	}

	// �J������]�l����]�s��ɕϊ�
	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	// ��]�s�񂩂�O�����x�N�g�������o��
	DirectX::XMVECTOR Front = Transform.r[2];
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, Front);

	DirectX::XMVECTOR Right = Transform.r[0];
	DirectX::XMFLOAT3 right;
	DirectX::XMStoreFloat3(&right, Right);

	// �����_������x�N�g�������Ɉ�苗�����ꂽ�J�������_�����߂�
	DirectX::XMFLOAT3 eye;
	eye.x = target.x - front.x * 25;
	eye.y = target.y - front.y * 25;
	eye.z = target.z - front.z * 25;

	// �J�����̎��_�ƒ����_��ݒ�
	SetLookAt(eye, target, DirectX::XMFLOAT3(0, 1, 0));


}

void Camera::ModelEditorCameraUpdate()
{
	// --- ���͏��� ---
	InputManager& inputManager = InputManager::Instance();

	difX = oldCursorX - inputManager.GetCursorPosX();
	difY = oldCursorY - inputManager.GetCursorPosY();
	oldCursorX = inputManager.GetCursorPosX();
	oldCursorY = inputManager.GetCursorPosY();

	if (inputManager.GetMousePress(MOUSEBUTTON_STATE::rightButton))
	{
		angle.y -= difX * Timer::Instance().DeltaTime() * 3;
		angle.x += difY * Timer::Instance().DeltaTime() * 3;
	}
	static float distance = 20;
	distance -= inputManager.GetScrollWheelValue() * Timer::Instance().DeltaTime() * 10;

	inputManager.ResetScrollWheelValue();

	// X���̃J������]�𐧌�
	if (angle.x > limitAngleX) angle.x = limitAngleX;
	if (angle.x < -limitAngleX) angle.x = -limitAngleX;

	// Y���̉�]�l��-3.14�`3.14�Ɏ��܂�悤�ɂ���
	if (angle.y < -DirectX::XM_PI)
	{
		angle.y += DirectX::XM_2PI;
	}
	if (angle.y > DirectX::XM_PI)
	{
		angle.y -= DirectX::XM_2PI;
	}

	// �J������]�l����]�s��ɕϊ�
	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	// ��]�s�񂩂�O�����x�N�g�������o��
	DirectX::XMVECTOR Front = Transform.r[2];
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, Front);

	// �����_������x�N�g�������Ɉ�苗�����ꂽ�J�������_�����߂�
	eye.x = target.x + front.x * distance;
	eye.y = target.y + front.y * distance;
	eye.z = target.z + front.z * distance;

	// �J�����̎��_�ƒ����_��ݒ�
	SetLookAt(eye, target, DirectX::XMFLOAT3(0, 1, 0));
}

void Camera::LockCameraUpdate()
{
	// --- �^�[�Q�b�g�̍��W���J�����̉�]���狁�߂�@---

	// �J������]�l����]�s��ɕϊ�
	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	// ��]�s�񂩂�O�����x�N�g�������o��
	DirectX::XMVECTOR Front = Transform.r[2];
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, Front);

	// �J�������W����O�x�N�g�������Ɉ�苗�����ꂽ�����_�����߂�
	focus.x = eye.x + front.x * 10;
	focus.y = eye.y + front.y * 10;
	focus.z = eye.z + front.z * 10;

	// --- �J�����̈ړ��x�N�g���v�Z ---

	// �J�����̕������擾
	DirectX::XMFLOAT3& cameraRight = right;
	DirectX::XMFLOAT3& cameraFront = front;
	DirectX::XMFLOAT3& cameraUp = up;

	// �J�����E�����x�N�g����P�ʃx�N�g���ɕϊ�
	DirectX::XMVECTOR cameraVectorRight = DirectX::XMLoadFloat3(&cameraRight);
	DirectX::XMVector3Normalize(cameraVectorRight);
	DirectX::XMStoreFloat3(&cameraRight, cameraVectorRight);

	// �J�����O�����x�N�g����P�ʃx�N�g���ɕϊ�
	DirectX::XMVECTOR cameraVectorFront = DirectX::XMLoadFloat3(&cameraFront);
	DirectX::XMVector3Normalize(cameraVectorFront);
	DirectX::XMStoreFloat3(&cameraFront, cameraVectorFront);

	// �J����������x�N�g����P�ʃx�N�g���ɕϊ�
	DirectX::XMVECTOR cameraVectorUp = DirectX::XMLoadFloat3(&cameraUp);
	DirectX::XMVector3Normalize(cameraVectorUp);
	DirectX::XMStoreFloat3(&cameraUp, cameraVectorUp);

	// --- view �ݒ� ---
	SetLookAt(eye, focus, DirectX::XMFLOAT3(0, 1, 0));
}

void Camera::ModelEidtorUpdate()
{
}

// view �ݒ�
void Camera::SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
{
	// ���_�A�����_�A���������r���[�s����쐬
	DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
	DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	DirectX::XMStoreFloat4x4(&view, View);

	// �r���[���t�s�񉻂��A���[���h�s��ɖ߂�
	DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, View);
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, World);

	// �J�����̕��������o��
	this->right.x = world.m[0][0];
	this->right.y = world.m[0][1];
	this->right.z = world.m[0][2];

	this->up.x = world.m[1][0];
	this->up.y = world.m[1][1];
	this->up.z = world.m[1][2];

	this->front.x = world.m[2][0];
	this->front.y = world.m[2][1];
	this->front.z = world.m[2][2];

	// ���_�A�����_��ۑ�
	this->eye = eye;
	this->focus = focus;
}

// projection �ݒ�
void Camera::SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{
	DirectX::XMMATRIX Projection = XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
	DirectX::XMStoreFloat4x4(&projection, Projection);

	// �p�����[�^�̕ۑ�
	this->fovY = fovY;
	this->aspect = aspect;
	this->nearZ = nearZ;
	this->farZ = farZ;
}

void Camera::DrawDebugGui()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Camera", nullptr, ImGuiWindowFlags_None))
	{
		// --- Rasterizer ---
		{
			const char* cameraTypeName[] = { "TARGET_PLAYER", "FREE", "ANIMATION_EDITOR" };
			if (ImGui::BeginCombo("CameraType", cameraTypeName[cameraType]))
			{
				for (int i = 0; i < IM_ARRAYSIZE(cameraTypeName); i++)
				{
					const bool isSelected = (cameraType == i);
					if (ImGui::Selectable(cameraTypeName[i], isSelected))
					{
						cameraType = i;
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
		}

		// eye reset
		if (ImGui::Button("RESET EYE"))
		{
			eye = { 0,5,20 };
		}

		// �ʒu
		ImGui::DragFloat3("Eye", &eye.x, 0.1f);
		// focus
		ImGui::DragFloat3("Focus", &focus.x, 0.1f);
		// ��]
		DirectX::XMFLOAT3 a;
		a.x = DirectX::XMConvertToDegrees(angle.x);
		a.y = DirectX::XMConvertToDegrees(angle.y);
		a.z = DirectX::XMConvertToDegrees(angle.z);
		ImGui::DragFloat3("Angle", &a.x);
		angle.x = DirectX::XMConvertToRadians(a.x);
		angle.y = DirectX::XMConvertToRadians(a.y);
		angle.z = DirectX::XMConvertToRadians(a.z);
		// target
		ImGui::InputFloat3("Target", &target.x);
	}
	ImGui::End();
}

// �萔�o�b�t�@�̍X�V
void Camera::UpdateCameraConstant()
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	RenderContext* rc = &RenderContext::Instance();

	DirectX::XMMATRIX ViewProjection = DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMStoreFloat4x4(&rc->cameraConstant.viewProjection, ViewProjection);

	rc->cameraConstant.cameraPosition = { eye.x, eye.y, eye.z, 0 };

	dc->UpdateSubresource(rc->cameraConstantBuffer.Get(), 0, 0, &rc->cameraConstant, 0, 0);
	dc->VSSetConstantBuffers(_cameraConstant, 1, rc->cameraConstantBuffer.GetAddressOf());
	dc->PSSetConstantBuffers(_cameraConstant, 1, rc->cameraConstantBuffer.GetAddressOf());
	dc->GSSetConstantBuffers(_cameraConstant, 1, rc->cameraConstantBuffer.GetAddressOf());
	//dc->CSSetConstantBuffers(_cameraConstant, 1, rc->cameraConstantBuffer.GetAddressOf());
}

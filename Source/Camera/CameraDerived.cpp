#include "CameraDerived.h"

#include "../../Library/Timer.h"

#include "../../Library/Input/InputManager.h"

#include "../Game/Object/StateMachine/Player/Player.h"


void PlayerCamera::Update()
{
	Vector3 playerPos = Player::Instance().GetPos();
	Vector3 offset = { 0.0f, 5.0f, 0.0f };
	target =  playerPos + offset;

	const float range = 10.0f;
	const float height = 10.0f;


	InputManager& input = InputManager::Instance();
	static Vector2 prevCursorPos;
	static Vector2 curCursorPos;
	
	curCursorPos = { input.GetCursorPosXFloat(), input.GetCursorPosYFloat() };
	const Vector2 cursorDelta = curCursorPos - prevCursorPos;
	prevCursorPos = curCursorPos;


	static float horizontalAngle = 0.0f;
	static float verticalAngle = 0.0f;
	horizontalAngle += cursorDelta.x * Timer::Instance().DeltaTime() * 3.0f;
	verticalAngle += cursorDelta.y * Timer::Instance().DeltaTime() * 0.2f;

	float horizontalTheta = DirectX::XMConvertToRadians(horizontalAngle);
	float verticalTheta = DirectX::XMConvertToRadians(verticalAngle);
	Vector3 cameraVec = { cosf(horizontalTheta), sinf(verticalAngle), sinf(horizontalTheta)};
	position = playerPos + cameraVec * range;

	CameraBase::Update();
}

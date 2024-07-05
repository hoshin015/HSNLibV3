#include "InputManager.h"

// ����������
void InputManager::Initialize(HWND hwnd)
{
	pHwnd = &hwnd;

	// --- �L�[�{�[�h ---
	keyboard = std::make_unique<DirectX::Keyboard>();

	// --- �}�E�X ---
	mouse = std::make_unique<DirectX::Mouse>();
	mouseButtonStates[static_cast<size_t>(MOUSEBUTTON_STATE::leftButton)]		= &mouseTracker.leftButton;
	mouseButtonStates[static_cast<size_t>(MOUSEBUTTON_STATE::middleButton)]		= &mouseTracker.middleButton;
	mouseButtonStates[static_cast<size_t>(MOUSEBUTTON_STATE::rightButton)]		= &mouseTracker.rightButton;
	mouseButtonStates[static_cast<size_t>(MOUSEBUTTON_STATE::xButton1)]			= &mouseTracker.xButton1;
	mouseButtonStates[static_cast<size_t>(MOUSEBUTTON_STATE::xButton2)]			= &mouseTracker.xButton2;
	SetMousePositionMode(DirectX::Mouse::MODE_ABSOLUTE);
	mouse->SetWindow(hwnd);

	// --- �Q�[���p�b�h ---
	gamepad = std::make_unique<DirectX::GamePad>();
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::a)]				= &gamepadTracker.a;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::b)]				= &gamepadTracker.b;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::x)]				= &gamepadTracker.x;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::y)]				= &gamepadTracker.y;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::leftStick)]		= &gamepadTracker.leftStick;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::rightStick)]		= &gamepadTracker.rightStick;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::leftShoulder)]		= &gamepadTracker.leftShoulder;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::rightShoulder)]	= &gamepadTracker.rightShoulder;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::back)]				= &gamepadTracker.back;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::view)]				= &gamepadTracker.view;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::start)]			= &gamepadTracker.start;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::menu)]				= &gamepadTracker.menu;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::dpadUp)]			= &gamepadTracker.dpadUp;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::dpadDown)]			= &gamepadTracker.dpadDown;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::dpadLeft)]			= &gamepadTracker.dpadLeft;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::dpadRight)]		= &gamepadTracker.dpadRight;
}

// �X�V����
void InputManager::Update()
{
	// --- �L�[�{�[�h ---
	// �L�[�{�[�h�̏�Ԏ擾
	keyState = keyboard->GetState();
	// �L�[�{�[�h��ԃg���b�J�[�X�V
	keyTracker.Update(keyState);

	// --- �}�E�X ---
	// �}�E�X�̏�Ԏ擾
	mouseState = mouse->GetState();
	// �}�E�X��ԃg���b�J�[�X�V
	mouseTracker.Update(mouseState);
	
	// --- �Q�[���p�b�h ---
	// �Q�[���p�b�h�̏�Ԏ擾
	gamepadState = gamepad->GetState(0);
	// �Q�[���p�b�h��ԃg���b�J�[�X�V
	gamepadTracker.Update(gamepadState);
}

// --- �L�[�{�[�h ---
bool InputManager::GetKeyPress(DirectX::Keyboard::Keys key)
{
	return keyState.IsKeyDown(key);
}

bool InputManager::GetKeyPressed(DirectX::Keyboard::Keys key)
{
	return keyTracker.IsKeyPressed(key);
}

bool InputManager::GetKeyReleased(DirectX::Keyboard::Keys key)
{
	return keyTracker.IsKeyReleased(key);
}




// ---	�}�E�X ---
void InputManager::ResetScrollWheelValue()
{
	mouse->ResetScrollWheelValue();
}

bool InputManager::GetMousePress(MOUSEBUTTON_STATE button)
{
	return (*mouseButtonStates[static_cast<size_t>(button)] == DirectX::Mouse::ButtonStateTracker::HELD);
}

bool InputManager::GetMousePressed(MOUSEBUTTON_STATE button)
{
	return (*mouseButtonStates[static_cast<size_t>(button)] == DirectX::Mouse::ButtonStateTracker::PRESSED);
}

bool InputManager::GetMouseReleased(MOUSEBUTTON_STATE button)
{
	return (*mouseButtonStates[static_cast<size_t>(button)] == DirectX::Mouse::ButtonStateTracker::RELEASED);
}

// �}�E�X�J�[�\���̕\���E��\���ݒ�
void InputManager::SetMouseVisible(bool visible)
{
	mouse->SetVisible(visible);
}

// �}�E�X�̍��W���Έʒu�����Έʒu���ݒ�
void InputManager::SetMousePositionMode(DirectX::Mouse::Mode mode)
{
	mouse->SetMode(mode);
}

// �}�E�X�̍��W�ݒ�(Window��)
void InputManager::SetCursorPos(int x, int y)
{
	POINT p = { x,y };
	ClientToScreen(*pHwnd, &p);
	::SetCursorPos(p.x, p.y);
}

// ---	�Q�[���p�b�h ---
bool InputManager::IsGamePadConnected()
{
	return gamepadState.IsConnected();
}

bool InputManager::GetGamePadButtonPress(GAMEPADBUTTON_STATE button)
{
	return (*gamepadButtonStates[static_cast<size_t>(button)] == DirectX::GamePad::ButtonStateTracker::HELD);
}

bool InputManager::GetGamePadButtonPressed(GAMEPADBUTTON_STATE button)
{
	return (*gamepadButtonStates[static_cast<size_t>(button)] == DirectX::GamePad::ButtonStateTracker::PRESSED);
}

bool InputManager::GetGamePadButtonReleased(GAMEPADBUTTON_STATE button)
{
	return (*gamepadButtonStates[static_cast<size_t>(button)] == DirectX::GamePad::ButtonStateTracker::RELEASED);

}

void InputManager::SetVibration(int index, float leftPower, float rightPower)
{
	gamepad->SetVibration(index, leftPower, rightPower);
}


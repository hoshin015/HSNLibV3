#pragma once

#include <Windows.h>
#include <memory>
#include <Keyboard.h>
#include <Mouse.h>
#include <GamePad.h>

enum class MOUSEBUTTON_STATE
{
	leftButton,
	middleButton,
	rightButton,
	xButton1,
	xButton2,

	MOUSEBUTTON_STATE_NUM,
};

enum class GAMEPADBUTTON_STATE
{
	a,
	b,
	x,
	y,
	leftStick,
	rightStick,
	leftShoulder,
	rightShoulder,

	back,
	view,
	start,
	menu,

	dpadUp,
	dpadDown,
	dpadLeft,
	dpadRight,

	GAMEPADBUTTON_STATE_NUM,
};

class InputManager
{
private:
	InputManager() {}
	~InputManager() {}

public:
	static InputManager& Instance()
	{
		static InputManager instance;
		return instance;
	}

private:
	HWND* pHwnd;

	// --- �L�[�{�[�h ---
	std::unique_ptr<DirectX::Keyboard> keyboard = nullptr;
	DirectX::Keyboard::State keyState = {};
	DirectX::Keyboard::KeyboardStateTracker keyTracker;
	// --- �}�E�X ---
	std::unique_ptr<DirectX::Mouse> mouse = nullptr;
	DirectX::Mouse::State mouseState = {};
	DirectX::Mouse::ButtonStateTracker mouseTracker;
	DirectX::Mouse::ButtonStateTracker::ButtonState* mouseButtonStates[static_cast<size_t>(MOUSEBUTTON_STATE::MOUSEBUTTON_STATE_NUM)];
	// --- �Q�[���p�b�h ---
	std::unique_ptr<DirectX::GamePad> gamepad;
	DirectX::GamePad::State gamepadState = {};
	DirectX::GamePad::ButtonStateTracker gamepadTracker;
	DirectX::GamePad::ButtonStateTracker::ButtonState* gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::GAMEPADBUTTON_STATE_NUM)];

public:
	// ����������
	void Initialize(HWND* hwnd);

	// �X�V����
	void Update();

	// --- �L�[�{�[�h ---
	bool GetKeyPress(DirectX::Keyboard::Keys key);		// �L�[�����������Ă���� true
	bool GetKeyPressed(DirectX::Keyboard::Keys key);	// �L�[���������� true
	bool GetKeyReleased(DirectX::Keyboard::Keys key);	// �L�[�𗣂����� true

	// --- �}�E�X ---
	// �}�E�X�̍��W�擾
	int GetCursorPosX() { return mouseState.x; }
	int GetCursorPosY() { return mouseState.y; }
	float GetCursorPosXFloat() { return static_cast<float>(mouseState.x); }
	float GetCursorPosYFloat() { return static_cast<float>(mouseState.y); }
	// �}�E�X�̍��W�ݒ�(Window��)
	void SetCursorPosition(int x, int y);


	// �}�E�X�̃z�C�[���l�擾
	int GetScrollWheelValue() { return mouseState.scrollWheelValue; }
	// �}�E�X�̃z�C�[���l���Z�b�g
	void ResetScrollWheelValue();
	// �}�E�X�{�^���̏�Ԃɂ��`�F�b�N
	bool GetMousePress(MOUSEBUTTON_STATE button);		// �}�E�X�����������Ă���� true
	bool GetMousePressed(MOUSEBUTTON_STATE button);		// �}�E�X���������� true
	bool GetMouseReleased(MOUSEBUTTON_STATE button);	// �}�E�X�𗣂����� true
	// �}�E�X�J�[�\���̕\���E��\���ݒ�
	void SetMouseVisible(bool visible);
	// �}�E�X�̍��W���Έʒu�����Έʒu���ݒ�
	void SetMousePositionMode(DirectX::Mouse::Mode mode);

	// ---	�Q�[���p�b�h ---
	// �Q�[���p�b�h�ɐڑ��ł��Ă��邩
	bool IsGamePadConnected();
	// �Q�[���p�b�h�{�^���̏�Ԃɂ��`�F�b�N
	bool GetGamePadButtonPress(GAMEPADBUTTON_STATE button);
	bool GetGamePadButtonPressed(GAMEPADBUTTON_STATE button);
	bool GetGamePadButtonReleased(GAMEPADBUTTON_STATE button);
	// L�X�e�B�b�N�̒l�擾
	float GetThumSticksLeftX() { return gamepadState.thumbSticks.leftX; }
	float GetThumSticksLeftY() { return gamepadState.thumbSticks.leftY; }
	// R�X�e�B�b�N�̒l�擾
	float GetThumSticksRightX() { return gamepadState.thumbSticks.rightX; }
	float GetThumSticksRightY() { return gamepadState.thumbSticks.rightY; }
	// �g���K�[�̒l�擾
	float GetTriggersLeft() { return gamepadState.triggers.left; }
	float GetTriggersRight() { return gamepadState.triggers.right; }
	// �o�C�u���[�V�����ݒ�
	void SetVibration(int index, float leftPower, float rightPower);
};
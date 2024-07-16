#pragma once

#include <DirectXMath.h>
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

	// --- キーボード ---
	std::unique_ptr<DirectX::Keyboard> keyboard = nullptr;
	DirectX::Keyboard::State keyState = {};
	DirectX::Keyboard::KeyboardStateTracker keyTracker;
	// --- マウス ---
	std::unique_ptr<DirectX::Mouse> mouse = nullptr;
	DirectX::Mouse::State mouseState = {};
	DirectX::Mouse::ButtonStateTracker mouseTracker;
	DirectX::Mouse::ButtonStateTracker::ButtonState* mouseButtonStates[static_cast<size_t>(MOUSEBUTTON_STATE::MOUSEBUTTON_STATE_NUM)];
	// --- ゲームパッド ---
	std::unique_ptr<DirectX::GamePad> gamepad;
	DirectX::GamePad::State gamepadState = {};
	DirectX::GamePad::ButtonStateTracker gamepadTracker;
	DirectX::GamePad::ButtonStateTracker::ButtonState* gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::GAMEPADBUTTON_STATE_NUM)];

public:
	// 初期化処理
	void Initialize(HWND* hwnd);

	// 更新処理
	void Update();

	// --- キーボード ---
	bool GetKeyPress(DirectX::Keyboard::Keys key);		// キーを押し続けている間 true
	bool GetKeyPressed(DirectX::Keyboard::Keys key);	// キーを押した時 true
	bool GetKeyReleased(DirectX::Keyboard::Keys key);	// キーを離した時 true

	// --- マウス ---
	// マウスの座標取得
	int GetCursorPosX() { return mouseState.x; }
	int GetCursorPosY() { return mouseState.y; }
	float GetCursorPosXFloat() { return static_cast<float>(mouseState.x); }
	float GetCursorPosYFloat() { return static_cast<float>(mouseState.y); }
	// マウスの座標設定(Window内)
	void SetCursorPosition(int x, int y);


	// マウスのホイール値取得
	int GetScrollWheelValue() { return mouseState.scrollWheelValue; }
	// マウスのホイール値リセット
	void ResetScrollWheelValue();
	// マウスボタンの状態によるチェック
	bool GetMousePress(MOUSEBUTTON_STATE button);		// マウスを押し続けている間 true
	bool GetMousePressed(MOUSEBUTTON_STATE button);		// マウスを押した時 true
	bool GetMouseReleased(MOUSEBUTTON_STATE button);	// マウスを離した時 true
	// マウスカーソルの表示・非表示設定
	void SetMouseVisible(bool visible);
	// マウスの座標を絶対位置か相対位置か設定
	void SetMousePositionMode(DirectX::Mouse::Mode mode);

	// ---	ゲームパッド ---
	// ゲームパッドに接続できているか
	bool IsGamePadConnected();
	// ゲームパッドボタンの状態によるチェック
	bool GetGamePadButtonPress(GAMEPADBUTTON_STATE button);
	bool GetGamePadButtonPressed(GAMEPADBUTTON_STATE button);
	bool GetGamePadButtonReleased(GAMEPADBUTTON_STATE button);
	// Lスティックの値取得
	float GetThumSticksLeftX() { return gamepadState.thumbSticks.leftX; }
	float GetThumSticksLeftY() { return gamepadState.thumbSticks.leftY; }
	// Rスティックの値取得
	float GetThumSticksRightX() { return gamepadState.thumbSticks.rightX; }
	float GetThumSticksRightY() { return gamepadState.thumbSticks.rightY; }
	// トリガーの値取得
	float GetTriggersLeft() { return gamepadState.triggers.left; }
	float GetTriggersRight() { return gamepadState.triggers.right; }
	// バイブレーション設定
	void SetVibration(int index, float leftPower, float rightPower);

	DirectX::XMFLOAT2 controllerPressed;
	void GetPressedSticks()
	{
		static int lastX = 0;
		static int lastY = 0;

		DirectX::XMFLOAT2 inputPad = { gamepadState.thumbSticks.leftX, gamepadState.thumbSticks.leftY };
		DirectX::XMVECTOR InputPad = DirectX::XMLoadFloat2(&inputPad);
		DirectX::XMVECTOR N = DirectX::XMVector2Normalize(InputPad);
		DirectX::XMFLOAT2 nInputPad;
		DirectX::XMStoreFloat2(&nInputPad, N);

		float length = DirectX::XMVectorGetX(DirectX::XMVector2Length(InputPad));
		if(length < 0.5f)
		{
			controllerPressed.x = static_cast<float>(0);
			controllerPressed.y = static_cast<float>(0);
			lastX = 0;
			lastY = 0;
			return;
		}


		// 入力の角度(radian)
		float radian = atan2(nInputPad.y, nInputPad.x);
		// degree
		float deg = DirectX::XMConvertToDegrees(radian);
		if (deg < 0) deg += 360;

		// 現在の入力
		int inputPressX = 0;
		int inputPressY = 0;
		int inputStorePressX = 0;
		int inputStorePressY = 0;
		if ((deg <=360 && deg > 315) || (deg >= 0 && deg <= 45)) inputPressX = 1;
		if (deg > 45 && deg <= 135) inputPressY = 1;
		if (deg > 135 && deg <= 225) inputPressX = -1;
		if (deg > 225 && deg <= 315) inputPressY = -1;

		inputStorePressX = inputPressX;
		inputStorePressY = inputPressY;

		if (lastX != 0) inputPressX = 0;
		if (lastY != 0) inputPressY = 0;

		lastX = inputStorePressX;
		lastY = inputStorePressY;

		controllerPressed.x = static_cast<float>(inputPressX);
		controllerPressed.y = static_cast<float>(inputPressY);
	}
};
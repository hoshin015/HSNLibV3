#pragma once
#include <wtypes.h>
#include <string>
#include <vector>

//--------------------------------------------------------------
//  Framework
//--------------------------------------------------------------
class Framework
{
private:
	Framework() {}
	~Framework() {}

public:
	static Framework& Instance()
	{
		static Framework instance;
		return instance;
	}

private:
	HWND hwnd;

	const wchar_t* windowName = L"HSNLibV2";
	const int screenWidth = 1280;
	const int screenHeight = 720;

	BOOL showCalcFrame = TRUE;

public:
	int GetScreenWidth() { return screenWidth; }
	int GetScreenHeight() { return screenHeight; }
	float GetScreenWidthF() { return static_cast<float>(screenWidth); }
	float GetScreenHeightF() { return static_cast<float>(screenHeight); }

public:
	// 初期化
	bool Initialize(HINSTANCE hInstance);
	// 更新
	void Update();
	// 終了化
	void Finalize();

private:
	// ウィンドウ作成
	void CreateHSNWindow(HINSTANCE hInstance);
	// ウィンドウプロシージャ
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	// FPS をタイトルバーにセット
	void CalculateFrame();
};
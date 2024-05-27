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
	// ������
	bool Initialize(HINSTANCE hInstance);
	// �X�V
	void Update();
	// �I����
	void Finalize();

private:
	// �E�B���h�E�쐬
	void CreateHSNWindow(HINSTANCE hInstance);
	// �E�B���h�E�v���V�[�W��
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	// FPS ���^�C�g���o�[�ɃZ�b�g
	void CalculateFrame();
};
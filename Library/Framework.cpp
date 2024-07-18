#include <sstream>
#include <dwmapi.h>
#include "Framework.h"
#include "Timer.h"
#include "Graphics/Graphics.h"
#include "ImGui/ImGuiManager.h"
#include "ImGui/ConsoleData.h"
#include "Icon/resource.h"
#include "Input/InputManager.h"
#include "Audio/AudioManager.h"
#include "Effekseer/EffectManager.h"
#include "Graphics/RenderContext.h"
#include "Effekseer/Effect.h"
#include "3D/DebugPrimitive.h"
#include "3D/LineRenderer.h"
#include "../Source/Scene/SceneManager.h"
#include "../Source/UserInterface/DamageTextManager.h"
#include "Graphics/Texture.h"

// ������
bool Framework::Initialize(HINSTANCE hInstance)
{
	// --- �E�B���h�E�쐬 ---
	CreateHSNWindow(hInstance);

	// --- Graphics������ ---
	Graphics::Instance().Initialize(hwnd, screenWidth, screenHeight);

#if USE_IMGUI
	// --- ImGui������ ---
	ImGuiManager::Instance().Initialize(hwnd);
#endif

	// --- InputManager ������ ---
	InputManager::Instance().Initialize(&hwnd);

	// --- AudioManager ������ ---
	AudioManager::Instance().Initialize();

	// --- EffectManager ������ ---
	EffectManager::Instance().Initialize();
	Effect::Instance().Initialize();

	// --- DebugPrimitive ������ ---
	DebugPrimitive::Instance().Initialize();

	// --- LineRenderer ������ ---
	LineRenderer::Instance().Initialize();

	// --- RenderContext ������ ---
	RenderContext::Instance().Initialize();

	// --- DamageTextManager ������ ---
	DamageTextManager::Instance().Initialize();

	// --- �V�[�������� ---
	SceneManager::Instance().ChangeScene(new SceneTitle);

	return true;
}

// �X�V
void Framework::Update()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG)); // ���b�Z�[�W�\���̂�������

	// --- ���C�����[�v ---
	while (msg.message != WM_QUIT)
	{
		// --- ���b�Z�[�W�̎擾 ---
		if (PeekMessage( // �߂�l�F���b�Z�[�W�̎擾���ʁi�����Ftrue�@���s:false�j
			&msg,        // ���b�Z�[�W������M����MSG�\���̂ւ̃|�C���^�[
			NULL,        // ���b�Z�[�W���擾����E�B���h�E�ւ̃n���h��
			0,           // �������郁�b�Z�[�W�͈͓̔��̍ŏ��̃��b�Z�[�W�̒l
			0,           // �������郁�b�Z�[�W�͈͓̔��̍Ō�̃��b�Z�[�W�̒l
			PM_REMOVE    // ���b�Z�[�W�̏������@�i�����ł͓������b�Z�[�W���Q��ǂݍ��܂Ȃ��悤�Ƀ��b�Z�[�W���폜���Ă���j
		))
		{
			TranslateMessage(&msg); // ���z�L�[���b�Z�[�W�𕶎����b�Z�[�W�ɕϊ�
			DispatchMessage(&msg);  // ���b�Z�[�W���E�B���h�E�v���V�[�W���ɑ��M�i�E�B���h�E�v���V�[�W�����Ăяo�������݂����ȃC���[�W���Ǝv���j
		}
		else
		// --- ���C������ ---
		{
			// ���b�Z�[�W���Ȃ��ꍇ�͂��������������

			// --- �^�C�}�[���� ---
			Timer::Instance().Tick();
			if (showCalcFrame) CalculateFrame();

			// --- �V�[���X�V ---
#if USE_IMGUI
#if SHOW_PERFORMANCE
			// --- UpdateTimer�J�n ---
			Timer::Instance().StartUpdateTime();
#endif
#endif
			SceneManager::Instance().Update();

#if USE_IMGUI
#if SHOW_PERFORMANCE
			// --- UpdateTimer�I�� ---
			Timer::Instance().StopUpdateTime();
#endif
#endif

			// --- �V�[���`�� ---
#if USE_IMGUI
#if SHOW_PERFORMANCE
			// --- RenderTimer�J�n ---
			Timer::Instance().StartRenderTime();
#endif
#endif
			SceneManager::Instance().Render();

			// --- �o�b�t�@�؂�ւ� ---
			Graphics::Instance().Present(isFullScreen);
#if USE_IMGUI
#if SHOW_PERFORMANCE
			// --- RenderTimer�I��  ---
			Timer::Instance().StopRenderTime();
#endif
#endif
		}
	}
}

// �I����
void Framework::Finalize()
{
	// --- EffectManager�I���� ---
	EffectManager::Instance().Finalize();

	// --- Texture ��� ---
	ReleaseAllTextures();

#if USE_IMGUI
	// --- ImGui�I���� ---
	ImGuiManager::Instance().Finalize();
#endif

	// --- SceneManager�I���� ---
	SceneManager::Instance().Clear();
}

// �E�B���h�E�쐬
void Framework::CreateHSNWindow(HINSTANCE hInstance)
{
	// --- �A�C�R���̓o�^ ---
	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYICON));

	// --- �E�B���h�E�N���X�̓o�^ ---
	WNDCLASSEX wc;                                         //�E�B���h�E�N���X���̍\����
	wc.cbSize        = sizeof(WNDCLASSEX);                 // �\���̂̃T�C�Y
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // �N���X�X�^�C���̑g�ݍ��킹(�E�B���h�E�̕��⍂������ύX�����ۂɍĕ`�悷�铙�H)
	wc.lpfnWndProc   = WndProc;                            // �E�B���h�E�v���V�[�W���̃A�h���X(���b�Z�[�W�����������ۂɂ��̃v���V�[�W���Ƀ��b�Z�[�W��������)
	wc.cbClsExtra    = 0;                                  // �⏕������
	wc.cbWndExtra    = 0;                                  // �⏕������
	wc.hInstance     = hInstance;                          // ���̃A�v���̃C���X�^���X�n���h��
	wc.hIcon         = hIcon;                              // �A�v���̃V���[�g�J�b�g���Ŏd�l�����A�C�R�����w��ł���
	wc.hIconSm       = hIcon;                              // �^�C�g���o�[�Ŏg�p�����A�C�R���̎w��
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);        // �E�B���h�E��̃}�E�X�J�[�\�����w��ł���
	wc.hbrBackground = NULL;                               // �E�B���h�E�̃N���C�A���g�̈�̔w�i�F
	wc.lpszMenuName  = NULL;                               // �E�B���h�E��i�ɐݒ肷�郁�j���[�̖��O�i���j���[���Ȃ����NULL�j
	wc.lpszClassName = windowName;                         // �o�^���Ɏg�p����E�B���h�E�N���X�̖��O

	RegisterClassExW(&wc); // ����OS�ɓo�^								

	// --- �E�B���h�E�T�C�Y�̒��� ---
	int  centerScreenX = GetSystemMetrics(SM_CXSCREEN) / 2 - screenWidth / 2;
	int  centerScreenY = GetSystemMetrics(SM_CYSCREEN) / 2 - screenHeight / 2;
	RECT wr;
	wr.left   = centerScreenX;
	wr.top    = centerScreenY;
	wr.right  = wr.left + screenWidth;
	wr.bottom = wr.top + screenHeight;
	AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

	// --- �E�B���h�E�̍쐬 ---
	hwnd = CreateWindowEx( // �E�B���h�E������ɍ쐬����Ȃ������ꍇ��NULL��Ԃ�
		0, // �g���E�B���h�E�X�^�C��
		windowName, // �E�B���h�E�N���X��
		windowName, // �E�B���h�E�^�C�g����
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, // �E�B���h�E�X�^�C��
		wr.left, // �E�B���h�E��X���W
		wr.top, // �E�B���h�E��Y���W
		wr.right - wr.left, // �E�B���h�E�̕�
		wr.bottom - wr.top, // �E�B���h�E�̍���
		NULL, // ���̃E�B���h�E�̐e�ւ̃n���h���i�ŏ��̃E�B���h�E�̂��ߐe�E�B���h�E�͂Ȃ��j
		NULL, // ���j���[�܂��͎q�E�B���h�E�̎��ʎq�ւ̃n���h���i���j���[���g�p�������ꍇ��NULL�ɐݒ肵�AWindowClassEx��menu���g�p���邱�Ƃ��ł���j
		hInstance, // ���̃E�B���h�E�Ŏg�p���郂�W���[���̃C���X�^���X�ւ̃n���h��
		NULL // �E�B���h�E���쐬����p�����[�^
	);

	// --- �E�B���h�E��\�� ---
	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// --- �_�[�N���[�h�L�����̐ݒ� ---
	BOOL USE_DARK_MODE                   = true;
	BOOL SET_IMMERSIVE_DARK_MODE_SUCCESS = SUCCEEDED(DwmSetWindowAttribute(
		hwnd, DWMWINDOWATTRIBUTE::DWMWA_USE_IMMERSIVE_DARK_MODE,
		&USE_DARK_MODE, sizeof(USE_DARK_MODE)));

	// --- �E�B���h�E���E���̐F�ݒ� ---
	COLORREF WINDOW_COLOR = 0xFFFFFF;
	DwmSetWindowAttribute(
		hwnd, DWMWINDOWATTRIBUTE::DWMWA_BORDER_COLOR,
		&WINDOW_COLOR, sizeof(WINDOW_COLOR));

	// --- �L���v�V�����̃J���[�ύX ---		
	COLORREF CAPTION_COLOR = 0x282828;
	DwmSetWindowAttribute(
		hwnd, DWMWINDOWATTRIBUTE::DWMWA_CAPTION_COLOR,
		&CAPTION_COLOR, sizeof(CAPTION_COLOR));

	// --- �L���v�V�����e�L�X�g�̃J���[�ύX ---
	COLORREF CAPTION_TEXT_COLOR = 0xf1f1f1;
	DwmSetWindowAttribute(
		hwnd, DWMWINDOWATTRIBUTE::DWMWA_TEXT_COLOR,
		&CAPTION_TEXT_COLOR, sizeof(CAPTION_TEXT_COLOR));
}

// �E�B���h�E�v���V�[�W��
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);

LRESULT Framework::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
#if USE_IMGUI
	if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
	{
		return true;
	}
#endif

	switch (uMsg)
	{
	// �E�B���h�E�������
	case WM_CLOSE:
		PostMessage(hwnd, WM_DESTROY, 0, 0);
		break;
	// �E�B���h�E���j�����ꂽ��
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_F4) {
			DestroyWindow(hwnd);
			return 0;
		}
		[[fallthrough]];
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		DirectX::Keyboard::ProcessMessage(uMsg, wParam, lParam);
		break;
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_XBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_XBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_MOUSEHOVER:
		DirectX::Mouse::ProcessMessage(uMsg, wParam, lParam);
		break;
	// �������郁�b�Z�[�W���Ȃ��ꍇ
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

// FPS ���^�C�g���o�[�ɃZ�b�g
void Framework::CalculateFrame()
{
	static UINT  frame     = 0;
	static float deltaTime = 0;

	frame++;

	if ((Timer::Instance().TimeStamp() - deltaTime) >= 1.0f)
	{
		float               fps = static_cast<float>(frame);
		std::wostringstream outs;
		outs.precision(6);
		outs << windowName <<
			L" : FPS : " << fps << L" - " <<
			L"Frame Time : " << 1000.0f / fps << L" (ms) - " <<
			L"Delta Time : " << Timer::Instance().DeltaTime() << L" (s)";
		SetWindowTextW(hwnd, outs.str().c_str());

		frame = 0;
		deltaTime += 1.0f;

		//ConsoleData::Instance().logs.push_back("FPS : " + std::to_string(static_cast<int>(fps)));
	}
}

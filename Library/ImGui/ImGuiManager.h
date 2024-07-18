#pragma once
#include <wtypes.h>
#include <vector>
#include <string>
#include "../../External/ImGui/imgui.h"
#include "../../External/ImGui/imgui_impl_win32.h"
#include "../../External/ImGui/imgui_impl_dx11.h"

//=======================================
#if _DEBUG
#define USE_IMGUI			0
#define SHOW_PERFORMANCE	0
#endif
//=======================================

//--------------------------------------------------------------
//  ImGuiManager
//--------------------------------------------------------------
class ImGuiManager
{
private:
	ImGuiManager() {}
	~ImGuiManager() {}

public:
	static ImGuiManager& Instance()
	{
		static ImGuiManager instance;
		return instance;
	}

	// ������
	void Initialize(HWND hwnd);
	// �I����
	void Finalize();
	// �X�V
	void Update();
	// �`��
	void Render();

	// DockSpace����
	void DockSpace();
	// Console����
	void Console();
	// DisplayPerformanceStats����
	void DisplayPerformanceStats();

	// std::string���g����ImGui::InputText�̃��b�p�[�֐�
	bool InputText(const char* label, std::string& str, ImGuiInputTextFlags flags = 0);
	// std::string���g����ImGui::InputText�̃��b�p�[�֐�
	bool InputText(std::string label, std::string& str, ImGuiInputTextFlags flags = 0);
	// DirectX::XMFLOAT4���g����ImGui::ColorPicker4�̃��b�p�[�֐�
	void SimpleColor4(std::string label, DirectX::XMFLOAT4& color);
};
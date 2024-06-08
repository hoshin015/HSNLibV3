#include "../Graphics/Graphics.h"
#include "../Timer.h"
#include "ImGuiManager.h"
#include "ConsoleData.h"
#include "../../External/ImGui/Implot/implot.h"
#include "../../External/ImGui/IconFontCppHeaders-main/IconsFontAwesome5.h"

// =====================================================================

// utility structure for realtime plot
struct ScrollingBuffer {
    int MaxSize;
    int Offset;
    ImVector<ImVec2> Data;
    ScrollingBuffer(int max_size = 4000) {
        MaxSize = max_size;
        Offset = 0;
        Data.reserve(MaxSize);
    }
    void AddPoint(float x, float y) {
        if (Data.size() < MaxSize)
            Data.push_back(ImVec2(x, y));
        else {
            Data[Offset] = ImVec2(x, y);
            Offset = (Offset + 1) % MaxSize;
        }
    }
    void Erase() {
        if (Data.size() > 0) {
            Data.shrink(0);
            Offset = 0;
        }
    }
};

// =====================================================================

// ������
void ImGuiManager::Initialize(HWND hwnd)
{
	// ImGui�̏���
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();		// Implot
	ImGuiIO& io = ImGui::GetIO();

	// Docking�̗L����
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// ��ʊO�Ή�
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	// �E�B���h�E�̐[�x�ύX�Ή�
	io.ConfigViewportsNoDefaultParent = true;

	// Font
	io.Fonts->AddFontDefault();
	io.Fonts->AddFontFromFileTTF("Data/Font/meiryob.ttc", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(Graphics::Instance().GetDevice(), Graphics::Instance().GetDeviceContext());

	// �X�^�C���̐ݒ�
	ImGui::StyleColorsDark();

}

// �I����
void ImGuiManager::Finalize()
{
	// --- ImGui�̏I�� ---
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImPlot::DestroyContext();		// Implot
	ImGui::DestroyContext();
}

// �X�V
void ImGuiManager::Update()
{
	// --- ImGui�̊J�n ---
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// --- DockSpace ���� ---
	DockSpace();
}

// �`��
void ImGuiManager::Render()
{
	// --- ImGui�`�� ---
	ImGui::Render();	// �`��f�[�^�̑g�ݗ���
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}



// DockSpace����
void ImGuiManager::DockSpace()
{
	// https://gist.github.com/moebiussurfing/8dbc7fef5964adcd29428943b78e45d2

	// --- window�t���O�ݒ� ---
	static ImGuiWindowFlags windowFlags = 
		//ImGuiWindowFlags_MenuBar |		// �����L���ɂ���ƃ��j���[�o�[��ǉ��ł���
		ImGuiDockNodeFlags_PassthruCentralNode |
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse  | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;


	// --- dockNode�t���O�ݒ� ---
	const ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_PassthruCentralNode;

	// --- ImGui�̕\���ݒ� ---
	ImGuiViewport* viewport = ImGui::GetMainViewport();				// ���C���r���[�̒l���擾
	ImGui::SetNextWindowPos(viewport->WorkPos);						// �|�W�V�����̐ݒ�
	ImGui::SetNextWindowSize(viewport->Size, ImGuiCond_Always);		// �T�C�Y�̐ݒ�
	ImGui::SetNextWindowViewport(viewport->ID);						// ID�̐ݒ�
	ImGui::SetNextWindowBgAlpha(0.0f);								// �w�i�A���t�@�̐ݒ�

	// --- DockSpace�̎��̓X�^�C���̐ݒ�? ---
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	// --- DockSpace�̍쐬 ---
	ImGui::Begin("HSNLib", NULL, windowFlags);
	ImGui::PopStyleVar(3);	// ���̓X�^�C���̓K�p?

	ImGuiID dockSpaceID = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockSpaceID, ImVec2(0.f, 0.f), dockFlags);

	ImGui::End();
}

// Console����
void ImGuiManager::Console()
{
	ImGui::Begin("Console");
	{
		if (ImGui::Button("Clear", ImVec2(60, 20)))
		{
			ConsoleData::Instance().logs.clear();
		}

		// �e�L�X�g��܂�Ԃ�
		ImGui::PushTextWrapPos();
		const float footerHeightToReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		if (ImGui::BeginChild("ScrollRegion##", ImVec2(0, -footerHeightToReserve), false, 0))
		{
			for (auto& text : ConsoleData::Instance().logs)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
				ImGui::TextUnformatted(text.c_str());
				ImGui::PopStyleColor();
			}
		}

		// ���O�������X�N���[��
		if ((ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
			ImGui::SetScrollHereY(1.0f);

		ImGui::EndChild();
	}
	ImGui::End();
}

// DisplayPerformanceStats����
void ImGuiManager::DisplayPerformanceStats()
{
#if SHOW_PERFORMANCE
	ImGui::Begin("Performance");
	{
		static ScrollingBuffer sdata1, sdata2, sdata3;
		ImVec2 mouse = ImGui::GetMousePos();
		static float t = 0;
		t += ImGui::GetIO().DeltaTime;
		sdata1.AddPoint(t, Timer::Instance().StoreDeltaTime());
		sdata2.AddPoint(t, Timer::Instance().StoreUpdateTime());
		sdata3.AddPoint(t, Timer::Instance().StoreRenderTime());

		static float history = 5.0f;
		ImGui::SliderFloat("History", &history, 1, 30, "%.1f s");

		ImGui::Text(("DeltaTime  : " + std::to_string(Timer::Instance().DeltaTime())).c_str());
		ImGui::Text(("UpdateTime : " + std::to_string(Timer::Instance().StoreUpdateTime())).c_str());
		ImGui::Text(("RenderTime : " + std::to_string(Timer::Instance().StoreRenderTime())).c_str());

		static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;

		if (ImPlot::BeginPlot("##Scrolling", ImVec2(-1, 150))) {
			ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
			ImPlot::SetupAxisLimits(ImAxis_X1, t - history, t, ImGuiCond_Always);
			ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0f, 0.08f);
			ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
			ImPlot::PlotLine("DeltaTime", &sdata1.Data[0].x, &sdata1.Data[0].y, sdata1.Data.size(), 0, sdata1.Offset, 2 * sizeof(float));
			ImPlot::PlotLine("UpdateTime", &sdata2.Data[0].x, &sdata2.Data[0].y, sdata2.Data.size(), 0, sdata2.Offset, 2 * sizeof(float));
			ImPlot::PlotLine("RenderTime", &sdata3.Data[0].x, &sdata3.Data[0].y, sdata3.Data.size(), 0, sdata3.Offset, 2 * sizeof(float));
			ImPlot::EndPlot();
		}

	}
	ImGui::End();

#endif
}

bool ImGuiManager::InputText(const char* label, std::string& str, ImGuiInputTextFlags flags)

{
	// ImGui::InputText�p�̈ꎞ�o�b�t�@������
	// �o�b�t�@�T�C�Y��str�̒���+1(�k���I�[)�ɂ���
	static std::vector<char> buffer;
	buffer.resize(str.size() + 256); // �]�T���������ă��T�C�Y

	// std::string�̓��e���o�b�t�@�ɃR�s�[
	memcpy(buffer.data(), str.c_str(), str.size() + 1);

	// ImGui::InputText���g�p
	bool result = ImGui::InputText(label, buffer.data(), buffer.size(), flags);

	// �o�b�t�@�̓��e���ύX���ꂽ�ꍇ�Astd::string�ɔ��f
	if (result)
	{
		str = buffer.data();
	}

	return result;
}

bool ImGuiManager::InputText(std::string label, std::string& str, ImGuiInputTextFlags flags)
{
	// ImGui::InputText�p�̈ꎞ�o�b�t�@������
	// �o�b�t�@�T�C�Y��str�̒���+1(�k���I�[)�ɂ���
	static std::vector<char> buffer;
	buffer.resize(str.size() + 256); // �]�T���������ă��T�C�Y

	// std::string�̓��e���o�b�t�@�ɃR�s�[
	memcpy(buffer.data(), str.c_str(), str.size() + 1);

	// ImGui::InputText���g�p
	bool result = ImGui::InputText(label.c_str(), buffer.data(), buffer.size(), flags);

	// �o�b�t�@�̓��e���ύX���ꂽ�ꍇ�Astd::string�ɔ��f
	if (result)
	{
		str = buffer.data();
	}

	return result;
}


void ImGuiManager::SimpleColor4(std::string label, DirectX::XMFLOAT4& color)
{
	ImVec4 imBoneCol = { color.x, color.y, color.z, color.w };
	// �F�̕\���p�̎l�p���u���b�N���쐬
	if (ImGui::ColorButton(("Color##" + label).c_str(), imBoneCol)) {
		// �l�p���u���b�N���N���b�N���ꂽ�Ƃ��Ƀ|�b�v�A�b�v���J��
		ImGui::OpenPopup(("ColorPicker##" + label).c_str());
	}

	// �|�b�v�A�b�v���J���Ă���Ƃ��ɃJ���[�G�f�B�^�[��\��
	if (ImGui::BeginPopup(("ColorPicker##" + label).c_str())) {
		ImGui::ColorPicker4(("##picker" + label).c_str(), (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::EndPopup();
	}
}


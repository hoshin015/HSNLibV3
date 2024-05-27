#include "../Graphics/Graphics.h"
#include "../Timer.h"
#include "ImGuiManager.h"
#include "ConsoleData.h"
#include "../../External/ImGui/Implot/implot.h"

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

// 初期化
void ImGuiManager::Initialize(HWND hwnd)
{
	// ImGuiの準備
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();		// Implot
	ImGuiIO& io = ImGui::GetIO();

	// Dockingの有効化
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// 画面外対応
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	// ウィンドウの深度変更対応
	io.ConfigViewportsNoDefaultParent = true;


	// Font
	io.Fonts->AddFontFromFileTTF("Data/Font/meiryob.ttc", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(Graphics::Instance().GetDevice(), Graphics::Instance().GetDeviceContext());
	
	// スタイルの設定
	ImGui::StyleColorsDark();

}

// 終了化
void ImGuiManager::Finalize()
{
	// --- ImGuiの終了 ---
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImPlot::DestroyContext();		// Implot
	ImGui::DestroyContext();
}

// 更新
void ImGuiManager::Update()
{
	// --- ImGuiの開始 ---
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// --- DockSpace 処理 ---
	DockSpace();
}

// 描画
void ImGuiManager::Render()
{
	// --- ImGui描画 ---
	ImGui::Render();	// 描画データの組み立て
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}



// DockSpace処理
void ImGuiManager::DockSpace()
{
	// https://gist.github.com/moebiussurfing/8dbc7fef5964adcd29428943b78e45d2

	// --- windowフラグ設定 ---
	static ImGuiWindowFlags windowFlags = 
		//ImGuiWindowFlags_MenuBar |		// これを有効にするとメニューバーを追加できる
		ImGuiDockNodeFlags_PassthruCentralNode |
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse  | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;


	// --- dockNodeフラグ設定 ---
	const ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_PassthruCentralNode;

	// --- ImGuiの表示設定 ---
	ImGuiViewport* viewport = ImGui::GetMainViewport();				// メインビューの値を取得
	ImGui::SetNextWindowPos(viewport->WorkPos);						// ポジションの設定
	ImGui::SetNextWindowSize(viewport->Size, ImGuiCond_Always);		// サイズの設定
	ImGui::SetNextWindowViewport(viewport->ID);						// IDの設定
	ImGui::SetNextWindowBgAlpha(0.0f);								// 背景アルファの設定

	// --- DockSpaceの周囲スタイルの設定? ---
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	// --- DockSpaceの作成 ---
	ImGui::Begin("HSNLib", NULL, windowFlags);
	ImGui::PopStyleVar(3);	// 周囲スタイルの適用?

	ImGuiID dockSpaceID = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockSpaceID, ImVec2(0.f, 0.f), dockFlags);

	ImGui::End();
}

// Console処理
void ImGuiManager::Console()
{
	ImGui::Begin("Console");
	{
		if (ImGui::Button("Clear", ImVec2(60, 20)))
		{
			ConsoleData::Instance().logs.clear();
		}

		// テキストを折り返す
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

		// ログを自動スクロール
		if ((ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
			ImGui::SetScrollHereY(1.0f);

		ImGui::EndChild();
	}
	ImGui::End();
}

// DisplayPerformanceStats処理
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
	// ImGui::InputText用の一時バッファを準備
	// バッファサイズをstrの長さ+1(ヌル終端)にする
	static std::vector<char> buffer;
	buffer.resize(str.size() + 256); // 余裕を持たせてリサイズ

	// std::stringの内容をバッファにコピー
	memcpy(buffer.data(), str.c_str(), str.size() + 1);

	// ImGui::InputTextを使用
	bool result = ImGui::InputText(label, buffer.data(), buffer.size(), flags);

	// バッファの内容が変更された場合、std::stringに反映
	if (result)
	{
		str = buffer.data();
	}

	return result;
}


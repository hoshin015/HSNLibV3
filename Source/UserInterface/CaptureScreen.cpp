#include "CaptureScreen.h"

#include "../../../External/ImGui/imgui.h"
#include "../Game/Object/StateMachine/Player/Player.h"
#include "../Game/Object/StateMachine/Enemy/Enemy.h"

CaptureScreen::CaptureScreen()
{
	constexpr uint32_t width = 1280;
	constexpr uint32_t height = 720;

	for (size_t i = 0; i < MAX_CAPTURE; i++)
	{
		frameBuffers[i] = std::make_unique<FrameBuffer>(width, height, false);
	}
}


void CaptureScreen::Initialize()
{
	for (size_t i = 0; i < MAX_CAPTURE; i++)
		frameBuffers[i]->Clear();

	currentCaptureIndex = 0;
	capture = false;
}


bool CaptureScreen::BeginCapture()
{
	if ((rand() % CAPTURE_FREQUENCY == 0 || capture) && Enemy::Instance().IsAwake() && Enemy::Instance().IsAlive())
	{
		frameBuffers[currentCaptureIndex]->Clear();
		frameBuffers[currentCaptureIndex]->Activate();

		return true;
	}

	return false;
}

void CaptureScreen::EndCapture()
{
	frameBuffers[currentCaptureIndex]->DeActivate();

	// Todo : 上書きされる
	currentCaptureIndex++;
	if (currentCaptureIndex >= MAX_CAPTURE)
		currentCaptureIndex = 0;

	capture = false;
}

void CaptureScreen::DrawDebugGui()
{
	ImGui::Begin(u8"画面キャプチャ");

	if (ImGui::Button(u8"キャプチャ", { 200.0f, 30.0f }))
		capture = true;

	for(size_t i = 0; i < 5; i++)
	{
		ImGui::Separator();
		ImGui::Image(frameBuffers[i]->shaderResourceViews[0].Get(), { 320.0f, 180.0f });
	}

	ImGui::End();
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CaptureScreen::GetRandomTexture()
{
	//size_t index = rand() % 5;
	//
	//auto srv = frameBuffers[index]->shaderResourceViews[0];
	//if (srv)
	//	return srv;

	//else
	//{
	//	return GetRandomTexture();
	//}

	//return nullptr;


	std::vector<size_t> buf;
	for (size_t i = 0; i < 5; i++)
	{
		if (frameBuffers[i]->shaderResourceViews[0])
			buf.emplace_back(i);
	}
	
	size_t index = rand() % buf.size();

	return frameBuffers[index]->shaderResourceViews[0];
}

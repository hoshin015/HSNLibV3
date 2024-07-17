#include "CameraManager.h"

#include "../../../External/ImGui/imgui.h"

#include "../../Source/Camera/CameraDerived.h"

#include "../../Library/Timer.h"

#include "../../Source/UserInterface/UiClearAfter.h"


CameraManager::CameraManager()
{
}


// --- カメラの取得 ---
std::shared_ptr<CameraBase> CameraManager::GetCamera(const char* key)
{
	auto it = cameraMap.find(key);

	if (it != cameraMap.end())
	{
		return it->second;
	}

	return nullptr;
}


// --- 現在のカメラの設定 ---
void CameraManager::SetCurrentCamera(const char* key)
{
	auto it = cameraMap.find(key);

	if (it != cameraMap.end())
	{
		currentCamera = it->second;
	}
}

void CameraManager::Initialize()
{
	shakeTimer = 0.0f;
	shakePower = 100.0f;
	clearTimer = 0.0f;
	clear = false;
	updateClearTimer = true;

	Register("PlayerCamera", std::make_shared<PlayerCamera>());
	Register("LockOnCamera", std::make_shared<LockOnCamera>());
	Register("EnemyDeadCamera", std::make_shared<EnemyDeadCamera>());
	Register("PlayerDeadCamera", std::make_shared<PlayerDeadCamera>());
	Register("ClearCamera", std::make_shared<ClearCamera>());
}


// --- 更新処理 ---
void CameraManager::Update()
{
	currentCamera->Update();
}


// --- 定数バッファの更新 ---
void CameraManager::UpdateConstants()
{
	currentCamera->UpdateConstants();
}


// --- カメラの登録 ---
void CameraManager::Register(std::string key, std::shared_ptr<CameraBase> camera)
{
	cameraMap.insert(std::make_pair(key, camera));
}


void CameraManager::DrawDebugGui()
{
	ImGui::Begin(u8"カメラマネージャー");

	ImGui::DragFloat(u8"クリアタイマー", &clearTimer);

	for (auto& camera : cameraMap)
	{
		if (ImGui::Button(camera.first.c_str(), { 200.0f, 30.0f }))
			currentCamera = camera.second;
	}

	ImGui::End();
}



void CameraManager::UpdateShake()
{
	shakeTimer -= Timer::Instance().UnscaledDeltaTime();

	if (shakeTimer < 0.0f)
	{
		shakeTimer = 0.0f;
		shakePower = 100.0f;
	}
}

void CameraManager::UpdateClearTimer()
{
	if(UiClearAfter::Instance().IsClear())
	{
		if (updateClearTimer)
			clearTimer -= Timer::Instance().DeltaTime();

		if (clearTimer < 0.0f)
		{
			// 更新止めてカメラ切り替え
			updateClearTimer = false;
			currentCamera = GetCamera("ClearCamera");
		}
	}
}

#include "CameraManager.h"

#include "../../Source/Camera/CameraDerived.h"

#include "../../Library/Timer.h"


CameraManager::CameraManager()
{
}


// --- �J�����̎擾 ---
std::shared_ptr<CameraBase> CameraManager::GetCamera(const char* key)
{
	auto it = cameraMap.find(key);

	if (it != cameraMap.end())
	{
		return it->second;
	}

	return nullptr;
}


// --- ���݂̃J�����̐ݒ� ---
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

	Register("PlayerCamera", std::make_shared<PlayerCamera>());
	Register("LockOnCamera", std::make_shared<LockOnCamera>());
	Register("EnemyDeadCamera", std::make_shared<EnemyDeadCamera>());
	Register("PlayerDeadCamera", std::make_shared<PlayerDeadCamera>());
}


// --- �X�V���� ---
void CameraManager::Update()
{
	currentCamera->Update();
}


// --- �萔�o�b�t�@�̍X�V ---
void CameraManager::UpdateConstants()
{
	currentCamera->UpdateConstants();
}


// --- �J�����̓o�^ ---
void CameraManager::Register(std::string key, std::shared_ptr<CameraBase> camera)
{
	cameraMap.insert(std::make_pair(key, camera));
}



void CameraManager::UpdateShake()
{
	shakeTimer -= Timer::Instance().DeltaTime();

	if (shakeTimer < 0.0f)
	{
		shakeTimer = 0.0f;
		shakePower = 0.0f;
	}
}

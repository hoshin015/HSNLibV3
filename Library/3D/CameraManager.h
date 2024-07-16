#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "CameraBase.h"

class CameraManager
{
private:
	CameraManager();
	~CameraManager() {}

public:
	static CameraManager& Instance()
	{
		static CameraManager instance;
		return instance;
	}

	std::shared_ptr<CameraBase> GetCamera(const char* key);
	std::shared_ptr<CameraBase> GetCamera() { return currentCamera; }

	void SetCurrentCamera(const char* key);

	bool IsCurrentCamera(const char* key) { return cameraMap[key] == currentCamera; }

	void Initialize();

	void Update();
	void UpdateConstants();
	void Register(std::string key, std::shared_ptr<CameraBase> camera);
	void DrawDebugGui();

	void UpdateShake();
	void UpdateClearTimer();


	float shakeTimer;
	float shakePower = 100.0f;
	float clearTimer;
	bool clear; // Todo : ‰¼ƒtƒ‰ƒO
	bool updateClearTimer;


private:
	std::unordered_map<std::string, std::shared_ptr<CameraBase>> cameraMap;
	std::shared_ptr<CameraBase> currentCamera = nullptr;
};


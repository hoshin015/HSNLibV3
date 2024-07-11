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

	void Initialize();

	void Update();
	void UpdateConstants();
	void Register(std::string key, std::shared_ptr<CameraBase> camera);


	float shakeTimer;


private:
	std::unordered_map<std::string, std::shared_ptr<CameraBase>> cameraMap;
	std::shared_ptr<CameraBase> currentCamera = nullptr;
};


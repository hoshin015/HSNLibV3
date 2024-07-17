#pragma once

#include "../../Source/Game/Object/Base/AnimatedObject.h"

class Gate final : public AnimatedObject
{
private:
	Gate(const char* fileName) : AnimatedObject(fileName) {}
	~Gate() {};
public:
	static Gate& Instance()
	{
		static Gate instance("Data/Fbx/Gate/mon.model");
		return instance;
	}

	void Initialize();
	void Update() override;
	void Render(bool isShadow = false) override;
	void DrawDebugImGui();

	float timer;
	float maxTime = 2.5f;
	float speed = 5.0f;
	float fallSpeed = 10.0f;
	bool complete;
};


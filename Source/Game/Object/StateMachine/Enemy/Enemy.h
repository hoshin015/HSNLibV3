#pragma once
#include "../../Base/AnimatedObject.h"

class Enemy : public AnimatedObject
{
private:
	Enemy(const char* filePath) : AnimatedObject(filePath) {}
	~Enemy() override {}

public:
	static Enemy& Instance()
	{
		static Enemy instance("Data/Fbx/Albino/Albino.model");
		return instance;
	}

	void Initialize();
	void Update() override;
	void Render(bool isShadow = false) override;
};
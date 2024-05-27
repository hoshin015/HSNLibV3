#pragma once
#include "Base/AnimatedObject.h"

class TestAnimated : public AnimatedObject
{
public:
	TestAnimated(const char* filePath) : AnimatedObject(filePath) {}
	~TestAnimated() override {}

	void Update() override;
	void Render(bool isShadow = false) override;
};
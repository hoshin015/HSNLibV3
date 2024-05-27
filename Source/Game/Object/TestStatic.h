#pragma once
#include "Base/StaticObject.h"

class TestStatic : public StaticObject
{
public:
	TestStatic(const char* filePath) : StaticObject(filePath) {}
	~TestStatic() override {}

	void Update() override;
	void Render(bool isShadow = false) override;

private:
	static const int instanceNum = 100;
	DirectX::XMFLOAT4X4 m[instanceNum] = {};
};
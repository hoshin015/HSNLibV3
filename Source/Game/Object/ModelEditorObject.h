#pragma once
#include "Base/AnimatedObject.h"

class ModelEditorObject : public AnimatedObject
{
public:
	ModelEditorObject(const char* filePath) : AnimatedObject(filePath) {}
	~ModelEditorObject() override {}

	void Update() override;
	void Render(bool isShadow = false) override;
};
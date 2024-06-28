#pragma once

#include "../../Base/StaticObject.h"
#include "../../Base/AnimatedObject.h"
#include "../../Library/Easing.h"
#include "../Lightning/LightningMainMesh.h"

// breath 吐き出し口のエフェクト
class BreathArea : public AnimatedObject
{
public:
	BreathArea(const char* filePath) : AnimatedObject(filePath){}
	~BreathArea() override {}

	void Update() override;
	void Render(bool isShadow) override;
private:
};


// breath の中心エフェクト
class BreathCrossPlane : public AnimatedObject
{
public:
	BreathCrossPlane(const char* filePath) : AnimatedObject(filePath) {}
	~BreathCrossPlane() override {}

	void Update() override;
	void Render(bool isShadow) override;
};


// breath の筒エフェクト
class BreathCylinder : public AnimatedObject
{
public:
	BreathCylinder(const char* filePath) : AnimatedObject(filePath) {}
	~BreathCylinder() override {}

	void Update() override;
	void Render(bool isShadow) override;
};

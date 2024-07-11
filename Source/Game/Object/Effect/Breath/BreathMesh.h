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

	void ReSetLifeTime() { lifeTimer = 0.0f; }

private:
	float lifeTimer = 0.0f;

	Easing::EasingValue crossPlaneStartScale =
	{
		0.0f, 0.15f,
		0.0f,6.0f
	};
	Easing::EasingValue crossPlaneEndScale =
	{
		1.0f, 2.0f,
		6.0f,0.0f
	};
};


// breath の筒エフェクト
class BreathCylinder : public AnimatedObject
{
public:
	BreathCylinder(const char* filePath) : AnimatedObject(filePath) {}
	~BreathCylinder() override {}

	void Update() override;
	void Render(bool isShadow) override;

	void ReSetLifeTime() { lifeTimer = 0.0f; }

private:
	float lifeTimer = 0.0f;

	Easing::EasingValue cylinderStartScale =
	{
		0.0f, 0.25f,
		0.0f,3.6f
	};
	Easing::EasingValue cylinderEndScale =
	{
		1.5f, 2.0f,
		3.6f,0.0f
	};
	Easing::EasingValue cylinderEndDissolveThread =
	{
		1.0f, 2.0f,
		0.2f,0.0f
	};
};

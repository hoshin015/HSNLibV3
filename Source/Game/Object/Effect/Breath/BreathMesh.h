#pragma once

#include "../../Base/StaticObject.h"
#include "../../Base/AnimatedObject.h"
#include "../../Library/Easing.h"
#include "../Lightning/LightningMainMesh.h"

// breath �f���o�����̃G�t�F�N�g
class BreathArea : public AnimatedObject
{
public:
	BreathArea(const char* filePath) : AnimatedObject(filePath){}
	~BreathArea() override {}

	void Update() override;
	void Render(bool isShadow) override;
private:
};


// breath �̒��S�G�t�F�N�g
class BreathCrossPlane : public AnimatedObject
{
public:
	BreathCrossPlane(const char* filePath) : AnimatedObject(filePath) {}
	~BreathCrossPlane() override {}

	void Update() override;
	void Render(bool isShadow) override;

private:
	float lifeTimer = 0.0f;

	Easing::EasingValue crossPlaneStartScale =
	{
		0.0f, 0.15f,
		0.0f,2.0f
	};
	Easing::EasingValue crossPlaneEndScale =
	{
		2.5f, 3.0f,
		2.0f,0.0f
	};
};


// breath �̓��G�t�F�N�g
class BreathCylinder : public AnimatedObject
{
public:
	BreathCylinder(const char* filePath) : AnimatedObject(filePath) {}
	~BreathCylinder() override {}

	void Update() override;
	void Render(bool isShadow) override;

private:
	float lifeTimer = 0.0f;

	Easing::EasingValue cylinderStartScale =
	{
		0.0f, 0.25f,
		0.0f,1.2f
	};
	Easing::EasingValue cylinderEndScale =
	{
		2.5f, 3.0f,
		1.2f,0.0f
	};
	Easing::EasingValue cylinderEndDissolveThread =
	{
		2.0f, 3.0f,
		0.2f,0.0f
	};
};

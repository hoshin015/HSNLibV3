#include "BreathEffect.h"

void BreathEffect::Initialize()
{
	crossPlaneObject = std::make_unique<BreathCrossPlane>("Data/Fbx/Breath/CrossPlane/CrossPlane.model");
	cylinderObject = std::make_unique<BreathCylinder>("Data/Fbx/Breath/Cylinder/Cylinder.model");
}

void BreathEffect::Update()
{
	crossPlaneObject->Update();
	cylinderObject->Update();
}

void BreathEffect::Render(bool isShadow)
{
	crossPlaneObject->Render(isShadow);
	cylinderObject->Render(isShadow);
}

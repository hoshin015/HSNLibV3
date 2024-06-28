#include "BreathEffect.h"

void BreathEffect::Initialize()
{
	crossPlaneObject = std::make_unique<BreathCrossPlane>("Data/Fbx/Breath/CrossPlane/CrossPlane.model");
}

void BreathEffect::Update()
{
	crossPlaneObject->Update();
}

void BreathEffect::Render(bool isShadow)
{
	crossPlaneObject->Render(isShadow);
}

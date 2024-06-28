#include "BreathMesh.h"
#include "../../../../../Library/Timer.h"

void BreathCrossPlane::Update()
{
	position = { 0,1,0 };
	scale = { 5, 5, 15 };

	float detaTime = Timer::Instance().DeltaTime();
	GetModel()->uvScrollConstant.uvScrollValue.y += 2 * detaTime;

	// Žp¨s—ñXV
	UpdateTransform();
}

void BreathCrossPlane::Render(bool isShadow)
{
	model->Render(transform, nullptr, isShadow);
}

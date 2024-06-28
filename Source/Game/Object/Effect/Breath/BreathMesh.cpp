#include "BreathMesh.h"
#include "../../../../../Library/Timer.h"

void BreathCrossPlane::Update()
{
	position = { 0,1,0 };
	scale = { 5, 15, 5 };
	angle = { 90,0,0 };

	float detaTime = Timer::Instance().DeltaTime();
	GetModel()->uvScrollConstant.uvScrollValue.y -= 2 * detaTime;

	// 姿勢行列更新
	UpdateTransform();
}

void BreathCrossPlane::Render(bool isShadow)
{
	model->Render(transform, nullptr, isShadow);
}





void BreathCylinder::Update()
{
	position = { 0,1,0 };
	scale = { 5, 15, 5 };
	angle = { 90,0,0 };

	float detaTime = Timer::Instance().DeltaTime();
	GetModel()->uvScrollConstant.uvScrollValue.y -= 2 * detaTime;

	// 姿勢行列更新
	UpdateTransform();
}

void BreathCylinder::Render(bool isShadow)
{
	model->Render(transform, nullptr, isShadow);
}

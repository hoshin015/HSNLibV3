#include "BreathMesh.h"
#include "../../../../../Library/Timer.h"
#include "../../../../../Library/Input/InputManager.h"

void BreathCrossPlane::Update()
{
	float deltaTime = Timer::Instance().DeltaTime();
	lifeTimer += deltaTime;

	GetModel()->uvScrollConstant.uvScrollValue.y += 1.5 * deltaTime;

	// 開始スケール
	float scale = Easing::GetNowParam(Easing::OutQuad<float>, lifeTimer, crossPlaneStartScale);
	SetScale({ scale, 140, scale });

	// 終了スケール
	if(lifeTimer > crossPlaneEndScale.startTime)
	{
		float scale = Easing::GetNowParam(Easing::OutQuad<float>, lifeTimer, crossPlaneEndScale);
		SetScale({ scale, 140, scale });
	}

	GetModel()->dissolveConstant.dissolveThreshold = 0.01f;

	// 姿勢行列更新
	UpdateTransform();
}

void BreathCrossPlane::Render(bool isShadow)
{
	model->Render(transform, nullptr, isShadow);
}





void BreathCylinder::Update()
{
	float deltaTime = Timer::Instance().DeltaTime();
	lifeTimer += deltaTime;


	GetModel()->uvScrollConstant.uvScrollValue.y += 1.5 * deltaTime;

	// 開始スケール
	float scale = Easing::GetNowParam(Easing::OutQuad<float>, lifeTimer, cylinderStartScale);
	SetScale({ scale, 140, scale });

	// 終了スケール
	if (lifeTimer > cylinderEndScale.startTime)
	{
		float scale = Easing::GetNowParam(Easing::OutQuad<float>, lifeTimer, cylinderEndScale);
		SetScale({ scale, 140, scale });
	}

	// 終了ディゾルブ値
	GetModel()->dissolveConstant.dissolveThreshold = Easing::GetNowParam(Easing::OutQuad<float>, lifeTimer, cylinderEndDissolveThread);


	// 姿勢行列更新
	UpdateTransform();
}

void BreathCylinder::Render(bool isShadow)
{
	model->Render(transform, nullptr, isShadow);
}

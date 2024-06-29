#include "BreathMesh.h"
#include "../../../../../Library/Timer.h"
#include "../../../../../Library/Input/InputManager.h"

void BreathCrossPlane::Update()
{
	float deltaTime = Timer::Instance().DeltaTime();
	lifeTimer += deltaTime;


	GetModel()->uvScrollConstant.uvScrollValue.y += 1.5 * deltaTime;

	if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Q))
	{
		lifeTimer = 0.0f;
	}

	// �J�n�X�P�[��
	float scale = Easing::GetNowParam(Easing::OutQuad<float>, lifeTimer, crossPlaneStartScale);
	SetScale({ scale, 80, scale });

	// �I���f�B�]���u�l
	if(lifeTimer > crossPlaneEndScale.startTime)
	{
		float scale = Easing::GetNowParam(Easing::OutQuad<float>, lifeTimer, crossPlaneEndScale);
		SetScale({ scale, 80, scale });
	}
	

	// �p���s��X�V
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

	if(InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Q))
	{
		lifeTimer = 0.0f;
	}

	// �J�n�X�P�[��
	float scale = Easing::GetNowParam(Easing::OutQuad<float>, lifeTimer, cylinderStartScale);
	SetScale({ scale, 80, scale });

	// �I���X�P�[��
	if (lifeTimer > cylinderEndScale.startTime)
	{
		float scale = Easing::GetNowParam(Easing::OutQuad<float>, lifeTimer, cylinderEndScale);
		SetScale({ scale, 80, scale });
	}

	// �I���f�B�]���u�l
	GetModel()->dissolveConstant.dissolveThreshold = Easing::GetNowParam(Easing::OutQuad<float>, lifeTimer, cylinderEndDissolveThread);


	// �p���s��X�V
	UpdateTransform();
}

void BreathCylinder::Render(bool isShadow)
{
	model->Render(transform, nullptr, isShadow);
}
#include "BreathEffect.h"
#include "../Lightning/LightningEffect.h"
#include "../../../../../Library/Timer.h"
#include "../../../../../Library/Particle/EmitterManager.h"
#include "../../../../../Library/Input/InputManager.h"

void BreathEffect::Initialize()
{

	crossPlaneObject = std::make_unique<BreathCrossPlane>("Data/Fbx/Breath/CrossPlane/CrossPlane.model");
	crossPlaneObject->SetPos({ 0,3,0 });
	crossPlaneObject->SetScale({ 2, 80, 2 });
	crossPlaneObject->SetAngle({ 90,0,0 });

	
	cylinderObject = std::make_unique<BreathCylinder>("Data/Fbx/Breath/Cylinder/Cylinder.model");
	cylinderObject->SetPos({ 0,3,0 });
	cylinderObject->SetScale({ 1.2, 80, 1.2 });
	cylinderObject->SetAngle({ 90,0,0 });
	cylinderObject->GetModel()->dissolveConstant.dissolveThreshold = 0.2f;
	cylinderObject->GetModel()->dissolveConstant.edgeThreshold = 0.3f;
	cylinderObject->GetModel()->dissolveConstant.edgeColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	cylinderObject->GetModel()->data.emissivePower = 2.0f;
}

void BreathEffect::Update()
{
	if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Q))
	{
		lifeTimer = 0.0f;
	}

	float deltaTime = Timer::Instance().DeltaTime();
	lifeTimer += deltaTime;

	if(lifeTimer < endTime)
	{
		// lightning ¶¬
		LightningGenerateUpdate();
	}
	

	crossPlaneObject->Update();
	cylinderObject->Update();
}

void BreathEffect::Render(bool isShadow)
{
	crossPlaneObject->Render(isShadow);
	cylinderObject->Render(isShadow);
}

// lightning ¶¬
void BreathEffect::LightningGenerateUpdate()
{
	float deltaTime = Timer::Instance().DeltaTime();

	static float bottomTimer = 0.0f;
	bottomTimer += deltaTime;
	while (bottomTimer > 0.01)
	{
		for (int i = 0; i < 24; i++)
		{
			LightningData* l = new LightningData();

			float r = rand() % 360 / 3.14;
			DirectX::XMFLOAT3 randomPos = { cosf(r) * 0.7f, -sinf(r) * 0.7f, (rand() % 50) * 1.0f };

			l->SetPos({ 0 + randomPos.x,3 + randomPos.y,0 + randomPos.z });

			float life = rand() % 1 * 0.1f + 0.1f;
			l->SetLifeTime(life);
			float s = (rand() % 1) * 0.1f + 0.05f;
			l->SetScale({ s + 0.05f, s, s });
			l->SetAngleY(90);
			l->SetAngleX(rand() % 360);
			l->SetEmissivePower(2.0f);
			l->SetUpdateType(LightningData::LightningFuncEnum::BreathCylinder);
			l->SetColor({ 20.8, 2.8, 2.5, 1 });

			if (rand() % 2)
			{
				LightningEffect::Instance().lightningMesh4->Register(l);
			}
			else
			{
				LightningEffect::Instance().lightningMesh5->Register(l);
			}
		}

		bottomTimer -= 0.2;
	}
}

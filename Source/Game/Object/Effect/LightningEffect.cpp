#include "LightningEffect.h"
#include "../../../../Library/Timer.h"
#include "../../../../Library/Particle/EmitterManager.h"

// ‰Šú‰»
void LightningEffect::Initialize()
{
	lightningMesh1 = std::make_unique<LightningMesh1>("Data/Fbx/Lightning/Main/1/mainLightning.model");
	lightningMesh2 = std::make_unique<LightningMesh1>("Data/Fbx/Lightning/Main/2/mainLightning.model");
	lightningMesh3 = std::make_unique<LightningMesh1>("Data/Fbx/Lightning/Area/LightningArea.model");
}

void LightningEffect::Update()
{
	float deltaTime = Timer::Instance().DeltaTime();

	int index = 0;
	for(auto & lightningEmit : lightningEmitters)
	{
		lightningEmit.timer += deltaTime;
		if(!lightningEmit.addLightning1 && lightningEmit.timer >= lightning1GenerateTime)
		{
			LightningData* l = new LightningData();
			l->SetLifeTime(1.0f);
			l->SetScale({ 0.5,0.5,0.5 });
			l->SetEmissivePower(2.0f);
			lightningMesh1->Register(l);

			lightningEmit.addLightning1 = true;
		}

		if(lightningEmit.timer >= lightningEmit.destoryTime)
		{
			lightningEmitters.erase(lightningEmitters.begin() + index);
			continue;
		}
		index++;
	}


	lightningMesh1->Update();
	lightningMesh2->Update();
	lightningMesh3->Update();
}

void LightningEffect::Render(bool isShadow)
{
	lightningMesh1->Render(isShadow);
	lightningMesh2->Render(isShadow);
	lightningMesh3->Render(isShadow);
}

void LightningEffect::Emit()
{
	LightningData* l2 = new LightningData();
	l2->SetLifeTime(1.7f);
	l2->SetPosY(l2->GetPosY() + 0.001);
	l2->SetEmissivePower(0.5f);
	l2->SetScale({ 4,0.5,4 });
	l2->SetColor({ 0.8,0.8,1.0,1 });
	l2->SetUpdateType(LightningData::LightningFuncEnum::Area);
	lightningMesh3->Register(l2);

	Emitter* emitter = new Emitter();
	emitter->position = { 0, 0, 0 };
	emitter->emitterData.duration = 1.2;
	emitter->emitterData.looping = false;
	emitter->emitterData.burstsTime = 0.05;
	emitter->emitterData.burstsCount = 5;
	emitter->emitterData.particleKind = 4;
	emitter->emitterData.particleLifeTimeMin = 0.6f;
	emitter->emitterData.particleLifeTimeMax = 2.0f;
	emitter->emitterData.particleSpeedMin = 0.025f;
	emitter->emitterData.particleSpeedMax = 2.0f;
	emitter->emitterData.particleSizeMin = { 0.1f, 0.05 };
	emitter->emitterData.particleSizeMax = { 0.4f, 0.1f };
	emitter->emitterData.particleColorMin = { 2.8, 2.8, 20.0, 1 };
	emitter->emitterData.particleColorMax = { 2.8, 2.8, 20.5, 1 };
	emitter->emitterData.particleFrictionMin = 0;
	emitter->emitterData.particleFrictionMax = 0.01;
	emitter->emitterData.particleGravity = 0;
	emitter->emitterData.particleBillboardType = 1;
	EmitterManager::Instance().Register(emitter);

	LightningEmitter lightningEmitter;
	lightningEmitters.push_back(lightningEmitter);
}

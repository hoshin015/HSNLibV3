#include "RockEffect.h"
#include "../../../../../Library/Math/Math.h"

void RockEffect::Initialize()
{
	rockMesh1 = std::make_unique<RockMainMesh>("Data/Fbx/Rock/Rock.model");
}

void RockEffect::Update()
{
	rockMesh1->Update();
}

void RockEffect::Render(bool isShadow)
{
	rockMesh1->Render(isShadow);
}

void RockEffect::Emit(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 vel, DirectX::XMFLOAT2 scaleMinMax)
{
	RockData* rock = new RockData();
	rock->SetPos(pos);
	rock->SetVeloicty(vel);
	rock->SetLifeTime(10.0f);
	rock->SetEmissivePower(0.0f);
	float rScale = Math::RandomRange(scaleMinMax.x, scaleMinMax.y);
	rock->SetScale({rScale, rScale, rScale});
	float rX = rand() % 360;
	float rY = rand() % 360;
	float rZ = rand() % 360;
	rock->SetAngle({rX, rY, rZ});
	rock->SetColor({1.0, 1.0, 1.0, 1});
	rock->SetUpdateType(RockData::RockFuncEnum::Default);
	rockMesh1->Register(rock);
}

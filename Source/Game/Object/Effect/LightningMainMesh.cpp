#include "LightningMainMesh.h"
#include "../../../../Library/Timer.h"

void LightningMainMesh::Initialize()
{
	lightningMainTimer = 0.0f;
	IsLightningMainMeshChild1Generate = false;
	IsLightningMainMeshChild2Generate = false;
	IsLightningMainMeshChild3Generate = false;
}

void LightningMainMesh::Update()
{
	lightningMainTimer += Timer::Instance().DeltaTime();

	int index = 0;

	// lightning1 データ
	for(auto& data : lightningData)
	{
		// 行列更新と寿更新
		data->Update();

		// 寿命を超えたら登録解除
		if (data->GetlifeTimer() >= data->GetlifeTime())
		{
			delete data;
			lightningData.erase(lightningData.begin() + index);
			continue;
		}

		// 更新した値を配列にいれる
		lightning1Transforms[index] = data->GetTransform();
		lightning1Emissives[index] = data->GetEmissivePower();

		index++;
	}

	if(!IsLightningMainMeshChild1Generate && lightningMainMeshChild1GenerateTime <= lightningMainTimer)
	{
		LightningMainMeshChild1* l = new LightningMainMeshChild1(path.c_str());
		l->SetLifeTimer(0.0f);
		l->SetPos({ 2,0,0 });
		l->SetScale({ 1,2,1 });
		l->SetEmissivePower(3.0);
		lightningData.emplace_back(l);
		IsLightningMainMeshChild1Generate = true;
	}
	if (!IsLightningMainMeshChild2Generate && lightningMainMeshChild2GenerateTime <= lightningMainTimer)
	{
		LightningMainMeshChild2* l = new LightningMainMeshChild2(path.c_str());
		l->SetLifeTimer(0.0f);
		l->SetLifeTime(0.1f);
		l->SetPos({ 2,0,0 });
		l->SetScale({ 0.5,2,0.5 });
		l->SetEmissivePower(1.0);
		lightningData.emplace_back(l);
		IsLightningMainMeshChild2Generate = true;
	}
	if (!IsLightningMainMeshChild3Generate && lightningMainMeshChild3GenerateTime <= lightningMainTimer)
	{
		LightningMainMeshChild2* l = new LightningMainMeshChild2(path.c_str());
		l->SetLifeTimer(0.0f);
		l->SetLifeTime(0.2f);
		l->SetPos({ 2,0,0 });
		l->SetScale({ 0.5,2,0.5 });
		l->SetEmissivePower(1.0);
		lightningData.emplace_back(l);
		IsLightningMainMeshChild3Generate = true;
	}
}

void LightningMainMesh::Render(bool isShadow)
{
	if (lightningData.empty()) return;
	model->Render(lightningData.size(), lightning1Transforms, lightning1Emissives, isShadow);
}

void LightningMainMeshChild1::Update()
{
	float s = Easing::GetNowParam(Easing::OutCubic<float>, lifeTimer, lightning1Scale);
	SetScale({ s, GetScaleY(), s });

	lifeTimer += Timer::Instance().DeltaTime();
	if (lifeTimer > lifeTime) lifeTimer = lifeTime;

	// 姿勢行列更新
	UpdateTransform();
}

void LightningMainMeshChild2::Update()
{
	lifeTimer += Timer::Instance().DeltaTime();
	if (lifeTimer > lifeTime) lifeTimer = lifeTime;

	// 姿勢行列更新
	UpdateTransform();
}

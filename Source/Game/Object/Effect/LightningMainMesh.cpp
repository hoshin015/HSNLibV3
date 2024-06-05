#include "LightningMainMesh.h"
#include "../../../../Library/Timer.h"

void LightningMainMesh::Initialize()
{
	LightningMainMeshChild* l = new LightningMainMeshChild(path.c_str());
	l->SetPos({ 2,0,0 });
	lightningData.emplace_back(l);

	LightningMainMeshChild* l2 = new LightningMainMeshChild(path.c_str());
	l2->SetPos({ -2,0,0 });
	lightningData.emplace_back(l2);
}

void LightningMainMesh::Update()
{
	int index = 0;
	for(auto& data : lightningData)
	{
		// 行列更新と寿更新
		data->Update();

		// 寿命切れなら登録解除
		if(data->GetlifeTimer() < 0.0f)
		{
			delete data;
			lightningData.erase(lightningData.begin() + index);
			continue;
		}

		// 更新した行列をいれる
		m[index] = data->GetTransform();

		index++;
	}
}

void LightningMainMesh::Render(bool isShadow)
{
	model->Render(lightningData.size(), m, isShadow);
}

void LightningMainMeshChild::Update()
{
	lifeTimer -= Timer::Instance().DeltaTime();

	// 姿勢行列更新
	UpdateTransform();
}

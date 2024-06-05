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
		// �s��X�V�Ǝ��X�V
		data->Update();

		// �����؂�Ȃ�o�^����
		if(data->GetlifeTimer() < 0.0f)
		{
			delete data;
			lightningData.erase(lightningData.begin() + index);
			continue;
		}

		// �X�V�����s��������
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

	// �p���s��X�V
	UpdateTransform();
}

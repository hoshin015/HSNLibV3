#include "RockMainMesh.h"
#include "../../../../../Library/Timer.h"

void RockData::Update()
{
	// �֐��|�C���^
	(this->*funcs[static_cast<int>(updateType)])();
	UpdateTransform();

	lifeTimer += Timer::Instance().DeltaTime();
	if (lifeTimer >= lifeTime)
	{
		owner->Remove(this);
	}
}

void RockData::UpdateTransform()
{
	// �X�P�[���s����쐬
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(GetScaleX(), GetScaleY(), GetScaleZ());
	// ��]�s����쐬
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(GetAngleX()), DirectX::XMConvertToRadians(GetAngleY()), DirectX::XMConvertToRadians(GetAngleZ()));
	// �ʒu�s����쐬
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(GetPosX(), GetPosY(), GetPosZ());

	// �S�̍s���g�ݍ��킹�A���[���h�s����쐬
	DirectX::XMMATRIX W = MSC * S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);
}

void RockData::RockDefaultUpdate()
{
	float deltaTime = Timer::Instance().DeltaTime();

	// �d��
	velocity.y -= gravity * deltaTime;

	position.x += velocity.x * deltaTime;
	position.y += velocity.y * deltaTime;
	position.z += velocity.z * deltaTime;

	// �n�ʂ�肵���Ȃ�폜
	if(position.y <= -1)
	{
		owner->Remove(this);
	}
}

void RockData::RockUpUpdate()
{
	float deltaTime = Timer::Instance().DeltaTime();

	// �t�d��
	velocity.y += gravity * deltaTime;

	position.x += velocity.x * deltaTime;
	position.y += velocity.y * deltaTime;
	position.z += velocity.z * deltaTime;

	// �n�ʂ�肵���Ȃ�폜
	if (position.y >= 20)
	{
		owner->Remove(this);
	}
}


void RockMainMesh::Initialize()
{
}

void RockMainMesh::Update()
{
	int index = 0;

	// rock �f�[�^�X�V
	for (auto& data : rockInfo)
	{
		// --- �s��X�V�Ǝ��X�V ---
		// �X�P�[���ύX
		DirectX::XMMATRIX MS = DirectX::XMMatrixScaling(model->GetModelResource()->GetScale(), model->GetModelResource()->GetScale(), model->GetModelResource()->GetScale());
		// ���ϊ��s��
		DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&model->GetModelResource()->GetCoordinateSystemTransform());
		data->Update();

		// --- �X�V�����l��z��ɂ���� ---
		rockTransforms[index] = data->GetTransform();
		rockEmissives[index] = data->GetEmissivePower();
		rockColors[index] = data->GetColor();

		index++;
	}

	// rock �f�[�^�폜
	for (auto& data : removes)
	{
		std::vector<RockData*>::iterator it = std::find(rockInfo.begin(), rockInfo.end(), data);

		if (it != rockInfo.end())
		{
			rockInfo.erase(it);
		}
		delete data;
	}
	removes.clear();
}

void RockMainMesh::Render(bool isShadow)
{
	if (rockInfo.empty()) return;
	model->Render(rockInfo.size(), rockTransforms, rockEmissives, rockColors, isShadow);
}

void RockMainMesh::Register(RockData* rockData)
{
	// �X�P�[���ύX
	DirectX::XMMATRIX MS = DirectX::XMMatrixScaling(model->GetModelResource()->GetScale(), model->GetModelResource()->GetScale(), model->GetModelResource()->GetScale());
	// ���ϊ��s��
	DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&model->GetModelResource()->GetCoordinateSystemTransform());

	rockData->SetOwner(this);
	rockData->SetMSC(MS * C);
	rockInfo.emplace_back(rockData);
}

void RockMainMesh::Remove(RockData* rockData)
{
	// �j�����X�g�ɒǉ�
	removes.insert(rockData);
}

void RockMainMesh::Clear()
{
	for(auto rock : rockInfo)
	{
		delete rock;
	}
	rockInfo.clear();
}

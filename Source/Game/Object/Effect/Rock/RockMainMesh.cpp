#include "RockMainMesh.h"
#include "../../../../../Library/Timer.h"

void RockData::Update()
{
	// 関数ポインタ
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
	// スケール行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(GetScaleX(), GetScaleY(), GetScaleZ());
	// 回転行列を作成
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(GetAngleX()), DirectX::XMConvertToRadians(GetAngleY()), DirectX::XMConvertToRadians(GetAngleZ()));
	// 位置行列を作成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(GetPosX(), GetPosY(), GetPosZ());

	// ４つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX W = MSC * S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);
}

void RockData::RockDefaultUpdate()
{
	float deltaTime = Timer::Instance().DeltaTime();

	// 重力
	velocity.y -= gravity * deltaTime;

	position.x += velocity.x * deltaTime;
	position.y += velocity.y * deltaTime;
	position.z += velocity.z * deltaTime;

	// 地面よりしたなら削除
	if(position.y <= -1)
	{
		owner->Remove(this);
	}
}

void RockData::RockUpUpdate()
{
	float deltaTime = Timer::Instance().DeltaTime();

	// 逆重力
	velocity.y += gravity * deltaTime;

	position.x += velocity.x * deltaTime;
	position.y += velocity.y * deltaTime;
	position.z += velocity.z * deltaTime;

	// 地面よりしたなら削除
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

	// rock データ更新
	for (auto& data : rockInfo)
	{
		// --- 行列更新と寿更新 ---
		// スケール変更
		DirectX::XMMATRIX MS = DirectX::XMMatrixScaling(model->GetModelResource()->GetScale(), model->GetModelResource()->GetScale(), model->GetModelResource()->GetScale());
		// 軸変換行列
		DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&model->GetModelResource()->GetCoordinateSystemTransform());
		data->Update();

		// --- 更新した値を配列にいれる ---
		rockTransforms[index] = data->GetTransform();
		rockEmissives[index] = data->GetEmissivePower();
		rockColors[index] = data->GetColor();

		index++;
	}

	// rock データ削除
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
	// スケール変更
	DirectX::XMMATRIX MS = DirectX::XMMatrixScaling(model->GetModelResource()->GetScale(), model->GetModelResource()->GetScale(), model->GetModelResource()->GetScale());
	// 軸変換行列
	DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&model->GetModelResource()->GetCoordinateSystemTransform());

	rockData->SetOwner(this);
	rockData->SetMSC(MS * C);
	rockInfo.emplace_back(rockData);
}

void RockMainMesh::Remove(RockData* rockData)
{
	// 破棄リストに追加
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

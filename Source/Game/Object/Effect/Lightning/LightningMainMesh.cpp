#include "LightningMainMesh.h"
#include "../../../../../Library/Timer.h"




void LightningData::LightningNoUpdate(){}

void LightningData::LightningAreaUpdate()
{
	float alpha = Easing::OutBounce(lifeTimer, lifeTime, 0.7f, 0.0f);
	DirectX::XMFLOAT4 c = GetColor();
	c.w = alpha;
	SetColor(c);
}
void LightningData::LightningMainUpdate()
{
	float scale = Easing::GetNowParam(Easing::InCubic<float>, lifeTimer, val1);
	DirectX::XMFLOAT3 s = GetScale();
	s.x = scale;
	s.z = scale;
	SetScale(s);

	float ePower = 4 - Easing::InCirc(lifeTimer, lifeTime, 2.0f, 0.0f);
	SetEmissivePower(ePower);

	float alpha = 1 - Easing::OutBounce(lifeTimer, lifeTime, 1.0f, 0.0f);
	DirectX::XMFLOAT4 c = GetColor();
	c.w = alpha;
	SetColor(c);
}

void LightningData::LightningBottomUpdate()
{
	float alpha = Easing::OutBounce(lifeTimer, lifeTime, 0.7f, 0.0f);
	DirectX::XMFLOAT4 c = GetColor();
	c.w = alpha;
	SetColor(c);

	float ePower = 4 - Easing::InCirc(lifeTimer, lifeTime, 2.0f, 0.0f);
	SetEmissivePower(ePower);
}


void LightningData::Update()
{
	// 関数ポインタ
	(this->*funcs[static_cast<int>(updateType)])();

	lifeTimer += Timer::Instance().DeltaTime();
	if(lifeTimer >= lifeTime)
	{
		owner->Remove(this);
	}

	UpdateTransform();
}

void LightningData::UpdateTransform()
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

void LightningData::SetMSC(DirectX::XMMATRIX MSC)
{
	this->MSC = MSC;
}

void LightningMainMesh::Initialize()
{
}

void LightningMainMesh::Update()
{
	int index = 0;

	// lightning データ更新
	for(auto& data : lightningInfo)
	{
		// --- 行列更新と寿更新 ---
		// スケール変更
		DirectX::XMMATRIX MS = DirectX::XMMatrixScaling(model->GetModelResource()->GetScale(), model->GetModelResource()->GetScale(), model->GetModelResource()->GetScale());
		// 軸変換行列
		DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&model->GetModelResource()->GetCoordinateSystemTransform());
		data->Update();

		// --- 更新した値を配列にいれる ---
		lightningTransforms[index] = data->GetTransform();
		lightningEmissives[index] = data->GetEmissivePower();
		lightningColors[index] = data->GetColor();

		index++;
	}

	// lightning データ削除
	for (auto& data : removes)
	{
		std::vector<LightningData*>::iterator it = std::find(lightningInfo.begin(), lightningInfo.end(), data);

		if(it != lightningInfo.end())
		{
			lightningInfo.erase(it);
		}
		delete data;
	}
	removes.clear();
}

void LightningMainMesh::Render(bool isShadow)
{
	if (lightningInfo.empty()) return;
	model->Render(lightningInfo.size(), lightningTransforms, lightningEmissives, lightningColors, isShadow);
}


// 登録
void LightningMainMesh::Register(LightningData* lightningData)
{
	// スケール変更
	DirectX::XMMATRIX MS = DirectX::XMMatrixScaling(model->GetModelResource()->GetScale(), model->GetModelResource()->GetScale(), model->GetModelResource()->GetScale());
	// 軸変換行列
	DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&model->GetModelResource()->GetCoordinateSystemTransform());

	lightningData->SetOwner(this);
	lightningData->SetMSC(MS * C);
	lightningInfo.emplace_back(lightningData);
}

// 解除
void LightningMainMesh::Remove(LightningData* lightningData)
{
	// 破棄リストに追加
	removes.insert(lightningData);
}

// 全削除
void LightningMainMesh::Clear()
{
	for(auto& lightning : lightningInfo)
	{
		delete lightning;
	}
	removes.clear();
}

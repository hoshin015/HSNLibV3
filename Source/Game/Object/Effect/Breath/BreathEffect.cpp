#include "BreathEffect.h"
#include "../Lightning/LightningEffect.h"
#include "../../../../../Library/Timer.h"
#include "../../../../../Library/Particle/EmitterManager.h"
#include "../../../../../Library/Input/InputManager.h"

void BreathEffect::Initialize()
{
	crossPlaneObject = std::make_unique<BreathCrossPlane>("Data/Fbx/Breath/CrossPlane/CrossPlane.model");
	crossPlaneObject->SetPos({0, 3, 0});
	crossPlaneObject->SetScale({6, 80, 6});
	crossPlaneObject->SetAngle({90, 0, 0});


	cylinderObject = std::make_unique<BreathCylinder>("Data/Fbx/Breath/Cylinder/Cylinder.model");
	cylinderObject->SetPos({0, 3, 0});
	cylinderObject->SetScale({3.6, 80, 3.6});
	cylinderObject->SetAngle({90, 0, 0});
	cylinderObject->GetModel()->dissolveConstant.dissolveThreshold = 0.2f;
	cylinderObject->GetModel()->dissolveConstant.edgeThreshold     = 0.3f;
	cylinderObject->GetModel()->dissolveConstant.edgeColor         = {1.0f, 1.0f, 1.0f, 1.0f};
	cylinderObject->GetModel()->data.emissivePower                 = 2.0f;
}

void BreathEffect::Update()
{
	if (!isBreath) return;

	float deltaTime = Timer::Instance().DeltaTime();
	lifeTimer += deltaTime;

	if (lifeTimer > lifeTime)
	{
		isBreath = false;
		return;
	}

	if (lifeTimer < endTime)
	{
		// lightning 生成
		LightningGenerateUpdate();
	}


	crossPlaneObject->Update();
	cylinderObject->Update();
}

void BreathEffect::Render(bool isShadow)
{
	if (!isBreath) return;

	crossPlaneObject->Render(isShadow);
	cylinderObject->Render(isShadow);
}

// lightning 生成
void BreathEffect::LightningGenerateUpdate()
{
	// 回転行列の計算
	DirectX::XMMATRIX RotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(angle.x), DirectX::XMConvertToRadians(angle.y), DirectX::XMConvertToRadians(angle.z));
	// デフォルトのベクトル（Z軸方向）
	DirectX::XMVECTOR DefaultForward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DirectX::XMVECTOR DefaultUp      = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR DefaultRight   = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	// 回転行列を適用してベクトルを回転
	DirectX::XMVECTOR ForwardVector = XMVector3TransformCoord(DefaultForward, RotationMatrix);
	DirectX::XMVECTOR UpVector      = XMVector3TransformCoord(DefaultUp, RotationMatrix);
	DirectX::XMVECTOR RightVector   = XMVector3TransformCoord(DefaultRight, RotationMatrix);
	DirectX::XMFLOAT3 forward;
	DirectX::XMFLOAT3 up;
	DirectX::XMFLOAT3 right;
	DirectX::XMStoreFloat3(&forward, ForwardVector);
	DirectX::XMStoreFloat3(&up, UpVector);
	DirectX::XMStoreFloat3(&right, RightVector);


	float deltaTime = Timer::Instance().DeltaTime();

	lightningTimer += deltaTime;
	while (lightningTimer > lightningTime)
	{
		for (int i = 0; i < 24; i++)
		{
			LightningData* l = new LightningData();

			float r = static_cast<float>(rand()) / RAND_MAX * 2.0f * 3.14159f; // ランダムな角度をラジアンに変換
			float cos_r = cosf(r);
			float sin_r = -sinf(r);

			float length = static_cast<float>(rand() % 75);

			float rLength = 2.1f;

			DirectX::XMFLOAT3 randomPos;
			randomPos.x = (right.x * cos_r * rLength) + (up.x * length) + (forward.x * sin_r * rLength);
			randomPos.y = (right.y * cos_r * rLength) + (up.y * length) + (forward.y * sin_r * rLength);
			randomPos.z = (right.z * cos_r * rLength) + (up.z * length) + (forward.z * sin_r * rLength);

			l->SetPos({position.x + randomPos.x, position.y + randomPos.y, position.z + randomPos.z});

			float life = rand() % 1 * 0.1f + 0.1f;
			l->SetLifeTime(life);
			float s = (rand() % 1) * 1.5f + 0.15f;
			l->SetScale({s + 0.075f, s, s});
			 
			float randomR = rand() % 180;


			l->SetAngle(angle);
			l->SetAngleZ(angle.z - 90);
			l->SetAngleX(angle.x + randomR);

			l->SetEmissivePower(2.0f);
			l->SetUpdateType(LightningData::LightningFuncEnum::BreathCylinder);
			l->SetColor({20.8, 2.8, 2.5, 1});

			if (rand() % 2)
			{
				LightningEffect::Instance().lightningMesh4->Register(l);
			}
			else
			{
				LightningEffect::Instance().lightningMesh5->Register(l);
			}
		}

		lightningTimer -= 0.2;
	}
}


void BreathEffect::Emit()
{
	isBreath = true;

	lifeTimer = 0.0f;

	angle.y = rand() % 360;
	//angle.x = rand() % 360;
	//angle.z = rand() % 360;

	crossPlaneObject->SetPos(position);
	crossPlaneObject->SetAngle(angle);
	crossPlaneObject->ReSetLifeTime();
	cylinderObject->SetPos(position);
	cylinderObject->SetAngle(angle);
	cylinderObject->ReSetLifeTime();

	AudioManager::Instance().PlayMusic(MUSIC_LABEL::Breath);
}

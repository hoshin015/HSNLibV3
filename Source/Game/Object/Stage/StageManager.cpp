// --- Game ---
#include "StageManager.h"

// �X�V����
void StageManager::Update()
{
	for (Stage* stage : stages)
	{
		stage->Update();
	}
}

// �`�揈��
void StageManager::Render(bool isShadow)
{
	for (Stage* stage : stages)
	{
		stage->Render(isShadow);
	}
}

// �X�e�[�W�o�^
void StageManager::Register(Stage* stage)
{
	stages.emplace_back(stage);
}

// �X�e�[�W�S�폜
void StageManager::Clear()
{
	stages.clear();
}

// ���C�L���X�g
bool StageManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	bool result = false;

	hit.distance = FLT_MAX;
	for (Stage* stage : stages)
	{
		HitResult hitResult;
		if (stage->RayCast(start, end, hitResult))
		{
			if (hit.distance > hitResult.distance)
			{
				hit = hitResult;
				result = true;
			}
		}
	}

	return result;
}


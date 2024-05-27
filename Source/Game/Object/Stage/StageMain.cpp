// --- Game ---
#include "StageMain.h"


// �X�V����
void StageMain::Update()
{
	// �p���s��X�V
	UpdateTransform();
}

// �`�揈��
void StageMain::Render(bool isShadow)
{
	model->Render(transform, nullptr, isShadow);
}

// ���C�L���X�g
bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::IntersectRayVsModel(start, end, model->GetModelResource(), hit);
}

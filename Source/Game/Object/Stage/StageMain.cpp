// --- Game ---
#include "StageMain.h"
#include "../../../../Library/Timer.h"


// �X�V����
void StageMain::Update()
{
	GetModel()->uvScrollConstant.uvScrollValue.x += 0.01 * Timer::Instance().DeltaTime();


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

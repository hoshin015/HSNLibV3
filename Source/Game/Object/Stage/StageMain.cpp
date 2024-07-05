// --- Game ---
#include "StageMain.h"
#include "../../../../Library/Timer.h"


// 更新処理
void StageMain::Update()
{
	GetModel()->uvScrollConstant.uvScrollValue.x += 0.01 * Timer::Instance().DeltaTime();


	// 姿勢行列更新
	UpdateTransform();
}

// 描画処理
void StageMain::Render(bool isShadow)
{
	model->Render(transform, nullptr, isShadow);
}

// レイキャスト
bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::IntersectRayVsModel(start, end, model->GetModelResource(), hit);
}

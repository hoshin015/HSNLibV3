#include "ModelEditorObject.h"
#include "../../../Library/3D/DebugPrimitive.h"

void ModelEditorObject::Update()
{
	if(!model->GetModelResource()->GetAnimationClips().empty())
		UpdateSe();

	// アニメーション更新
	UpdateAnimation();

	// 姿勢行列更新
	UpdateTransform();
}

void ModelEditorObject::Render(bool isShadow)
{
	model->Render(transform, &keyFrame, isShadow);

	DrawDebugPrimitive();
}

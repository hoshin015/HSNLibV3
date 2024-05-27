#include "ModelEditorObject.h"

void ModelEditorObject::Update()
{
	// アニメーション更新
	UpdateAnimation();

	// 姿勢行列更新
	UpdateTransform();
}

void ModelEditorObject::Render(bool isShadow)
{
	model->Render(transform, &keyFrame, isShadow);
}

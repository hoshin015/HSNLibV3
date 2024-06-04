#include "ModelEditorObject.h"
#include "../../../Library/3D/DebugPrimitive.h"

void ModelEditorObject::Update()
{
	// �A�j���[�V�����X�V
	UpdateAnimation();

	// �p���s��X�V
	UpdateTransform();
}

void ModelEditorObject::Render(bool isShadow)
{
	model->Render(transform, &keyFrame, isShadow);

	DrawDebugPrimitive();
}

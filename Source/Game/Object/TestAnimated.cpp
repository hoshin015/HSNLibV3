#include "TestAnimated.h"

void TestAnimated::Update()
{
	position = { 0, 0, 5 };

	// �A�j���[�V�����X�V
	UpdateAnimation();

	// �p���s��X�V
	UpdateTransform();
}

void TestAnimated::Render(bool isShadow)
{
	model->Render(transform, &keyFrame, isShadow);
}

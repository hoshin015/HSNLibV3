#include "TestAnimated.h"

void TestAnimated::Update()
{
	// �A�j���[�V�����X�V
	UpdateAnimation();

	// �p���s��X�V
	UpdateTransform();
}

void TestAnimated::Render(bool isShadow)
{
	model->Render(transform, &keyFrame, isShadow);
}

void TestAnimated::NoAnimRender(bool isShadow)
{
	model->Render(transform, nullptr, isShadow);
}

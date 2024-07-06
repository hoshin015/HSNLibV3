#include "TestAnimated.h"

void TestAnimated::Update()
{
	// アニメーション更新
	UpdateAnimation();

	// 姿勢行列更新
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

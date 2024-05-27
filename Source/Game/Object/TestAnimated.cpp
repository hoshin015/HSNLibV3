#include "TestAnimated.h"

void TestAnimated::Update()
{
	position = { 0, 0, 5 };

	// アニメーション更新
	UpdateAnimation();

	// 姿勢行列更新
	UpdateTransform();
}

void TestAnimated::Render(bool isShadow)
{
	model->Render(transform, &keyFrame, isShadow);
}

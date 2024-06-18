#include "Enemy.h"

void Enemy::Initialize()
{
	PlayAnimation(10, true);
}

void Enemy::Update()
{
	position = { 0, 0, 5 };

	// アニメーション更新
	UpdateAnimation();

	// 姿勢行列更新
	UpdateTransform();
}

void Enemy::Render(bool isShadow)
{
	model->Render(transform, &keyFrame, isShadow);
}

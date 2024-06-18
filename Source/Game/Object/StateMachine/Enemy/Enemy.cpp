#include "Enemy.h"

void Enemy::Initialize()
{
	PlayAnimation(10, true);
}

void Enemy::Update()
{
	position = { 0, 0, 5 };

	// �A�j���[�V�����X�V
	UpdateAnimation();

	// �p���s��X�V
	UpdateTransform();
}

void Enemy::Render(bool isShadow)
{
	model->Render(transform, &keyFrame, isShadow);
}

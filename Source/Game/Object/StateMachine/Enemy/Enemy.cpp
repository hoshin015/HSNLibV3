#include "Enemy.h"
#include "../../../../../External/ImGui/imgui.h"

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

void Enemy::DrawDebugImGui(int number)
{
	ImGui::Begin("Enemy");
	{
		ImGui::DragFloat("emissive", &GetModel()->data.emissivePower, 0.01f);
		ImGui::SliderFloat("roughness", &GetModel()->data.roughnessPower, -1.0f, 1.0f);
		ImGui::SliderFloat("metalness", &GetModel()->data.metalnessPower, -1.0f, 1.0f);
	}
	ImGui::End();
}
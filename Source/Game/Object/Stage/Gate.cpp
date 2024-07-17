#include "Gate.h"

#include "../../../External/ImGui/imgui.h"

#include "../../../Library/Timer.h"

#include "../../Object/StateMachine/Player/Player.h"

void Gate::Initialize()
{
	timer = maxTime;
	complete = false;
	position.y = 0.0f;
}

void Gate::Update()
{
	float elapsedTime = Timer::Instance().DeltaTime();
	bool enterEntrance = Player::Instance().enterEntrance;
	bool enterStage = Player::Instance().enterStage;


	// ステージに入ったら下げる
	if (enterStage)
	{
		position.y -= fallSpeed * elapsedTime;

		if (position.y < 0.0f)
			position.y = 0.0f;
	}

	// 入口に入ったら上げる
	else
	{
		if (enterEntrance && !complete)
		{
			position.y += speed * elapsedTime;
			timer -= elapsedTime;

			if (timer < 0.0f)
			{
				timer = 0.0f;
				complete = true;
			}
		}
	}

	UpdateTransform();
}

void Gate::Render(bool isShadow)
{
	model->Render(transform, &keyFrame, isShadow);
}

void Gate::DrawDebugImGui()
{
	ImGui::Begin(u8"ゲート");

	ImGui::Checkbox(u8"完了", &complete);
	ImGui::DragFloat(u8"タイマー", &timer);
	ImGui::DragFloat(u8"スピード", &speed);
	ImGui::DragFloat(u8"落下速度", &fallSpeed);

	if (ImGui::Button(u8"リセット", { 200.0f, 30.0f }))
	{
		Initialize();
	}

	ImGui::End();
}

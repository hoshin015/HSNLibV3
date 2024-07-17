#include "UiClearAfter.h"

#include "../../External/ImGui/imgui.h"
#include "../../Library/Timer.h"
#include "../Scene/SceneManager.h"

// 初期化
void UiClearAfter::Initialize()
{
	state = static_cast<int>(State::NO);
	clearWaitTimer = 0.0f;
	clearAnimTimer = 0.0f;
	SetAllOffRender();
	clearFlag = false;
}

// 更新
void UiClearAfter::Update()
{
	switch (static_cast<State>(state))
	{
	case State::NO:
		{
			
		}
		break;
	case State::ClearWait:
		{
			float deltaTime = Timer::Instance().DeltaTime();
			clearAfterTimer -= deltaTime;
			clearWaitTimer += deltaTime;

			imgQuestClearText->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, clearWaitTimer, questClearTextInAlpha));
			if(questClearTextOutAlpha.startTime < clearWaitTimer) imgQuestClearText->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, clearWaitTimer, questClearTextOutAlpha));
			imgQuestClearText->SetPosY(Easing::GetNowParam(Easing::OutQuad<float>, clearWaitTimer, questClearTextInPosY));
			if(questClearTextOutPosY.startTime < clearWaitTimer) imgQuestClearText->SetPosY(Easing::GetNowParam(Easing::OutQuad<float>, clearWaitTimer, questClearTextOutPosY));

			// テキスト
			int showTimer = static_cast<int>(clearAfterTimer);
			text = std::to_string(showTimer);

			if(clearAfterTimer < 1.0f)
			{
				// off
				imgQuestClearText->SetIsRender(false);
				imgClearAfterTimer->SetIsRender(false);
				imgText->SetIsRender(false);

				// on
				imgClearText->SetIsRender(true);
				imgClearText->SetScale({ imgClearTextScale.startValue,imgClearTextScale.startValue });
				imgClearText->SetColorA(0.0f);

				imgClearBackBar->SetIsRender(true);
				imgClearBackBar->spriteDissolveConstant.dissolveThreshold = 0.0f;

				imgBlack->SetIsRender(true);
				imgBlack->SetColorA(0.0f);

				state = static_cast<int>(State::ClearAnim);
			}
		}
		break;
	case State::ClearAnim:
		{
			float deltaTime = Timer::Instance().DeltaTime();
			clearAnimTimer += deltaTime;

			imgClearText->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, clearAnimTimer, imgClearTextAlpha));
			float s = Easing::GetNowParam(Easing::OutQuad<float>, clearAnimTimer, imgClearTextScale);
			imgClearText->SetScale({ s,s });
			imgClearText->SetAngle(Easing::GetNowParam(Easing::OutQuad<float>, clearAnimTimer, imgClearTextAngle));

			imgClearBackBar->spriteDissolveConstant.dissolveThreshold = Easing::GetNowParam(Easing::OutQuad<float>, clearAnimTimer, imgClearBackBarDissolveThread);

			imgBlack->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, clearAnimTimer, imgBlackAlpha));

			if(clearAnimTimer > clearAnimTime)
			{
				SceneManager::Instance().ChangeScene(new SceneResult);
			}
		}
		break;
	}
}

// 描画
void UiClearAfter::Render()
{
	imgQuestClearText->Render();
	imgClearAfterTimer->Render();

	imgText->SprTextOut(text, {640, 100});

	imgClearBackBar->Render();
	imgClearText->Render();
	imgBlack->Render();
}

// gui 描画
void UiClearAfter::DrawDebugImGui()
{
	ImGui::Begin("uiClear");
	{
		if(ImGui::Button("OnClearFlag"))
		{
			OnClear();
		}
		ImGui::Checkbox("clearFlag", &clearFlag);
	}
	ImGui::End();
}

// クリアフラグON
void UiClearAfter::OnClear()
{
	clearFlag = true;
	clearAfterTimer = clearAfterTime;

	imgQuestClearText->SetIsRender(true);
	imgClearAfterTimer->SetIsRender(true);
	imgText->SetIsRender(true);

	state = static_cast<int>(State::ClearWait);
}


void UiClearAfter::SetAllOffRender()
{
	for (auto* sprite : sprites)
	{
		sprite->SetIsRender(false);
	}
}

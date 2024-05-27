#include "UiPause.h"
#include "../../Library/Timer.h"
#include "../../Library/Input/InputManager.h"


void UiPause::Initialize()
{
	state = UiPauseState::Hidden;

	pauseText = std::make_unique<Sprite>("Data/Texture/UserInterface/Pause/pauseText.sprite");
	pauseText->UpdateAnimation();

	pauseText1 = std::make_unique<Sprite>("Data/Texture/UserInterface/Pause/pauseText.sprite");
	pauseText1->UpdateAnimation();

	pauseText2 = std::make_unique<Sprite>("Data/Texture/UserInterface/Pause/pauseText.sprite");
	pauseText2->UpdateAnimation();

	pauseImg1 = std::make_unique<Sprite>("Data/Texture/UserInterface/Pause/pauseImg1.sprite");
	pauseImg1->UpdateAnimation();
}

bool UiPause::Update()
{
	switch (state)
	{
	case UiPauseState::Hidden:
		{
			// ポーズフラグがtrueになったら遷移
			if(InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Enter))
			{
				pauseText->SetIsRender(true);
				pauseText1->SetIsRender(true);
				pauseText2->SetIsRender(true);
				pauseImg1->SetIsRender(true);

				isPause = true;
				state = UiPauseState::Showing;
			}
		}
		break;
	case UiPauseState::Showing:
		{
			pauseTimer += Timer::Instance().DeltaTime();

			// 表示中にポーズ解除されたら非表示遷移状態に遷移
			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Enter))
			{
				isPause = false;
				state = UiPauseState::Hide;
				break;
			}

			pauseText->SetPos(GetNowParamVec(Easing::OutQuad<float>, pauseTimer, pauseTextPos));
			pauseText->SetColorA(GetNowParam(Easing::OutQuad<float>, pauseTimer , pauseTextAlpha));

			pauseText1->SetPos(GetNowParamVec(Easing::OutQuad<float>, pauseTimer, pauseTextPos1));
			pauseText1->SetColorA(GetNowParam(Easing::OutQuad<float>, pauseTimer, pauseTextAlpha1));

			pauseText2->SetPos(GetNowParamVec(Easing::OutQuad<float>, pauseTimer, pauseTextPos2));
			pauseText2->SetColorA(GetNowParam(Easing::OutQuad<float>, pauseTimer, pauseTextAlpha2));

			pauseImg1->SetPos(GetNowParamVec(Easing::OutQuad<float>, pauseTimer, pauseImg1Pos));

			// 表示完了したら遷移
			if(pauseTimer > pauseTotalTime)
			{
				pauseTimer = pauseTotalTime;
				state = UiPauseState::Visible;
			}
		}
		break;
	case UiPauseState::Visible:
		{
			// ポーズフラグがfalseになったら遷移
			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Enter))
			{
				isPause = false;
				state = UiPauseState::Hide;
			}
		}
		break;
	case UiPauseState::Hide:
		{
			pauseTimer -= Timer::Instance().DeltaTime();

			// 表示中にポーズ解除されたら非表示遷移状態に遷移
			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Enter))
			{
				isPause = true;
				state = UiPauseState::Showing;
				break;
			}

			pauseText->SetPos(GetNowParamVec(Easing::OutQuad<float>, pauseTimer, pauseTextPos));
			pauseText->SetColorA(GetNowParam(Easing::OutQuad<float>, pauseTimer, pauseTextAlpha));

			pauseText1->SetPos(GetNowParamVec(Easing::OutQuad<float>, pauseTimer, pauseTextPos1));
			pauseText1->SetColorA(GetNowParam(Easing::OutQuad<float>, pauseTimer, pauseTextAlpha1));

			pauseText2->SetPos(GetNowParamVec(Easing::OutQuad<float>, pauseTimer, pauseTextPos2));
			pauseText2->SetColorA(GetNowParam(Easing::OutQuad<float>, pauseTimer, pauseTextAlpha2));

			pauseImg1->SetPos(GetNowParamVec(Easing::OutQuad<float>, pauseTimer, pauseImg1Pos));


			// 非表示完了したら遷移
			if (pauseTimer < 0.0)
			{
				pauseText->SetIsRender(false);
				pauseText1->SetIsRender(false);
				pauseText2->SetIsRender(false);
				pauseImg1->SetIsRender(false);


				pauseTimer = 0.0f;
				state = UiPauseState::Hidden;
				break;
			}
		}
		break;
	}

	pauseImg1->UpdateAnimation();

	return isPause;
}

// 描画
void UiPause::Render()
{
	//if (!isPause) return;

	pauseText->Render();
	pauseText1->Render();
	pauseText2->Render();
	pauseImg1->Render();
}

// イージングによる現在のパラメータ取得
float UiPause::GetNowParam(float time, float startTime, float endTime, float startValue, float endValue)
{
	return Easing::OutQuad(time - startTime, endTime - startTime, endValue, startValue);
}

// イージングによる現在のパラメータ取得
template <typename  Ty>
float UiPause::GetNowParam(EasingFunc<Ty> func, float time, UiEasingValue uiEasingValue)
{
	if (pauseTimer < uiEasingValue.startTime)
	{
		return uiEasingValue.startValue;
	}
	if (pauseTimer > uiEasingValue.endTime)
	{
		return uiEasingValue.endValue;
	}

	return func(time - uiEasingValue.startTime, uiEasingValue.endTime - uiEasingValue.startTime, uiEasingValue.endValue, uiEasingValue.startValue);
}

template<typename Ty>
DirectX::XMFLOAT2 UiPause::GetNowParamVec(EasingFunc<Ty> func, float time, UiEasingValueVec uiEasingValueVec)
{
	if (pauseTimer < uiEasingValueVec.startTime)
	{
		return uiEasingValueVec.startValueVec;
	}
	if(pauseTimer > uiEasingValueVec.endTime)
	{
		return uiEasingValueVec.endValueVec;
	}


	DirectX::XMFLOAT2 vec2 =
	{
		func(time - uiEasingValueVec.startTime, uiEasingValueVec.endTime - uiEasingValueVec.startTime, uiEasingValueVec.endValueVec.x, uiEasingValueVec.startValueVec.x),
		func(time - uiEasingValueVec.startTime, uiEasingValueVec.endTime - uiEasingValueVec.startTime, uiEasingValueVec.endValueVec.y, uiEasingValueVec.startValueVec.y)
	};

	return vec2;
}

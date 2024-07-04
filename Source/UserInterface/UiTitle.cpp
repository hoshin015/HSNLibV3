#include "UiTitle.h"
#include "../../Library/Timer.h"
#include "../../Library/Input/InputManager.h"


void UiTitle::Initialize()
{
	state = UiTitleState::Title;

	imgPressAnyButton = std::make_unique<Sprite>("Data/Texture/Text/PressAnyButton.sprite");
	imgPressAnyButton->UpdateAnimation();
	imgPressAnyButton->SetPos({640, 550});

	imgGameStart = std::make_unique<Sprite>("Data/Texture/Text/GameStart.sprite");
	imgGameStart->UpdateAnimation();
	imgGameStart->SetIsRender(false);

	imgOptions = std::make_unique<Sprite>("Data/Texture/Text/Options.sprite");
	imgOptions->UpdateAnimation();
	imgOptions->SetIsRender(false);

	imgQuit = std::make_unique<Sprite>("Data/Texture/Text/Quit.sprite");
	imgQuit->UpdateAnimation();
	imgQuit->SetIsRender(false);

	imgSelectBar = std::make_unique<Sprite>("Data/Texture/UserInterface/Title/selectBar.sprite");
	imgSelectBar->UpdateAnimation();
	imgSelectBar->SetIsRender(false);
	imgSelectBar->SetPos({100.0f, 100.0});
}

void UiTitle::Update()
{
	switch (state)
	{
	case UiTitleState::Title:
		{
			// ƒ|[ƒYƒtƒ‰ƒO‚ªtrue‚É‚È‚Á‚½‚ç‘JˆÚ
			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Space))
			{
				imgGameStart->SetIsRender(true);
				imgOptions->SetIsRender(true);
				imgQuit->SetIsRender(true);

				state = UiTitleState::TitleToSelectMenu;
			}
		}
		break;
	case UiTitleState::TitleToSelectMenu:
		{
			titleTimer += Timer::Instance().DeltaTime();

			// pressAnyButton
			float _imgPressAnyButtonScale = Easing::GetNowParam(Easing::OutQuad<float>, titleTimer,
			                                                    imgPressAnyButtonScale);
			imgPressAnyButton->SetScale({_imgPressAnyButtonScale, _imgPressAnyButtonScale});
			imgPressAnyButton->SetColorA(
				Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgPressAnyButtonAlpha));

			// imgGameStartPos
			imgGameStart->SetPos(Easing::GetNowParamVec(Easing::OutQuad<float>, titleTimer, imgGameStartPos));
			imgGameStart->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgGameStartAlpha));

			// imgOptions
			imgOptions->SetPos(Easing::GetNowParamVec(Easing::OutQuad<float>, titleTimer, imgOptionsPos));
			imgOptions->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgOptionsAlpha));

			// imgQuit
			imgQuit->SetPos(Easing::GetNowParamVec(Easing::OutQuad<float>, titleTimer, imgQuitPos));
			imgQuit->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgQuitAlpha));


			// •\Ž¦Š®—¹‚µ‚½‚ç‘JˆÚ
			if (titleTimer > titleToSelectMenuTime)
			{
				imgPressAnyButton->SetIsRender(false);

				imgSelectBar->SetIsRender(true);

				selectMenu = static_cast<int>(SelectMenu::GamePlay);

				titleTimer = 0.0f;
				state      = UiTitleState::SelectMenu;
			}
		}
		break;
	case UiTitleState::SelectMenu:
		{
			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Back))
			{
				state = UiTitleState::SelectMenuToTitle;
			}

			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::W))
			{
				selectMenu--;
			}
			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::S))
			{
				selectMenu++;
			}
			selectMenu = (selectMenu + static_cast<int>(SelectMenu::END)) % static_cast<int>(SelectMenu::END);

			switch (static_cast<SelectMenu>(selectMenu))
			{
			case SelectMenu::GamePlay:
				{
					imgSelectBar->SetPos(imgGameStartPos.endValueVec);
				}
				break;
			case SelectMenu::Options:
				{
					imgSelectBar->SetPos(imgOptionsPos.endValueVec);
				}
				break;
			case SelectMenu::Quit:
				{
					imgSelectBar->SetPos(imgQuitPos.endValueVec);
				}
				break;
			}
		}
		break;
	case UiTitleState::SelectMenuToTitle:
		{
		}
		break;
	}
}

// •`‰æ
void UiTitle::Render()
{
	//if (!isPause) return;

	imgPressAnyButton->Render();
	imgGameStart->Render();
	imgOptions->Render();
	imgQuit->Render();
	imgSelectBar->Render();
}

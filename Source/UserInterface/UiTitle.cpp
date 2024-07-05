#include "UiTitle.h"

#include "../../Library/Framework.h"
#include "../../Library/Timer.h"
#include "../../Library/Input/InputManager.h"
#include "../../Library/RegisterNum.h"
#include "../../Library/Particle/Particle.h"


void UiTitle::Initialize()
{
	state = UiTitleState::Title;

	imgPressAnyButton = std::make_unique<Sprite>("Data/Texture/Text/PressAnyButton.sprite");
	imgPressAnyButton->UpdateAnimation();
	imgPressAnyButton->SetPos({640, 550});

	imgBgCover = std::make_unique<Sprite>("Data/Texture/UserInterface/Title/BgCover.png");
	//imgBgCover->UpdateAnimation();
	imgBgCover->SetPos({0, 0});
	imgBgCover->SetIsRender(false);
	//imgBgCover->SetColorA(0.0f);

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


	imgSelectLevel = std::make_unique<Sprite>("Data/Texture/Text/LevelSelect.sprite");
	imgSelectLevel->UpdateAnimation();
	imgSelectLevel->SetIsRender(false);
	imgSelectLevel->SetPos(imgSelectLevelPos);
	imgSelectLevel->SetDissolveTexture(L"Data/Texture/Noise/GrungeMap.png");
	imgSelectLevel->spriteDissolveConstant.edgeThreshold = 1.0f;
	imgSelectLevel->spriteDissolveConstant.edgeColor = { 1.0f, 0.0f, 0.0f, 1.0f };

	imgEasy = std::make_unique<Sprite>("Data/Texture/Text/Easy.sprite");
	imgEasy->UpdateAnimation();
	imgEasy->SetIsRender(false);
	imgEasy->SetPos(imgEasyPos);

	imgNormal = std::make_unique<Sprite>("Data/Texture/Text/normal.sprite");
	imgNormal->UpdateAnimation();
	imgNormal->SetIsRender(false);
	imgNormal->SetPos(imgNormalPos);

	imgHard = std::make_unique<Sprite>("Data/Texture/Text/hard.sprite");
	imgHard->UpdateAnimation();
	imgHard->SetIsRender(false);
	imgHard->SetPos(imgHardPos);
}

void UiTitle::Update()
{
	switch (state)
	{
	case UiTitleState::Title:
		{
			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Space))
			{
				imgBgCover->SetIsRender(true);
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

			// imgBgCover
			imgBgCover->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgBgCoverAlpha));

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

					if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Space))
					{
						imgGameStart->SetIsRender(false);
						imgOptions->SetIsRender(false);
						imgQuit->SetIsRender(false);
						imgSelectBar->SetIsRender(false);

						imgSelectLevel->SetIsRender(true);
						imgEasy->SetIsRender(true);
						imgNormal->SetIsRender(true);
						imgHard->SetIsRender(true);

						// ƒXƒe[ƒW”ñ•`‰æ
						isStageRender = false;

						titleTimer = 0.0f;
						state      = UiTitleState::SelectMenuToLevel;
					}
				}
				break;
			case SelectMenu::Options:
				{
					imgSelectBar->SetPos(imgOptionsPos.endValueVec);

					if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Space))
					{
					}
				}
				break;
			case SelectMenu::Quit:
				{
					imgSelectBar->SetPos(imgQuitPos.endValueVec);

					if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Space))
					{
					}
				}
				break;
			}
		}
		break;
	case UiTitleState::SelectMenuToTitle:
		{
		}
		break;
	case UiTitleState::SelectMenuToLevel:
		{
			titleTimer += Timer::Instance().DeltaTime();

			imgSelectLevel->spriteDissolveConstant.dissolveThreshold = Easing::GetNowParam(
				Easing::OutQuad<float>, titleTimer, imgSelectLevelDissolveThread);

			// •\Ž¦Š®—¹‚µ‚½‚ç‘JˆÚ
			if (titleTimer > selectMenuToSelectLevelTime)
			{
				titleTimer = 0.0f;
				state = UiTitleState::Level;
			}
		}
		break;
	case UiTitleState::Level:
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
	imgBgCover->Render();
	imgGameStart->Render();
	imgOptions->Render();
	imgQuit->Render();
	imgSelectBar->Render();
	imgSelectLevel->Render();
	imgEasy->Render();
	imgNormal->Render();
	imgHard->Render();
}

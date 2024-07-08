#include "UiTitle.h"

#include "../../Library/Framework.h"
#include "../../Library/Timer.h"
#include "../../Library/Input/InputManager.h"
#include "../../Library/RegisterNum.h"
#include "../../Library/Particle/Particle.h"
#include "../../Library/3D/LightManager.h"
#include "../Scene/SceneManager.h"


void UiTitle::Initialize()
{
	state = UiTitleState::Title;
	isStageRender = true;
	isCharacterRender = false;


	imgTitleLogo = std::make_unique<Sprite>("Data/Texture/UserInterface/Title/TitleLogo.png");
	imgTitleLogoSmall = std::make_unique<Sprite>("Data/Texture/UserInterface/Title/TitleLogoSmall.png");
	imgTitleLogoSmall->SetIsRender(false);
	imgTitleLogoSmall->SetColorA(0.0f);

	imgPressAnyButton = std::make_unique<Sprite>("Data/Texture/Text/PressAnyButton.sprite");
	imgPressAnyButton->UpdateAnimation();
	imgPressAnyButton->SetPos({640, 550});

	imgTitleText = std::make_unique<Sprite>("Data/Texture/UserInterface/Title/titleText.sprite");
	imgTitleText->UpdateAnimation();
	imgTitleText->SetPos(imgTitleTextPos);


	imgBgCover = std::make_unique<Sprite>("Data/Texture/UserInterface/Title/BgCover.png");
	//imgBgCover->UpdateAnimation();
	imgBgCover->SetPos({0, 0});
	imgBgCover->SetIsRender(false);
	imgBgCover->SetColorA(0.0f);

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


	imgSelectLevelBgCover = std::make_unique<Sprite>("Data/Texture/UserInterface/Title/levelSelectBgCover.png");
	imgSelectLevelBgCover->SetIsRender(false);
	imgSelectLevelBgCover->SetDissolveTexture(L"Data/Texture/Noise/GrungeMap.png");
	imgSelectLevelBgCover->spriteDissolveConstant.edgeThreshold = 0.0f;
	imgSelectLevelBgCover->spriteDissolveConstant.edgeColor = { 0.2f, 0.2f, 1.0f, 1.0f };

	imgSelectLevel = std::make_unique<Sprite>("Data/Texture/Text/LevelSelect.sprite");
	imgSelectLevel->UpdateAnimation();
	imgSelectLevel->SetIsRender(false);
	imgSelectLevel->SetPos(imgSelectLevelPos);

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
				imgTitleLogoSmall->SetIsRender(true);
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


			// imgTitleLogo
			imgTitleLogo->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgTitleLogoAlpha));
			// imgTitleLogoSmall
			imgTitleLogoSmall->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgTitleLogoSmallAlpha));

			// imgTitleText
			imgTitleText->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgTitleTextAlpha));

			// pressAnyButton
			float _imgPressAnyButtonScale = Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgPressAnyButtonScale);
			imgPressAnyButton->SetScale({_imgPressAnyButtonScale, _imgPressAnyButtonScale});
			imgPressAnyButton->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgPressAnyButtonAlpha));

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
				imgTitleLogo->SetIsRender(false);
				imgTitleText->SetIsRender(false);
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
						imgTitleLogoSmall->SetIsRender(false);
						imgGameStart->SetIsRender(false);
						imgOptions->SetIsRender(false);
						imgQuit->SetIsRender(false);
						imgSelectBar->SetIsRender(false);

						imgSelectLevelBgCover->SetIsRender(true);
						imgSelectLevel->SetIsRender(true);
						imgEasy->SetIsRender(true);
						imgNormal->SetIsRender(true);
						imgHard->SetIsRender(true);

						// ƒXƒe[ƒW”ñ•`‰æ
						isStageRender = false;
						// player•`‰æ
						isCharacterRender = true;

						// ƒ‰ƒCƒgÝ’è
						//LightManager::Instance().Clear();
						//Light* directionLight = new Light(LightType::Directional);
						//directionLight->SetDirection(DirectX::XMFLOAT3(0.5, -1, -1));
						//directionLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
						//LightManager::Instance().Register(directionLight);
						//LightManager::Instance().SetAmbientColor({ 0.2f, 0.2f, 0.2f, 1.0f });

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

			imgSelectLevelBgCover->spriteDissolveConstant.dissolveThreshold = Easing::GetNowParam(
				Easing::OutQuad<float>, titleTimer, imgSelectLevelBgCoverDissolveThread);

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
			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::W))
			{
				selectLevel--;
			}
			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::S))
			{
				selectLevel++;
			}
			selectLevel = (selectLevel + static_cast<int>(SelectLevel::END)) % static_cast<int>(SelectLevel::END);

			switch (static_cast<SelectLevel>(selectLevel))
			{
			case SelectLevel::Easy:
				{
				if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Space))
				{
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTest));
				}
				}
				break;
			case SelectLevel::Normal:
				{
				if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Space))
				{
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTest));
				}
				}
				break;
			case SelectLevel::Hard:
				{
				if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Space))
				{
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTest));
				}
				}
				break;
			}
		}
		break;
	}
}

// •`‰æ
void UiTitle::Render()
{
	//if (!isPause) return;

	imgTitleLogo->Render();
	imgTitleLogoSmall->Render();
	imgTitleText->Render();
	imgPressAnyButton->Render();
	imgBgCover->Render();
	imgGameStart->Render();
	imgOptions->Render();
	imgQuit->Render();
	imgSelectBar->Render();
	imgSelectLevelBgCover->Render();
	imgSelectLevel->Render();
	imgEasy->Render();
	imgNormal->Render();
	imgHard->Render();
}

void UiTitle::DrawDebugImGui()
{
	ImGui::Begin("UiTitle");
	{
		ImGui::SliderFloat("DissolveThread", &imgSelectLevelBgCover->spriteDissolveConstant.dissolveThreshold, 0.0f, 1.0f);
		ImGui::SliderFloat("EdgeThread", &imgSelectLevelBgCover->spriteDissolveConstant.edgeThreshold, 0.0f, 1.0f);
	}
	ImGui::End();
}

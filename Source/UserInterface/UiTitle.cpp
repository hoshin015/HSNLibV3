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
	state = UiTitleState::Init;


	imgBlack = std::make_unique<Sprite>("Data/Texture/Black.png");
	sprites.emplace_back(imgBlack.get());

	imgEnterText = std::make_unique<Sprite>("Data/Texture/Text/enterText.png");
	imgEnterText->SetIsRender(false);
	imgEnterText->SetColorA(0.0f);
	imgEnterText->SetPos(imgEnterTextPos);
	sprites.emplace_back(imgEnterText.get());
	imgBackText = std::make_unique<Sprite>("Data/Texture/Text/backText.png");
	imgBackText->SetIsRender(false);
	imgBackText->SetColorA(0.0f);
	imgBackText->SetPos(imgBackTextPos);
	sprites.emplace_back(imgBackText.get());


	imgTitleLogo = std::make_unique<Sprite>("Data/Texture/UserInterface/Title/TitleLogo.png");
	imgTitleLogo->SetDissolveTexture(L"Data/Texture/Noise/DirtGradient.png");
	sprites.emplace_back(imgTitleLogo.get());
	imgTitleLogoSmall = std::make_unique<Sprite>("Data/Texture/UserInterface/Title/TitleLogoSmall.png");
	imgTitleLogoSmall->SetIsRender(false);
	imgTitleLogoSmall->SetColorA(0.0f);
	sprites.emplace_back(imgTitleLogoSmall.get());

	imgPressAnyButton = std::make_unique<Sprite>("Data/Texture/Text/PressAnyButton.sprite");
	imgPressAnyButton->UpdateAnimation();
	imgPressAnyButton->SetPos({640, 550});
	sprites.emplace_back(imgPressAnyButton.get());

	imgTitleText = std::make_unique<Sprite>("Data/Texture/UserInterface/Title/titleText.sprite");
	imgTitleText->SetDissolveTexture(L"Data/Texture/Noise/DirtGradient2.png");
	imgTitleText->UpdateAnimation();
	imgTitleText->SetPos(imgTitleTextPos);
	sprites.emplace_back(imgTitleText.get());

	imgBgCover = std::make_unique<Sprite>("Data/Texture/UserInterface/Title/BgCover.png");
	imgBgCover->SetPos({0, 0});
	imgBgCover->SetIsRender(false);
	imgBgCover->SetColorA(0.0f);
	sprites.emplace_back(imgBgCover.get());

	imgGameStart = std::make_unique<Sprite>("Data/Texture/Text/GameStart.sprite");
	imgGameStart->UpdateAnimation();
	imgGameStart->SetIsRender(false);
	sprites.emplace_back(imgGameStart.get());

	imgOptions = std::make_unique<Sprite>("Data/Texture/Text/Options.sprite");
	imgOptions->UpdateAnimation();
	imgOptions->SetIsRender(false);
	sprites.emplace_back(imgOptions.get());

	imgQuit = std::make_unique<Sprite>("Data/Texture/Text/Quit.sprite");
	imgQuit->UpdateAnimation();
	imgQuit->SetIsRender(false);
	sprites.emplace_back(imgQuit.get());

	imgSelectBar = std::make_unique<Sprite>("Data/Texture/UserInterface/Title/selectBar.sprite");
	imgSelectBar->UpdateAnimation();
	imgSelectBar->SetIsRender(false);
	imgSelectBar->SetPos({100.0f, 100.0});
	sprites.emplace_back(imgSelectBar.get());


	imgSelectLevelBgCover = std::make_unique<Sprite>("Data/Texture/UserInterface/Title/levelSelectBgCover.png");
	imgSelectLevelBgCover->SetIsRender(false);
	imgSelectLevelBgCover->SetDissolveTexture(L"Data/Texture/Noise/GrungeMap.png");
	imgSelectLevelBgCover->spriteDissolveConstant.edgeThreshold = 0.0f;
	imgSelectLevelBgCover->spriteDissolveConstant.edgeColor = { 0.2f, 0.2f, 1.0f, 1.0f };
	sprites.emplace_back(imgSelectLevelBgCover.get());

	imgSelectLevel = std::make_unique<Sprite>("Data/Texture/Text/LevelSelect.sprite");
	imgSelectLevel->UpdateAnimation();
	imgSelectLevel->SetIsRender(false);
	imgSelectLevel->SetPos(imgSelectLevelPos);
	sprites.emplace_back(imgSelectLevel.get());

	imgEasy = std::make_unique<Sprite>("Data/Texture/Text/Easy.sprite");
	imgEasy->UpdateAnimation();
	imgEasy->SetIsRender(false);
	imgEasy->SetPos(imgEasyPos);
	sprites.emplace_back(imgEasy.get());

	imgNormal = std::make_unique<Sprite>("Data/Texture/Text/normal.sprite");
	imgNormal->UpdateAnimation();
	imgNormal->SetIsRender(false);
	imgNormal->SetPos(imgNormalPos);
	sprites.emplace_back(imgNormal.get());

	imgHard = std::make_unique<Sprite>("Data/Texture/Text/hard.sprite");
	imgHard->UpdateAnimation();
	imgHard->SetIsRender(false);
	imgHard->SetPos(imgHardPos);
	sprites.emplace_back(imgHard.get());
}

void UiTitle::Update()
{
	switch (state)
	{
	case UiTitleState::Init:
		{
			state = UiTitleState::ToTitle1;
		}
		break;
	case UiTitleState::ToTitle1:
		{
			SetAllOffRender();


			imgBlack->SetIsRender(true);
			imgTitleLogo->SetIsRender(true);
			imgTitleLogo->SetColorA(1.0f);
			imgTitleText->SetIsRender(true);
			imgTitleText->SetColorA(1.0f);
			imgPressAnyButton->SetIsRender(true);
			imgPressAnyButton->SetColorA(1.0f);

			// ƒXƒe[ƒW”ñ•`‰æ
			isStageRender = true;
			// player•`‰æ
			isCharacterRender = false;

			titleTimer = 0.0f;

			state = UiTitleState::ToTitle2;
		}
		//[[fallthrough]]
	case UiTitleState::ToTitle2:
		{
			titleTimer += Timer::Instance().DeltaTime();

			imgBlack->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgBlackToTitleAlpha));
			imgTitleLogo->spriteDissolveConstant.dissolveThreshold = Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgTitleLogDissolveThread);
			imgTitleText->spriteDissolveConstant.dissolveThreshold = Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgTitleTextDissolveThread);

			// •\Ž¦Š®—¹‚µ‚½‚ç‘JˆÚ
			if (titleTimer > toTitleTime)
			{
				state = UiTitleState::Title1;
			}
		}
		break;
	case UiTitleState::Title1:
		{
			titleTimer = 0.0f;

			state = UiTitleState::Title2;

		}
		//[[fallthrough]]
	case UiTitleState::Title2:
		{
			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Space))
			{
				state = UiTitleState::TitleToSelectMenu1;
			}
		}
		break;
	case UiTitleState::TitleToSelectMenu1:
		{
			SetAllOffRender();

			imgTitleLogo->SetIsRender(true);
			imgTitleText->SetIsRender(true);
			imgPressAnyButton->SetIsRender(true);

			imgEnterText->SetIsRender(true);
			imgBackText->SetIsRender(true);
			imgTitleLogoSmall->SetIsRender(true);
			imgBgCover->SetIsRender(true);
			imgGameStart->SetIsRender(true);
			imgOptions->SetIsRender(true);
			imgQuit->SetIsRender(true);

			titleTimer = 0.0f;

			state = UiTitleState::TitleToSelectMenu2;
		}
		//[[fallthrough]]
	case UiTitleState::TitleToSelectMenu2:
		{
			titleTimer += Timer::Instance().DeltaTime();

			// imgEnterBackText
			imgEnterText->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgEnterBackTextTitleToSelectMenuAlpha));
			imgBackText->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgEnterBackTextTitleToSelectMenuAlpha));

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
				state      = UiTitleState::SelectMenu1;
			}
		}
		break;
	case UiTitleState::SelectMenu1:
		{
			selectMenu = static_cast<int>(SelectMenu::GamePlay);

			imgSelectBar->SetIsRender(true);
			titleTimer = 0.0f;
			state = UiTitleState::SelectMenu2;
		}
		//[[fallthrough]]
	case UiTitleState::SelectMenu2:
		{
			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Back))
			{
				state = UiTitleState::ToTitle1;
			}

			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::W))
			{
				AudioManager::Instance().PlayMusic(MUSIC_LABEL::SELECT_MOVE, false);
				selectMenu--;
			}
			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::S))
			{
				AudioManager::Instance().PlayMusic(MUSIC_LABEL::SELECT_MOVE, false);
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
						state      = UiTitleState::SelectMenuToLevel1;
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
	case UiTitleState::SelectMenuToLevel1:
		{
			// ƒeƒLƒXƒgƒJƒ‰[Ý’è
			imgEasy->spriteAddColorConstant.addColor = { 0,0.1,0,1 };
			imgNormal->spriteAddColorConstant.addColor = { 0,0,0,1 };
			imgHard->spriteAddColorConstant.addColor = { 0,0,0,1 };

			SetAllOffRender();

			imgBlack->SetIsRender(true);
			imgEnterText->SetIsRender(true);
			imgBackText->SetIsRender(true);

			imgSelectLevelBgCover->SetIsRender(true);
			imgSelectLevel->SetIsRender(true);
			imgEasy->SetIsRender(true);
			imgNormal->SetIsRender(true);
			imgHard->SetIsRender(true);

			// ƒXƒe[ƒW”ñ•`‰æ
			isStageRender = false;
			// player•`‰æ
			isCharacterRender = true;

			titleTimer = 0.0f;
			state = UiTitleState::SelectMenuToLevel2;
		}
		//[[fallthrough]]
	case UiTitleState::SelectMenuToLevel2:
		{
			titleTimer += Timer::Instance().DeltaTime();

			imgBlack->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgBlackSelectMenuToLevelAlpha));

			// imgEnterBackText
			imgEnterText->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgEnterBackTextSelectMenuToLevelAlpha));
			imgBackText->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgEnterBackTextSelectMenuToLevelAlpha));

			imgSelectLevelBgCover->spriteDissolveConstant.dissolveThreshold = Easing::GetNowParam(
				Easing::OutQuad<float>, titleTimer, imgSelectLevelBgCoverDissolveThread);

			// •\Ž¦Š®—¹‚µ‚½‚ç‘JˆÚ
			if (titleTimer > selectMenuToSelectLevelTime)
			{
				state = UiTitleState::Level1;
			}
		}
		break;
	case UiTitleState::Level1:
		{
			titleTimer = 0.0f;
			state = UiTitleState::Level2;
		}
		//[[fallthrough]]
	case UiTitleState::Level2:
		{
			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Back))
			{
				state = UiTitleState::ToTitle1;
			}

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
				imgEasy->spriteAddColorConstant.addColor = { 0,0.1,0,1 };
				imgNormal->spriteAddColorConstant.addColor = { 0,0,0,1 };
				imgHard->spriteAddColorConstant.addColor = { 0,0,0,1 };
				if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Space))
				{
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTest));
				}
				}
				break;
			case SelectLevel::Normal:
				{
				imgEasy->spriteAddColorConstant.addColor = { 0,0,0,1 };
				imgNormal->spriteAddColorConstant.addColor = { 0,0.1,0,1 };
				imgHard->spriteAddColorConstant.addColor = { 0,0,0,1 };
				if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Space))
				{
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTest));
				}
				}
				break;
			case SelectLevel::Hard:
				{
				imgEasy->spriteAddColorConstant.addColor = { 0,0,0,1 };
				imgNormal->spriteAddColorConstant.addColor = { 0,0,0,1 };
				imgHard->spriteAddColorConstant.addColor = { 0,0.1,0,1 };
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

	imgEnterText->Render();
	imgBackText->Render();

	imgBlack->Render();
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

void UiTitle::SetAllOffRender()
{
	for(auto* sprite: sprites)
	{
		sprite->SetIsRender(false);
	}
}

#include "UiTitle.h"

#include "../../Library/Framework.h"
#include "../../Library/Timer.h"
#include "../../Library/Input/InputManager.h"
#include "../../Library/RegisterNum.h"
#include "../../Library/Particle/Particle.h"
#include "../../Library/3D/LightManager.h"
#include "../Scene/SceneManager.h"
#include "../../Library/Particle/EmitterManager.h"


void UiTitle::Initialize()
{
	state = UiTitleState::Init;
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
			imgPressAnyKeyButton->SetIsRender(true);
			imgPressAnyPadButton->SetIsRender(true);
			imgPressAnyKeyButton->SetColorA(1.0f);
			imgPressAnyPadButton->SetColorA(1.0f);

			isEmitterRender = false;

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
			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::F) ||
				InputManager::Instance().GetGamePadButtonPressed(GAMEPADBUTTON_STATE::a))
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
			imgPressAnyKeyButton->SetIsRender(true);
			imgPressAnyPadButton->SetIsRender(true);

			imgEnterKeyText->SetIsRender(true);
			imgEnterPadText->SetIsRender(true);
			imgBackKeyText->SetIsRender(true);
			imgBackPadText->SetIsRender(true);
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
			imgEnterKeyText->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgEnterBackTextTitleToSelectMenuAlpha));
			imgEnterPadText->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgEnterBackTextTitleToSelectMenuAlpha));
			imgBackKeyText->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgEnterBackTextTitleToSelectMenuAlpha));
			imgBackPadText->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgEnterBackTextTitleToSelectMenuAlpha));

			// imgTitleLogo
			imgTitleLogo->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgTitleLogoAlpha));
			// imgTitleLogoSmall
			imgTitleLogoSmall->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgTitleLogoSmallAlpha));

			// imgTitleText
			imgTitleText->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgTitleTextAlpha));

			// pressAnyButton
			float _imgPressAnyButtonScale = Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgPressAnyButtonScale);
			imgPressAnyKeyButton->SetScale({_imgPressAnyButtonScale, _imgPressAnyButtonScale});
			imgPressAnyPadButton->SetScale({_imgPressAnyButtonScale, _imgPressAnyButtonScale});
			imgPressAnyKeyButton->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgPressAnyButtonAlpha));
			imgPressAnyPadButton->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgPressAnyButtonAlpha));

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
			emitterPos = { 80, 100 };

			isEmitterRender = true;
			imgEmitterTop->SetIsRender(true);
			imgSelectBar->SetIsRender(true);

			titleTimer = 0.0f;
			state = UiTitleState::SelectMenu2;
		}
		//[[fallthrough]]
	case UiTitleState::SelectMenu2:
		{
			if (InputManager::Instance().GetMousePressed(MOUSEBUTTON_STATE::rightButton) ||
				InputManager::Instance().GetGamePadButtonPressed(GAMEPADBUTTON_STATE::b))
			{
				state = UiTitleState::ToTitle1;
			}

			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::W) ||
				InputManager::Instance().controllerPressed.y >= 0.9f)
			{
				AudioManager::Instance().PlayMusic(MUSIC_LABEL::SELECT_MOVE, false);
				selectMenu--;
			}
			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::S) ||
				InputManager::Instance().controllerPressed.y <= -0.9f)
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
					emitterTargetPos = { 80, 100 };

					if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::F) ||
						InputManager::Instance().GetGamePadButtonPressed(GAMEPADBUTTON_STATE::a))
					{
						state      = UiTitleState::SelectMenuToLevel1;
					}
				}
				break;
			case SelectMenu::Options:
				{
					imgSelectBar->SetPos(imgOptionsPos.endValueVec);
					emitterTargetPos = { 80, 200 };

					if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::F) ||
						InputManager::Instance().GetGamePadButtonPressed(GAMEPADBUTTON_STATE::a))
					{
						state = UiTitleState::SelectMenuToOption1;
					}
				}
				break;
			case SelectMenu::Quit:
				{
					imgSelectBar->SetPos(imgQuitPos.endValueVec);
					emitterTargetPos = { 80, 300 };

					if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::F) ||
						InputManager::Instance().GetGamePadButtonPressed(GAMEPADBUTTON_STATE::a))
					{
						Framework::Instance().SetGameEnd();
					}
				}
				break;
			}
			EmitUpdate();
		}
		break;
	case UiTitleState::SelectMenuToLevel1:
		{

			SetAllOffRender();

			imgBlack->SetIsRender(true);
			imgEnterKeyText->SetIsRender(true);
			imgEnterPadText->SetIsRender(true);
			imgBackKeyText->SetIsRender(true);
			imgBackPadText->SetIsRender(true);

			imgSelectLevelBgCover->SetIsRender(true);
			imgSelectLevel->SetIsRender(true);
			imgEasy->SetIsRender(true);
			imgNormal->SetIsRender(true);
			imgHard->SetIsRender(true);

			imgEasySelect->SetIsRender(true);

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
			imgEnterKeyText->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgEnterBackTextSelectMenuToLevelAlpha));
			imgEnterPadText->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgEnterBackTextSelectMenuToLevelAlpha));
			imgBackKeyText->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgEnterBackTextSelectMenuToLevelAlpha));
			imgBackPadText->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgEnterBackTextSelectMenuToLevelAlpha));

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
			emitterPos = { 100, 250 };
			isEmitterRender = true;
			imgEmitterTop->SetIsRender(true);

			selectLevel = static_cast<int>(SelectLevel::Easy);
			titleTimer = 0.0f;
			state = UiTitleState::Level2;
		}
		//[[fallthrough]]
	case UiTitleState::Level2:
		{
			if (InputManager::Instance().GetMousePressed(MOUSEBUTTON_STATE::rightButton) ||
				InputManager::Instance().GetGamePadButtonPressed(GAMEPADBUTTON_STATE::b))
			{
				state = UiTitleState::ToTitle1;
			}

			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::W) ||
				InputManager::Instance().controllerPressed.y >= 0.9f)
			{
				emitterMoveTimer = 0.0f;
				selectLevel--;
			}
			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::S) ||
				InputManager::Instance().controllerPressed.y <= -0.9f)
			{
				emitterMoveTimer = 0.0f;
				selectLevel++;
			}
			selectLevel = (selectLevel + static_cast<int>(SelectLevel::END)) % static_cast<int>(SelectLevel::END);

			switch (static_cast<SelectLevel>(selectLevel))
			{
			case SelectLevel::Easy:
				{
				emitterTargetPos = { 100, 250 };

				imgEasySelect->SetIsRender(true);
				imgNormalSelect->SetIsRender(false);
				imgHardSelect->SetIsRender(false);

				if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::F) ||
					InputManager::Instance().GetGamePadButtonPressed(GAMEPADBUTTON_STATE::a))
				{
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTest));
				}
				}
				break;
			case SelectLevel::Normal:
				{
				emitterTargetPos = { 100, 400 };

				imgEasySelect->SetIsRender(false);
				imgNormalSelect->SetIsRender(true);
				imgHardSelect->SetIsRender(false);

				if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::F) ||
					InputManager::Instance().GetGamePadButtonPressed(GAMEPADBUTTON_STATE::a))
				{
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTest));
				}
				}
				break;
			case SelectLevel::Hard:
				{
				emitterTargetPos = { 100, 550 };

				imgEasySelect->SetIsRender(false);
				imgNormalSelect->SetIsRender(false);
				imgHardSelect->SetIsRender(true);

				if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::F) ||
					InputManager::Instance().GetGamePadButtonPressed(GAMEPADBUTTON_STATE::a))
				{
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTest));
				}
				}
				break;
			}
			EmitUpdate();
		}
		break;
	case UiTitleState::SelectMenuToOption1:
		{
			SetAllOffRender();

			imgBackBlack->SetIsRender(true);
			imgBackKeyText->SetIsRender(true);
			imgBackPadText->SetIsRender(true);
			imgBackKeyText->SetPos(imgBackTextPos2);
			imgBackPadText->SetPos(imgBackTextPos2);

			titleTimer = 0.0f;
			state = UiTitleState::SelectMenuToOption2;
		}
	case UiTitleState::SelectMenuToOption2:
		{
			titleTimer += Timer::Instance().DeltaTime();

			imgBackBlack->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgBlackSelectMenuToOptionAlpha));

			imgBackKeyText->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgEnterBackTextSelectMenuToLevelAlpha));
			imgBackPadText->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, titleTimer, imgEnterBackTextSelectMenuToLevelAlpha));

			// •\Ž¦Š®—¹‚µ‚½‚ç‘JˆÚ
			if (titleTimer > selectMenuToSelectLevelTime)
			{
				state = UiTitleState::Option1;
			}
		}
		break;
	case UiTitleState::Option1:
		{
			titleTimer = 0.0f;
			state = UiTitleState::Option2;
		}
	case UiTitleState::Option2:
		{
			if (InputManager::Instance().GetMousePressed(MOUSEBUTTON_STATE::rightButton) ||
				InputManager::Instance().GetGamePadButtonPressed(GAMEPADBUTTON_STATE::b))
			{
				imgBackKeyText->SetPos(imgBackTextPos);
				imgBackPadText->SetPos(imgBackTextPos);

				state = UiTitleState::ToTitle1;
			}
		}
		break;
	}
}

// •`‰æ
void UiTitle::Render()
{
	//if (!isPause) return;

	imgBackBlack->Render();
	imgTitleLogo->Render();
	imgTitleLogoSmall->Render();
	imgTitleText->Render();
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
	imgEasySelect->Render();
	imgNormalSelect->Render();
	imgHardSelect->Render();

	if(InputManager::Instance().IsGamePadConnected())
	{
		imgPressAnyPadButton->Render();
		imgEnterPadText->Render();
		imgBackPadText->Render();
	}
	else
	{
		imgPressAnyKeyButton->Render();
		imgEnterKeyText->Render();
		imgBackKeyText->Render();
	}
	

	imgBlack->Render();


	// sprite
	imgEmitterTop->Render();
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
	isEmitterRender = false;
}

void UiTitle::EmitUpdate()
{
	// emitterTargetPos ‚Ü‚Å‚ÌˆÚ“®
	emitterMoveTimer += Timer::Instance().DeltaTime() * 0.02f;
	if (emitterMoveTimer > 1.0f)emitterMoveTimer = 1.0f;
	DirectX::XMVECTOR EPOS = DirectX::XMLoadFloat2(&emitterPos);
	DirectX::XMVECTOR TargetEPOS = DirectX::XMLoadFloat2(&emitterTargetPos);
	DirectX::XMVECTOR V = DirectX::XMVectorLerp(EPOS, TargetEPOS, emitterMoveTimer);
	DirectX::XMStoreFloat2(&emitterPos, V);
	imgEmitterTop->SetPos(emitterPos);


	uiEmitterTimer += Timer::Instance().DeltaTime();

	while(uiEmitterTimer > uiEmitterTime)
	{
		uiEmitterTimer -= uiEmitterTime;

		Emitter* emitter0 = new Emitter();
		emitter0->position = { emitterPos.x, emitterPos.y , 0 };
		emitter0->emitterData.duration = 5.0;
		emitter0->emitterData.looping = false;
		emitter0->emitterData.burstsTime = 0.1;
		emitter0->emitterData.burstsCount = 1;
		emitter0->emitterData.particleKind = pk_titleSelect;
		emitter0->emitterData.particleLifeTimeMin = 0.25f;
		emitter0->emitterData.particleLifeTimeMax = 0.8f;
		emitter0->emitterData.particleSpeedMin = 0.0f;
		emitter0->emitterData.particleSpeedMax = 0.0f;
		emitter0->emitterData.particleSizeMin = { 5.0f, 10.0f };
		emitter0->emitterData.particleSizeMax = { 20.0f, 20.0f };
		emitter0->emitterData.particleColorMin = { 1.0, 2.0, 2.0, 1 };
		emitter0->emitterData.particleColorMax = { 1.0, 2.0, 2.0, 1 };
		emitter0->emitterData.particleGravity = 1;
		emitter0->emitterData.particleBillboardType = 0;
		emitter0->emitterData.particleTextureType = 0;
		emitter0->emitterData.burstsOneShot = 2;
		EmitterManager::Instance().Register(emitter0);
	}
}

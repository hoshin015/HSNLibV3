#include "UiPause.h"
#include "../../Library/Timer.h"
#include "../../Library/Input/InputManager.h"
#include "../Scene/SceneManager.h"


void UiPause::Initialize()
{
	state = UiPauseState::Hidden;

	isPause = false;

	SetAllOffRender();
	imgBlack->SetColorA(0.0f);
	imgPauseText->SetColorA(0.0f);
	imgResume->SetColorA(0.0f);
	imgGoTitle->SetColorA(0.0f);
	imgResumeGoTitleSelect->SetColorA(0.0f);
}

bool UiPause::Update()
{
	switch (state)
	{
	case UiPauseState::Hidden:
		{
			// �|�[�Y�t���O��true�ɂȂ�����J��
			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Back) ||
				InputManager::Instance().GetGamePadButtonPressed(GAMEPADBUTTON_STATE::start))
			{
				imgBlack->SetIsRender(true);
				imgPauseText->SetIsRender(true);
				imgResume->SetIsRender(true);
				imgGoTitle->SetIsRender(true);
				imgResumeGoTitleSelect->SetIsRender(true);

				checkResumeGoTitleState = static_cast<int>(CheckResumeGoTitle::Resume);

				isPause = true;
				state   = UiPauseState::Showing;
			}
		}
		break;
	case UiPauseState::Showing:
		{
			pauseTimer += Timer::Instance().UnscaledDeltaTime();

			imgBlack->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, pauseTimer, imgBlackAlpha));
			imgPauseText->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, pauseTimer, imgPauseTextAlpha));
			imgResume->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, pauseTimer, imgPauseTextAlpha));
			imgGoTitle->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, pauseTimer, imgPauseTextAlpha));
			imgResumeGoTitleSelect->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, pauseTimer, imgPauseTextAlpha));

			// �\�����Ƀ|�[�Y�������ꂽ���\���J�ڏ�ԂɑJ��
			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Back) ||
				InputManager::Instance().GetGamePadButtonPressed(GAMEPADBUTTON_STATE::start))
			{
				isPause = false;
				state   = UiPauseState::Hide;
				break;
			}

			// �\������������J��
			if (pauseTimer > pauseTotalTime)
			{
				pauseTimer = pauseTotalTime;
				state      = UiPauseState::Visible;
			}
		}
		break;
	case UiPauseState::Visible:
		{
		DirectX::XMFLOAT2 i = InputManager::Instance().controllerPressed;
			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::A) ||
				InputManager::Instance().controllerPressed.x <= -0.9f)
			{
				checkResumeGoTitleState--;
			}
			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::D) ||
				InputManager::Instance().controllerPressed.x >= 0.9f)
			{
				checkResumeGoTitleState++;
			}
			checkResumeGoTitleState = (checkResumeGoTitleState + static_cast<int>(CheckResumeGoTitle::END)) % static_cast<int>(CheckResumeGoTitle::END);

			switch (static_cast<CheckResumeGoTitle>(checkResumeGoTitleState))
			{
			case CheckResumeGoTitle::Resume:
				{
					imgResumeGoTitleSelect->SetPos({ 500, 550 });

					if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Enter) ||
						InputManager::Instance().GetGamePadButtonPressed(GAMEPADBUTTON_STATE::a))
					{
						isPause = false;
						state = UiPauseState::Hide;
					}
				}
				break;
			case CheckResumeGoTitle::GoTitle:
				{
					imgResumeGoTitleSelect->SetPos({ 800, 550 });

					if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Enter) ||
						InputManager::Instance().GetGamePadButtonPressed(GAMEPADBUTTON_STATE::a))
					{
						imgPauseText->SetIsRender(false);
						imgResume->SetIsRender(false);
						imgGoTitle->SetIsRender(false);
						imgResumeGoTitleSelect->SetIsRender(false);

						imgReallyBox->SetIsRender(true);
						imgYesText->SetIsRender(true);
						imgNoText->SetIsRender(true);
						imgYesNoSelect->SetIsRender(true);

						checkYesNoState = static_cast<int>(CheckYesNo::No);

						state = UiPauseState::Check;
					}

				}
				break;
			}
		}
		break;
	case UiPauseState::Hide:
		{
			pauseTimer -= Timer::Instance().UnscaledDeltaTime();

			imgBlack->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, pauseTimer, imgBlackAlpha));
			imgPauseText->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, pauseTimer, imgPauseTextAlpha));
			imgResume->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, pauseTimer, imgPauseTextAlpha));
			imgGoTitle->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, pauseTimer, imgPauseTextAlpha));
			imgResumeGoTitleSelect->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, pauseTimer, imgPauseTextAlpha));

			// �\�����Ƀ|�[�Y�������ꂽ���\���J�ڏ�ԂɑJ��
			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Back))
			{
				isPause = true;
				state   = UiPauseState::Showing;
				break;
			}

			// ��\������������J��
			if (pauseTimer < 0.0)
			{
				imgBlack->SetIsRender(false);
				imgPauseText->SetIsRender(false);
				imgResume->SetIsRender(false);
				imgGoTitle->SetIsRender(false);
				imgResumeGoTitleSelect->SetIsRender(false);

				pauseTimer = 0.0f;
				state      = UiPauseState::Hidden;
				break;
			}
		}
		break;
	case UiPauseState::Check:
		{
			if(InputManager::Instance().GetKeyPressed(DirectX::Keyboard::A) ||
				InputManager::Instance().controllerPressed.x <= -0.9f)
			{
				checkYesNoState--;
			}
			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::D) ||
				InputManager::Instance().controllerPressed.x >= 0.9f)
			{
				checkYesNoState++;
			}
			checkYesNoState = (checkYesNoState + static_cast<int>(CheckYesNo::END)) % static_cast<int>(CheckYesNo::END);

			switch (static_cast<CheckYesNo>(checkYesNoState))
			{
				case CheckYesNo::Yes:
				{
					imgYesNoSelect->SetPos({ 440, 400 });

					if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Enter) ||
						InputManager::Instance().GetGamePadButtonPressed(GAMEPADBUTTON_STATE::a))
					{
						SceneManager::Instance().ChangeScene(new SceneTitle);
						break;
					}
				}
				break;
				case CheckYesNo::No:
				{
					imgYesNoSelect->SetPos({ 840, 400 });

					if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Enter) ||
						InputManager::Instance().GetGamePadButtonPressed(GAMEPADBUTTON_STATE::a))
					{
						imgPauseText->SetIsRender(true);
						imgResume->SetIsRender(true);
						imgGoTitle->SetIsRender(true);
						imgResumeGoTitleSelect->SetIsRender(true);

						imgReallyBox->SetIsRender(false);
						imgYesText->SetIsRender(false);
						imgNoText->SetIsRender(false);
						imgYesNoSelect->SetIsRender(false);
						state = UiPauseState::Visible;
					}
					
				}
				break;
			}


			
		}
		break;
	}

	return isPause;
}

// �`��
void UiPause::Render()
{
	//if (!isPause) return;

	imgBlack->Render();
	imgPauseText->Render();
	imgResume->Render();
	imgGoTitle->Render();
	imgResumeGoTitleSelect->Render();
	imgReallyBox->Render();
	imgYesText->Render();
	imgNoText->Render();
	imgYesNoSelect->Render();
}

void UiPause::SetAllOffRender()
{
	for (auto* sprite : sprites)
	{
		sprite->SetIsRender(false);
	}
}

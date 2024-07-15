#include "UiPause.h"
#include "../../Library/Timer.h"
#include "../../Library/Input/InputManager.h"


void UiPause::Initialize()
{
	state = UiPauseState::Hidden;


}

bool UiPause::Update()
{
	switch (state)
	{
	case UiPauseState::Hidden:
		{
			// �|�[�Y�t���O��true�ɂȂ�����J��
			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Back))
			{

				isPause = true;
				state   = UiPauseState::Showing;
			}
		}
		break;
	case UiPauseState::Showing:
		{
			pauseTimer += Timer::Instance().DeltaTime();

			// �\�����Ƀ|�[�Y�������ꂽ���\���J�ڏ�ԂɑJ��
			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Back))
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
			// �|�[�Y�t���O��false�ɂȂ�����J��
			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Back))
			{
				isPause = false;
				state   = UiPauseState::Hide;
			}
		}
		break;
	case UiPauseState::Hide:
		{
			pauseTimer -= Timer::Instance().DeltaTime();

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

				pauseTimer = 0.0f;
				state      = UiPauseState::Hidden;
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
}

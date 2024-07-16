#include "UiGame.h"

#include "../../External/ImGui/imgui.h"
#include "../../Library/Timer.h"
#include "../../Library/Input/InputManager.h"

#include "../Game/Object/StateMachine/Enemy/Enemy.h"
#include "../Game/Object/StateMachine/Player/Player.h"

// 初期化
void UiGame::Initialize()
{
	somaGauge = std::make_unique<Sprite>("Data/Texture/UserInterface/Game/somaGauge.sprite");
	somaGauge->UpdateAnimation();
	somaGauge->SetPos({ 640, 40 });

	somaGaugeBg = std::make_unique<Sprite>("Data/Texture/UserInterface/Game/somaGaugeBg.sprite");
	somaGaugeBg->UpdateAnimation();
	somaGaugeBg->SetPos({ 640, 40 });

	hpGauge = std::make_unique<Sprite>("Data/Texture/UserInterface/Game/hpGauge.sprite");
	hpGauge->UpdateAnimation();
	hpGauge->SetPos({ 50, 650 });

	hpGaugeBg = std::make_unique<Sprite>("Data/Texture/UserInterface/Game/hpGaugeBg.sprite");
	hpGaugeBg->UpdateAnimation();
	hpGaugeBg->SetPos({ 50, 650 });

	injuryGauge = std::make_unique<Sprite>("Data/Texture/UserInterface/Game/injuryGauge.sprite");
	injuryGauge->UpdateAnimation();
	injuryGauge->SetPos({ 50, 650 });

	skillGauge = std::make_unique<Sprite>("Data/Texture/UserInterface/Game/skillGauge.sprite");
	skillGauge->UpdateAnimation();
	skillGauge->SetPos({ 50, 625 });

	skillGaugeBg = std::make_unique<Sprite>("Data/Texture/UserInterface/Game/skillGaugeBg.sprite");
	skillGaugeBg->UpdateAnimation();
	skillGaugeBg->SetPos({ 50, 625 });

	imgDownOver = std::make_unique<Sprite>("Data/Texture/UserInterface/Game/downOver.sprite");
	imgDownOver->UpdateAnimation();
	imgDownOver->SetColorA(0.0f);
	imgDownOver->SetPosX(640);
	imgDownOver->SetIsRender(false);

	imgBlack = std::make_unique<Sprite>("Data/Texture/Black.png");
	imgBlack->SetColorA(0.0f);
	imgBlack->SetIsRender(false);

	imgKeyOption = std::make_unique<Sprite>("Data/Texture/UserInterface/Game/Keyboard/Option.sprite");
	imgKeyOption->SetPos({ 20,20 });
	imgKeyOption->UpdateAnimation();
	imgKeyCameraMove = std::make_unique<Sprite>("Data/Texture/UserInterface/Game/Keyboard/CameraMove.sprite");
	imgKeyCameraMove->SetPos({ 1260, 350 });
	imgKeyCameraMove->UpdateAnimation();
	imgKeyLockOn = std::make_unique<Sprite>("Data/Texture/UserInterface/Game/Keyboard/LockOn.sprite");
	imgKeyLockOn->SetPos({ 1260, 400 });
	imgKeyLockOn->UpdateAnimation();
	imgKeyMove = std::make_unique<Sprite>("Data/Texture/UserInterface/Game/Keyboard/Move.sprite");
	imgKeyMove->SetPos({ 1260, 550 });
	imgKeyMove->UpdateAnimation();
	imgKeyAttack = std::make_unique<Sprite>("Data/Texture/UserInterface/Game/Keyboard/Attack.sprite");
	imgKeyAttack->SetPos({ 1260, 600 });
	imgKeyAttack->UpdateAnimation();
	imgKeyAvoid = std::make_unique<Sprite>("Data/Texture/UserInterface/Game/Keyboard/Avoid.sprite");
	imgKeyAvoid->SetPos({ 1260, 650 });
	imgKeyAvoid->UpdateAnimation();


	imgConOption = std::make_unique<Sprite>("Data/Texture/UserInterface/Game/Controller/Option.sprite");
	imgConOption->SetPos({ 20,20 });
	imgConOption->UpdateAnimation();
	imgConCameraMove = std::make_unique<Sprite>("Data/Texture/UserInterface/Game/Controller/CameraMove.sprite");
	imgConCameraMove->SetPos({ 1260, 350 });
	imgConCameraMove->UpdateAnimation();
	imgConLockOn = std::make_unique<Sprite>("Data/Texture/UserInterface/Game/Controller/LockOn.sprite");
	imgConLockOn->SetPos({ 1260, 400 });
	imgConLockOn->UpdateAnimation();
	imgConMove = std::make_unique<Sprite>("Data/Texture/UserInterface/Game/Controller/Move.sprite");
	imgConMove->SetPos({ 1260, 550 });
	imgConMove->UpdateAnimation();
	imgConAvoid = std::make_unique<Sprite>("Data/Texture/UserInterface/Game/Controller/Avoid.sprite");
	imgConAvoid->SetPos({ 1260, 600 });
	imgConAvoid->UpdateAnimation();
	imgConAttack = std::make_unique<Sprite>("Data/Texture/UserInterface/Game/Controller/Attack.sprite");
	imgConAttack->SetPos({ 1260, 650 });
	imgConAttack->UpdateAnimation();
}

// 更新
void UiGame::Update()
{
	if(isDownOverFlag)
	{
		switch (static_cast<State>(state))
		{
		case State::BlackUp:
			{
			downTimer += Timer::Instance().DeltaTime();
			imgDownOver->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, downTimer, imgDownOverAlpha));
			imgDownOver->SetPosY(Easing::GetNowParam(Easing::OutQuad<float>, downTimer, imgDownOverPosY));
			imgBlack->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, downTimer, imgBlackAlphaUp));

			if (downTimer > downTime)
			{
				// ここで遷移処理
				imgDownOver->SetIsRender(false);
				imgDownOver->SetColorA(0.0f);

				downTimer = 0.0f;
				state = static_cast<int>(State::BlackDown);
			}
			}
			break;
		case State::BlackDown:
			{
				downTimer += Timer::Instance().DeltaTime();
				imgBlack->SetColorA(Easing::GetNowParam(Easing::OutQuad<float>, downTimer, imgBlackAlphaDown));

				if (downTimer > blackDownTime)
				{
					// ここで遷移処理

					downTimer = 0.0f;
					state = static_cast<int>(State::BlackUp);

					isDownOverFlag = false;
				}
			}
			break;
		}
	}
	auto& enemy = Enemy::Instance();
	auto& player = Player::Instance();
	hp = player.AStatus().hp;
	hpMax = player.CStatus().maxHp;
}

// 描画
void UiGame::Render()
{
	// ====== soma ======
	{
		somaGaugeBg->Render();
		somaGaugeBg->SetScaleX(1);
		somaGaugeBg->Render();
		somaGaugeBg->SetScaleX(-1);

		// 初期値の保存
		float somaSizeX = somaGauge->GetSizeX();
		float somaPivotX = somaGauge->GetPivotPointX();
		// 割合計算
		float somaProportion = soma / static_cast<float>(somaMax);
		somaGauge->SetSizeX(somaSizeX * somaProportion);
		somaGauge->SetPivotPointX(somaSizeX * somaProportion);

		somaGauge->Render();
		somaGauge->SetScaleX(-somaGauge->GetScaleX());
		somaGauge->Render();

		// 次の描画時のために値をリセット
		somaGauge->SetPivotPointX(somaPivotX);	// 中心点リセット
		somaGauge->SetSizeX(somaSizeX);			// サイズリセット
		somaGauge->SetScaleX(1);				// スケールリセット
	}
	


	// ====== hp ======
	{
		float hpSizeX = hpGauge->GetSizeX();
		float hpProportion = hp / static_cast<float>(hpMax);
		hpGauge->SetSizeX(hpSizeX * hpProportion);

		float injurySizeX = injuryGauge->GetSizeX();
		float injuryProportion = injury / static_cast<float>(hpMax);
		injuryGauge->SetSizeX(injurySizeX * injuryProportion);

		injuryGauge->Render();
		hpGauge->Render();
		hpGaugeBg->Render();

		hpGauge->SetSizeX(hpSizeX);
		injuryGauge->SetSizeX(injurySizeX);
		
	}


	// ====== skill ======
	{
		float skillSizeX = skillGauge->GetSizeX();
		float skillProportion = skill / static_cast<float>(skillMax);
		skillGauge->SetSizeX(skillSizeX * skillProportion);

		skillGauge->Render();
		skillGaugeBg->Render();

		skillGauge->SetSizeX(skillSizeX);
	}

	imgDownOver->Render();
	imgBlack->Render();


	if(InputManager::Instance().IsGamePadConnected())
	{
		imgConOption->Render();
		imgConCameraMove->Render();
		imgConLockOn->Render();
		imgConMove->Render();
		imgConAvoid->Render();
		imgConAttack->Render();
	}
	else
	{
		imgKeyOption->Render();
		imgKeyCameraMove->Render();
		imgKeyLockOn->Render();
		imgKeyMove->Render();
		imgKeyAvoid->Render();
		imgKeyAttack->Render();
	}
}

// debugGui
void UiGame::DrawDebugImGui()
{
	ImGui::Begin("UiGame");
	{
		if(ImGui::Button("onDown"))
		{
			OnDown();
		}

		ImGui::SliderInt("soma", &soma, 0, 100);
		ImGui::SliderInt("hp", &hp, 0, 100);
		ImGui::SliderInt("injury", &injury, 0, 100);
		ImGui::SliderInt("skill", &skill, 0, 100);
	}
	ImGui::End();
}

void UiGame::OnDown()
{
	imgDownOver->SetIsRender(true);
	imgDownOver->SetColorA(0.0f);
	imgBlack->SetIsRender(true);
	imgBlack->SetColorA(0.0f);

	state = static_cast<int>(State::BlackUp);
	isDownOverFlag = true;
}

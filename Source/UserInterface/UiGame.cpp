#include "UiGame.h"

#include "../../External/ImGui/imgui.h"

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
}

// 更新
void UiGame::Update()
{
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
}

// debugGui
void UiGame::DrawDebugImGui()
{
	ImGui::Begin("UiGame");
	{
		ImGui::SliderInt("soma", &soma, 0, 100);
		ImGui::SliderInt("hp", &hp, 0, 100);
		ImGui::SliderInt("injury", &injury, 0, 100);
		ImGui::SliderInt("skill", &skill, 0, 100);
	}
	ImGui::End();
}

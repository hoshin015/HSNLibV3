// --- Game ---
#include "StageManager.h"

#include "../../../../External/ImGui/imgui.h"

// 更新処理
void StageManager::Update()
{
	for (Stage* stage : stages)
	{
		stage->Update();
	}
}

// 描画処理
void StageManager::Render(bool isShadow)
{
	for (Stage* stage : stages)
	{
		stage->Render(isShadow);
	}
}

// ステージ登録
void StageManager::Register(Stage* stage)
{
	stages.emplace_back(stage);
}

// ステージ全削除
void StageManager::Clear()
{
	stages.clear();
}

// レイキャスト
bool StageManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	bool result = false;

	hit.distance = FLT_MAX;
	for (Stage* stage : stages)
	{
		HitResult hitResult;
		if (stage->RayCast(start, end, hitResult))
		{
			if (hit.distance > hitResult.distance)
			{
				hit = hitResult;
				result = true;
			}
		}
	}

	return result;
}

void StageManager::DrawDebugImGui()
{
	ImGui::Begin("Stage");
	{
		if (ImGui::TreeNode("Infor"))
		{
			int nodeId = 0;
			for (Stage* stage : stages)
			{
				ImGui::PushID(nodeId++);
				ImGui::DragFloat("emissive", &stage->GetModel()->data.emissivePower, 0.01f);
				ImGui::SliderFloat("roughness", &stage->GetModel()->data.roughnessPower, -1.0f, 1.0f);
				ImGui::SliderFloat("metalness", &stage->GetModel()->data.metalnessPower, -1.0f, 1.0f);
				ImGui::PopID();
			}
			ImGui::TreePop();
		}
	}
	ImGui::End();
}


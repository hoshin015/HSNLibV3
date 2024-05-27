#include "LightManager.h"
#include "../../External/ImGui/imgui.h"
#include "../Graphics/Graphics.h"
#include "../RegisterNum.h"

LightManager::~LightManager()
{
	Clear();
}

// ライト登録
void LightManager::Register(Light* light)
{
	lights.emplace_back(light);
}

// ライト削除
void LightManager::Remove(Light* light)
{
	std::vector<Light*>::iterator	it = std::find(lights.begin(), lights.end(), light);
	if (it != lights.end())
	{
		lights.erase(it);
		delete	light;
	}
}

// ライト全削除
void LightManager::Clear()
{
	for (Light* light : lights)
	{
		delete	light;
	}
	lights.clear();

}

// ライト情報を積む
void LightManager::PushLightData(LightConstants& cBuffer)
{
	cBuffer = {};
	
	// 環境光の情報を追加
	cBuffer.ambientLightColor = ambientColor;

	// 登録されている光源の情報を設定
	for (Light* light : lights)
	{
		light->PushLightData(cBuffer);
	}
}

// 定数バッファの更新
void LightManager::UpdateConstants()
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();
	RenderContext* rc = &RenderContext::Instance();

	PushLightData(rc->lightConstant);

	dc->UpdateSubresource(rc->lightConstantBuffer.Get(), 0, 0, &rc->lightConstant, 0, 0);
	dc->VSSetConstantBuffers(_lightConstant, 1, rc->lightConstantBuffer.GetAddressOf());
	dc->PSSetConstantBuffers(_lightConstant, 1, rc->lightConstantBuffer.GetAddressOf());
}

// デバッグ情報の表示
void LightManager::DrawDebugGui()
{
	if (ImGui::Button("ON"))
	{
		LightManager::Instance().Clear();
		Light* directionLight = new Light(LightType::Directional);
		directionLight->SetDirection(DirectX::XMFLOAT3(0.5, -1, -1));
		directionLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
		LightManager::Instance().Register(directionLight);
		LightManager::Instance().SetAmbientColor({ 0.2f, 0.2f, 0.2f, 1.0f });
	}
	ImGui::SameLine();
	if (ImGui::Button("OFF"))
	{
		LightManager::Instance().Clear();
		Light* light = new Light(LightType::Point);
		light->SetPosition({ 5, 5, 5 });
		light->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
		light->SetRange(30.0f);
		LightManager::Instance().Register(light);
		LightManager::Instance().SetAmbientColor({ 0.1f,0.1f,0.1f,1.0f });
	}
	if (ImGui::TreeNode("Lights"))
	{
		ImGui::ColorEdit3("AmbientColor", &ambientColor.x);
		int nodeId = 0;
		for (Light* light : lights)
		{
			ImGui::PushID(nodeId++);
			light->DrawDebugGUI();
			ImGui::PopID();
		}
		ImGui::TreePop();
	}

}

// デバッグプリミティブ描画
void LightManager::DrawDebugPrimitive()
{
	for (Light* light : lights)
	{
		light->DrawDebugPrimitive();
	}
}



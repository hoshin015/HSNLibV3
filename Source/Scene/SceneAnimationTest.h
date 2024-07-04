#pragma once
#include "Scene.h"
#include "../../External/ImGui/imgui_internal.h"
#include "../../Library/Resource/Model/Animator.h"
#include "../Game/Object/Base/AnimatedModel.h"

class SceneAnimationTest:public Scene {
private:
	struct Transform {
		DirectX::XMFLOAT3 scale;
		DirectX::XMFLOAT4 quaternion;
		DirectX::XMFLOAT3 position;

		DirectX::XMFLOAT4X4 world;

		void UpdateMatrix() {
			using namespace DirectX;
			XMMATRIX S = XMMatrixScaling(scale.x, scale.y, scale.z);
			XMMATRIX R = XMMatrixRotationQuaternion(XMLoadFloat4(&quaternion));
			XMMATRIX T = XMMatrixTranslation(position.x,position.y,position.z);

			XMStoreFloat4x4(&world, S * R * T);
		}

		void Debug() {
			if(ImGui::Begin("Transform")) {
				ImGui::DragFloat3("scale", &scale.x);
				static DirectX::XMFLOAT3 rotate = {0,0,0};
				ImGui::DragFloat3("rotate", &rotate.x,DirectX::XM_PI/180);
				XMStoreFloat4(&quaternion, DirectX::XMQuaternionRotationRollPitchYaw(rotate.x, rotate.y, rotate.z));
				ImGui::DragFloat3("position", &position.x);
			}
			ImGui::End();
		}
	};

private:
	std::shared_ptr<AnimatedModel> _model;
	Transform _transform;
	Animator _animator;

private:
	void DrawDebugGUI();

public:
	SceneAnimationTest() = default;
	~SceneAnimationTest() override = default;

	void Initialize() override;
	void Finalize() override;
	void Update() override;
	void Render() override;
};

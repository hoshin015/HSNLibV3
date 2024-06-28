#include "SceneAnimationTest.h"

#include "../../Library/Timer.h"
#include "../../Library/3D/Camera.h"
#include "../../Library/3D/ResourceManager.h"
#include "../../Library/ImGui/ImGuiManager.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/3D/LightManager.h"
#include "../../Library/Input/InputManager.h"

using namespace DirectX;

void SceneAnimationTest::DrawDebugGUI() {
	DrawMenuBar();
	ImGuiManager::Instance().DisplayPerformanceStats();

	float& rate = _animator.GetParameter<float>("x");
	bool& attack = _animator.GetParameter<bool>("attack");
	bool& end = _animator.GetParameter<bool>("end");
	if (ImGui::Begin("Animator")) {
		ImGui::DragFloat("rate", &rate, 0.001f, 0, 1);
		ImGui::Checkbox("attack", &attack);
		ImGui::Checkbox("end", &end);
	}
	ImGui::End();

	_animator.AnimationEditor();

	Camera::Instance().DrawDebugGui();
	_transform.Debug();
}

void SceneAnimationTest::Initialize() {
	_model = std::make_shared<AnimatedModel>("Data/Fbx/Albino/Albino.model");
	std::vector<ModelResource::Animation>& animations = _model->GetModelResource()->GetAnimationClips();

	Animator::BlendTree blendTree;
	blendTree.motions.emplace_back(Animator::Motion{&animations[6],{0,0}, 1.f });
	blendTree.motions.emplace_back(Animator::Motion{&animations[12],{0.5f,0},1});
	blendTree.motions.emplace_back(Animator::Motion{&animations[9],{1,0},2});
	blendTree.parameters[0] = "x";
	blendTree.maxSeconds = 1.33333f;

	Animator::State move;
	move.object = std::make_shared<Animator::BlendTree>(std::forward<Animator::BlendTree>(blendTree));
	move.type = Animator::State::BLEND_TREE;
	move.transitions.emplace_back(
		TFunc {
			if (bool& attack = animator.GetParameter<bool>("attack")) {
				attack = false;
				return &animator.GetState("attack");
			}
			return nullptr;
		}
	);

	Animator::Motion motion;
	motion.animationSpeed = 1;
	motion.motion = &animations[0];

	Animator::State attack;
	attack.object = std::make_shared<Animator::Motion>(std::forward<Animator::Motion>(motion));
	attack.type = Animator::State::MOTION;
	attack.transitions.emplace_back(
		TFunc {
			if(std::static_pointer_cast<Animator::Motion>(animator.GetState("attack").object)->endMotion) {
				return &animator.GetState("move");
			}
			return nullptr;
		}
	);

	_animator.SetModelSceneView(&_model->GetModelResource()->GetSceneView());
	_animator.SetParameter("x", 0.f);
	_animator.SetParameter("attack", false);
	_animator.SetParameter("end", false);
	_animator.AddState("move",move);
	_animator.AddState("attack",attack);
	_animator.SetEntryState("move");

	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(-7.5f, 8, 12.5f), 
		DirectX::XMFLOAT3(0, 0, 0),  
		DirectX::XMFLOAT3(0, 1, 0)   
	);
	Camera::Instance().SetAngle(
		{ DirectX::XMConvertToRadians(24), DirectX::XMConvertToRadians(147), 0 }
	);
	Camera::Instance().SetTarget({ 0,0,0 });
	Camera::Instance().cameraType = Camera::CAMERA::FREE;

	Light* directionLight = new Light(LightType::Directional);
	directionLight->SetDirection(DirectX::XMFLOAT3(-1, -1, -1));
	directionLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	LightManager::Instance().Register(directionLight);
	LightManager::Instance().SetAmbientColor({ 1.0f, 1.0f, 1.0f, 1.0f });

	_transform.scale = { 0.01f,0.01f,0.01f };
	_transform.position = { 0,0,0 };
	XMStoreFloat4(&_transform.quaternion, XMQuaternionRotationRollPitchYaw(0, 0, 0));
}

void SceneAnimationTest::Finalize() {
	LightManager::Instance().Clear();
}

void SceneAnimationTest::Update() {
#if USE_IMGUI
	ImGuiManager::Instance().Update();
#endif

	InputManager::Instance().Update();
	Camera::Instance().Update();
	_transform.UpdateMatrix();
}

void SceneAnimationTest::Render() {
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	dc->OMSetRenderTargets(1, gfx->GetRTVAddress(), gfx->GetDSV());
	{

		dc->ClearRenderTargetView(gfx->GetRTV(), gfx->GetBgColor());
		dc->ClearDepthStencilView(gfx->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}


	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_CULL_NONE);

	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);

	gfx->SetBlend(BLEND_STATE::NONE);

	Camera::Instance().UpdateCameraConstant();


	LightManager::Instance().UpdateConstants();


	ModelResource::KeyFrame keyFrame = _animator.PlayAnimation(Timer::Instance().DeltaTime());
	_model->Render(_transform.world, &keyFrame , false);

#if USE_IMGUI
	DrawDebugGUI();


	ImGuiManager::Instance().Render();
#endif
}

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

	//_animator.AnimationEditor("move");

	Camera::Instance().DrawDebugGui();
	_transform.Debug();
}

void SceneAnimationTest::Initialize() {
	_model = std::make_shared<AnimatedModel>("Data/Fbx/Albino/Albino.model");
	std::vector<ModelResource::Animation>& animations = _model->GetModelResource()->GetAnimationClips();
//,{ &animations.at(6),&animations.at(12),&animations.at(9) }
	Animator::BlendTree blendTree;
	blendTree.motions.emplace_back(&animations[6],{},1);
	blendTree.motions.emplace_back(&animations[12],{0.5f},1);
	blendTree.motions.emplace_back(&animations[9],{1},2);
	blendTree.timer = 0;
	blendTree.maxSeconds = 1.33333f;

	Animator::Motion motion;
	motion.motion = &animations[1];
	motion.threshold = {};
	motion.animationSpeed = 1;

	_animator.AddAnimation(&_model->GetModelResource()->GetSceneView() ,blendTree,motion);

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

	static float rate = 0;
	ModelResource::KeyFrame keyFrame = _animator.PlayAnimation("move", Timer::Instance().DeltaTime(),rate);
	_model->Render(_transform.world, &keyFrame, false);

#if USE_IMGUI
	DrawDebugGUI();
	if (ImGui::Begin("Animator")) {
		ImGui::DragFloat("rate", &rate, 0.001f, 0, 1);
	}
	ImGui::End();

	ImGuiManager::Instance().Render();
#endif
}

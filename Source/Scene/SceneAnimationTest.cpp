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

	float& rate   = _animator.GetParameter<float>("x");
	bool&  attack = _animator.GetParameter<bool>("attack");
	bool&  end    = _animator.GetParameter<bool>("end");
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
	_model                                            = std::make_shared<AnimatedModel>("Data/Fbx/RootMotionTest/test.model");
	std::vector<ModelResource::Animation>& animations = _model->GetModelResource()->GetAnimationClips();

	// Animator::BlendTree blendTree;
	// blendTree.motions.emplace_back(Animator::Motion{&animations[2],{0,0}, 1.f });
	// blendTree.motions.emplace_back(Animator::Motion{&animations[4],{0,1},1});
	// blendTree.motions.emplace_back(Animator::Motion{&animations[6],{0,-1},1});
	// blendTree.motions.emplace_back(Animator::Motion{&animations[3],{1,0},1});
	// blendTree.motions.emplace_back(Animator::Motion{&animations[5],{-1,0},1});
	// blendTree.parameters[0] = "x";
	// blendTree.parameters[1] = "y";
	// blendTree.maxSeconds = 1.83333f;

	// Animator::State move;
	// move.object = std::make_shared<Animator::BlendTree>(std::forward<Animator::BlendTree>(blendTree));
	// move.type = Animator::State::BLEND_TREE;
	// move.transitions.emplace_back(
	// 	STATE_FUNC(animator) {
	// 		if (bool& attack = animator.GetParameter<bool>("attack")) {
	// 			attack = false;
	// 			return &animator.GetState("attack");
	// 		}
	// 		return nullptr;
	// 	}
	// );

	Animator::Motion motion;
	motion.animationSpeed = 1;
	motion.motion         = &animations[0];

	Animator::State rootMotion;
	rootMotion.object = std::make_shared<Animator::Motion>(std::forward<Animator::Motion>(motion));
	rootMotion.type   = Animator::State::MOTION;
	rootMotion.transitions.emplace_back(
 	STATE_FUNC(animator) {
 		if (bool& attack = animator.GetParameter<bool>("attack")) {
 			attack = false;
 			return &animator.GetState("attack");
 		}
 		return nullptr;
 	}
 );

	Animator::Motion attackMoiton;
	attackMoiton.animationSpeed = 1;
	attackMoiton.motion         = &animations[0];

	Animator::State attack;
	attack.object = Animator::MakeObjPointer(attackMoiton);
	attack.type   = Animator::State::MOTION;
	attack.transitions.emplace_back(
		STATE_FUNC(animator) {
			if (animator.GetState("attack").GetObj<Animator::Motion>()->endMotion) {
				return &animator.GetState("move");
			}
			return nullptr;
		}
	);

	_animator.SetModelSceneView(&_model->GetModelResource()->GetSceneView());
	_animator.SetParameter("x", 0.f);
	_animator.SetParameter("y", 0.f);
	_animator.SetParameter("attack", false);
	_animator.SetParameter("end", false);
	_animator.EnableRootMotion("kosi");
	//_animator.AddState("move",move);
	_animator.AddState("rootMotion", rootMotion);
	_animator.AddState("attack", attack);
	_animator.SetEntryState("rootMotion");

	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(-7.5f, 8, 12.5f),
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 1, 0)
	);
	Camera::Instance().SetAngle(
		{DirectX::XMConvertToRadians(24), DirectX::XMConvertToRadians(147), 0}
	);
	Camera::Instance().SetTarget({0, 0, 0});
	Camera::Instance().cameraType = Camera::CAMERA::FREE;

	Light* directionLight = new Light(LightType::Directional);
	directionLight->SetDirection(DirectX::XMFLOAT3(-1, -1, -1));
	directionLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	LightManager::Instance().Register(directionLight);
	LightManager::Instance().SetAmbientColor({1.0f, 1.0f, 1.0f, 1.0f});

	_transform.scale    = {1.f, 1.f, 1.f};
	_transform.position = {0, 0, 0};
	XMStoreFloat4(&_transform.quaternion, XMQuaternionRotationRollPitchYaw(0, 0, 0));

}

void SceneAnimationTest::Finalize() { LightManager::Instance().Clear(); }

void SceneAnimationTest::Update() {
#if USE_IMGUI
	ImGuiManager::Instance().Update();
#endif


	InputManager& input = InputManager::Instance();

	input.Update();
	_animator.SetParameter("x", input.GetThumSticksLeftX());
	_animator.SetParameter("y", input.GetThumSticksLeftY());

	const XMFLOAT3& vel = _animator.GetVelocity();
	_transform.position.x += vel.x;
	_transform.position.y += vel.y;
	_transform.position.z += vel.z;

	Camera::Instance().Update();
	_transform.UpdateMatrix();
}

void SceneAnimationTest::Render() {
	Graphics*            gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc  = gfx->GetDeviceContext();

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
	_model->Render(_transform.world, &keyFrame, false);

#if USE_IMGUI
	DrawDebugGUI();

	ImGuiManager::Instance().Render();
#endif
}

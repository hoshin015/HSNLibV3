#include <tchar.h>
#include <DirectXMath.h>
// --- external ---
#include "../../External/ImGui/imgui.h"
// --- libarary ---
#include "../../Library/Framework.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Math/OperatorXMFloat3.h"
#include "../../Library/Timer.h"
#include "../../Library/ImGui/ImGuiManager.h"
#include "../../Library/Input/InputManager.h"
#include "../../Library/Effekseer/EffectManager.h"
#include "../../Library/3D/Camera.h"
#include "../../Library/3D/LightManager.h"
#include "../../Library/Particle/Particle.h"
#include "../../Library/Particle/EmitterManager.h"
#include "../../Library/Text/DispString.h"
// --- Scene ---
#include "SceneTest.h"
#include "SceneManager.h"
// --- Game ---
#include "../Game/Object/Stage/StageManager.h"
#include "../Game/Object/Stage/StageMain.h"
// --- UserInterface ---
#include "../../Library/3D/DebugPrimitive.h"
#include "../UserInterface//UiPause.h"
#include "../Game/Object/StateMachine/Enemy/Enemy.h"
#include "../Game/Object/StateMachine/Player/Player.h"
#include "../../Library/Graphics/Shader.h"



void SceneTest::Initialize()
{
	// �J���������ݒ�
	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(0, 20, 20),		// �J�������W
		DirectX::XMFLOAT3(0, 0, 0),			// �^�[�Q�b�g(�ݒ肵�Ă��Ӗ��Ȃ�)
		DirectX::XMFLOAT3(0, 1, 0)			// ������x�N�g��
	);
	Camera::Instance().SetAngle({ DirectX::XMConvertToRadians(45), DirectX::XMConvertToRadians(180), 0 });
	Camera::Instance().cameraType = Camera::CAMERA::TARGET_PLAYER;

	// ���C�g�����ݒ�
	Light* directionLight = new Light(LightType::Directional);
	directionLight->SetDirection(DirectX::XMFLOAT3(0.5, -1, -1));
	directionLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	LightManager::Instance().Register(directionLight);
	LightManager::Instance().SetAmbientColor({ 0.2f, 0.2f, 0.2f, 1.0f });


	// �X�e�[�W������
	StageManager& stageManager = StageManager::Instance();
	StageMain* stageMain = new StageMain("Data/Fbx/ExampleStage/ExampleStage.model");
	stageManager.Register(stageMain);


	bitBlockTransfer = std::make_unique<FullScreenQuad>();
	normalBuffer = std::make_unique<FrameBuffer>(Framework::Instance().GetScreenWidthF(), Framework::Instance().GetScreenHeightF(), true);
	frameBuffer = std::make_unique<FrameBuffer>(Framework::Instance().GetScreenWidthF(), Framework::Instance().GetScreenHeightF(), true);
	bloom = std::make_unique<Bloom>(Framework::Instance().GetScreenWidthF(), Framework::Instance().GetScreenHeightF());
	shadow = std::make_unique<Shadow>();

	wbOitBuffer = std::make_unique<WbOitBuffer>(Framework::Instance().GetScreenWidthF(), Framework::Instance().GetScreenHeightF());

	testStatic = std::make_unique<TestStatic>("Data/Fbx/StaticAlbino/StaticAlbino.model");
	sprTest = std::make_unique<Sprite>("Data/Texture/bomb/bomb.sprite");
	sprTest2 = std::make_unique<Sprite>("Data/Texture/Icon.sprite");
	sprTest2->SetPos({ 200, 100 });
	sprTest3 = std::make_unique<Sprite>("Data/Texture/Nessie.sprite");
	sprTest3->SetPos({ 500, 100 });
	sprTest3->SetScale({ 0.2, 0.2 });
	sprTest3->UpdateAnimation();

	blendTestPlayer = std::make_unique<BlendTestPlayer>("Data/Fbx/BlendTestPlayer/BlendTestPlayer.model");

	Particle::Instance().Initialize();

	Emitter* emitter0 = new Emitter();
	emitter0->position = { 0, 3, 3 };
	emitter0->rate = 9999;
	emitter0->duration = 2;
	emitter0->looping = false;
	emitter0->rateOverTime = 0.5;
	emitter0->startKind = 0;
	emitter0->startLifeTime = 1.0f;
	emitter0->startSize = 0.1f;
	emitter0->startColor = { 1.8,1.8,1.8,1 };
	EmitterManager::Instance().Register(emitter0);
	
	Emitter* emitter1 = new Emitter();
	emitter1->position = { 0, 3, 0 };
	emitter1->rate = 32;
	emitter1->startKind = 1;
	emitter1->rateOverTime = 0.25f;
	emitter1->startLifeTime = 6.0f;
	emitter1->startSize = 0.05f;
	EmitterManager::Instance().Register(emitter1);

	Emitter* emitter2 = new Emitter();
	emitter2->position = { 3, 3, 0 };
	emitter2->rate = 5;
	emitter2->startKind = 2;
	emitter2->startLifeTime = 3.0f;
	emitter2->rateOverTime = 0.5f;
	emitter2->startColor = { 2,0.4,0.4,1 };
	emitter2->startSize = 0.3f;
	EmitterManager::Instance().Register(emitter2);


	UiPause::Instance().Initialize();


	Enemy::Instance().Initialize();
	Player::Instance().Initialize();

	lightningEffect = std::make_unique<LightningMainMesh>("Data/Fbx/normal/lightning1.model");

	CreatePsFromCso("Data/Shader/WbOitPS.cso", wbOitPixelShader.GetAddressOf());
}

void SceneTest::Finalize()
{
	StageManager::Instance().Clear();
	LightManager::Instance().Clear();
	EmitterManager::Instance().Clear();
}

void SceneTest::Update()
{
#if USE_IMGUI
	// --- imguiManager���� ---
	ImGuiManager::Instance().Update();
#endif
	// --- inputManager���� ---
	InputManager::Instance().Update();


	if (UiPause::Instance().Update()) return;


	// --- effectManager���� ---
	EffectManager::Instance().Update();

	// --- �J�������� ---
	Camera::Instance().SetTarget(Player::Instance().GetPos());
	//Camera::Instance().SetTarget(blendTestPlayer->GetPos());
	Camera::Instance().Update();


	// �^�C�}�[�̒萔�o�b�t�@�̍X�V
	UpdateTimerConstnat();

	// �X�e�[�W�X�V
	StageManager::Instance().Update();

	testStatic->Update();

	Enemy::Instance().Update();

	Player::Instance().Update();
	//blendTestPlayer->Update();

	sprTest->SetAngle(sprTest->GetAngle() + 180 * Timer::Instance().DeltaTime());
	sprTest->UpdateAnimation();

	sprTest3->SetAngle(sprTest->GetAngle() + 180 * Timer::Instance().DeltaTime());


	EmitterManager::Instance().Update();
	Particle::Instance().Update();

	if(InputManager::Instance().GetKeyPressed(Keyboard::F2))
	{
		lightningEffect->Initialize();
	}
	lightningEffect->Update();
}

void SceneTest::Render()
{
	// �K�v�ȃ|�C���^�擾
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	// renderTarget�̐ݒ�
	dc->OMSetRenderTargets(1, gfx->GetRTVAddress(), gfx->GetDSV());

	// imGuiBuffer���g�p���Ȃ��ꍇ�͂��������L�q����
	{
		// renderTarget�̃N���A
		dc->ClearRenderTargetView(gfx->GetRTV(), gfx->GetBgColor());
		// depthStencilView�̃N���A
		dc->ClearDepthStencilView(gfx->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	// �J�����̒萔�o�b�t�@�̍X�V
	Camera::Instance().UpdateCameraConstant();
	// ���C�g�̒萔�o�b�t�@�̍X�V
	LightManager::Instance().UpdateConstants();

	

	// shadowMap
	{
		shadow->Clear();					// �V���h�E�}�b�v�N���A
		shadow->UpdateShadowCasterBegin();	// �V���h�E�}�b�v�`�揀��

		for (int i = 0; i < SHADOWMAP_COUNT; i++)
		{
			shadow->Activate(i);
			// �e��t���������f���͂����ŕ`����s��(Render �̈����� true �������)
			{
				// animated object
				shadow->SetAnimatedShader();
				StageManager::Instance().Render(true);
				Enemy::Instance().Render(true);

				Player::Instance().Render(true);
				//blendTestPlayer->Render(true);

				// static object
				shadow->SetStaticShader();
				testStatic->Render(true);
			}
			shadow->DeActivate();
		}

		// �ʏ�`��p�Ƀe�N�X�`���ƒ萔�o�b�t�@�X�V
		shadow->SetShadowTextureAndConstants();
	}

	// rasterizerState�̐ݒ�
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	// depthStencilState�̐ݒ�
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	// blendState�̐ݒ�
	gfx->SetBlend(BLEND_STATE::ALPHA);

	normalBuffer->Clear(gfx->GetBgColor()); 
	normalBuffer->Activate();
	{
		StageManager::Instance().Render();

		testStatic->Render();
		Enemy::Instance().Render();

		Player::Instance().Render();

		Enemy::Instance().DrawDebugPrimitive();
		Player::Instance().DrawDebugPrimitive();
		DebugPrimitive::Instance().Render();
	}
	normalBuffer->DeActivate();


	// rasterizerState�̐ݒ�
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	// depthStencilState�̐ݒ�
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_OFF);
	// blendState�̐ݒ�
	gfx->SetBlend(BLEND_STATE::WBOIT);

	wbOitBuffer->Clear();
	wbOitBuffer->Activate(normalBuffer->depthStencilView.Get());
	{
		// rasterizerState�̐ݒ�
		gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_CULL_NONE);

		Particle::Instance().Render();

		lightningEffect->Render();

		// rasterizerState�̐ݒ�
		gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	}
	wbOitBuffer->DeActivate();

	// rasterizerState�̐ݒ�
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	// depthStencilState�̐ݒ�
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	// blendState�̐ݒ�
	gfx->SetBlend(BLEND_STATE::ALPHA);

	// �ʏ�`��
	float c[4] = { 0,0,0,1 };
	frameBuffer->Clear(c);
	frameBuffer->Activate();
	{

		bitBlockTransfer->blit(normalBuffer->shaderResourceViews->GetAddressOf(), 0, 1);
		ID3D11ShaderResourceView* srvs[2] =
		{
			wbOitBuffer->shaderResourceViews[0].Get(),
			wbOitBuffer->shaderResourceViews[1].Get(),
		};
		bitBlockTransfer->blit(srvs, 0, 2, wbOitPixelShader.Get());
	}
	frameBuffer->DeActivate();

	// blendState�̐ݒ�
	gfx->SetBlend(BLEND_STATE::ALPHA);

#if 1
	// �u���[���������Ă̕`��
	bloom->Make(frameBuffer->shaderResourceViews[0].Get());
	bitBlockTransfer->blit(bloom->GetSrvAddress(), 0, 1, nullptr, nullptr);
#else
	// ���̂܂ܕ`��
	bitBlockTransfer->blit(frameBuffer->shaderResourceViews[0].GetAddressOf(), 0, 1, nullptr, nullptr);
#endif	

	// �u���[���Ȃ�
	//sprTest->Render();
	//sprTest2->Render();
	//sprTest3->Render();

	UiPause::Instance().Render();

#if USE_IMGUI
	// --- �f�o�b�OGUI�`�� ---
	DrawDebugGUI();

	Player::Instance().DrawDebugImGui(0);

	LightManager::Instance().DrawDebugGui();
	bloom->DrawDebugGui();
	shadow->DrawDebugGui();
	wbOitBuffer->DrawDebugGui();

	DispString::Instance().Draw(L"�Ă��Ƃ߂����[��", {200,50},50);

#if SHOW_PERFORMANCE
	// --- �p�t�H�[�}���X�`�� ---
	ImGuiManager::Instance().DisplayPerformanceStats();
#endif

	// --- imguiManager �`�� ---
	ImGuiManager::Instance().Render();
#endif
}

// �f�o�b�O�`��
void SceneTest::DrawDebugGUI()
{
	// ���j���[�o�[�`��
	DrawMenuBar();
}

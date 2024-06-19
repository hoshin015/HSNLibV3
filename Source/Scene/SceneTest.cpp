#include <tchar.h>
#include <DirectXMath.h>
// --- external ---
#include "../../External/ImGui/imgui.h"
// --- libarary ---
#include "../../Library/Framework.h"
#include "../../Library/Graphics/Shader.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Math/OperatorXMFloat3.h"
#include "../../Library/Timer.h"
#include "../../Library/ImGui/ConsoleData.h"
#include "../../Library/ImGui/ImGuiManager.h"
#include "../../Library/Input/InputManager.h"
#include "../../Library/Effekseer/EffectManager.h"
#include "../../Library/3D/Camera.h"
#include "../../Library/3D/DebugPrimitive.h"
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
#include "../Game/Object/StateMachine/Enemy/Enemy.h"
#include "../Game/Object/StateMachine/Player/Player.h"
// --- UserInterface ---
#include "../Game/Object/Effect/LightningEffect.h"
#include "../Game/Object/Effect/RockEffect.h"
#include "../UserInterface//UiPause.h"
#include "../UserInterface/DamageTextManager.h"
#include "../../Library/Math/Math.h"


void SceneTest::Initialize()
{
	// --- �J���������ݒ� ---
	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(0, 20, 20), // �J�������W
		DirectX::XMFLOAT3(0, 0, 0),   // �^�[�Q�b�g(�ݒ肵�Ă��Ӗ��Ȃ�)
		DirectX::XMFLOAT3(0, 1, 0)    // ������x�N�g��
	);
	Camera::Instance().SetAngle({DirectX::XMConvertToRadians(45), DirectX::XMConvertToRadians(180), 0});
	Camera::Instance().cameraType = Camera::CAMERA::TARGET_PLAYER;

	// --- ���C�g�����ݒ� ---
	Light* directionLight = new Light(LightType::Directional);
	directionLight->SetDirection(DirectX::XMFLOAT3(0.5, -1, -1));
	directionLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	LightManager::Instance().Register(directionLight);
	LightManager::Instance().SetAmbientColor({0.2f, 0.2f, 0.2f, 1.0f});


	// --- �X�e�[�W������ ---
	StageManager& stageManager = StageManager::Instance();
	StageMain*    stageMain    = new StageMain("Data/Fbx/ExampleStage/ExampleStage.model");
	stageManager.Register(stageMain);

	// --- buffer �n������ ---
	bitBlockTransfer = std::make_unique<FullScreenQuad>();
	frameBuffer      = std::make_unique<FrameBuffer>(Framework::Instance().GetScreenWidthF(),
	                                            Framework::Instance().GetScreenHeightF(), true);
	bloom = std::make_unique<Bloom>(Framework::Instance().GetScreenWidthF(), Framework::Instance().GetScreenHeightF());
	shadow = std::make_unique<Shadow>();
	wbOitBuffer = std::make_unique<WbOitBuffer>(Framework::Instance().GetScreenWidthF(),
	                                            Framework::Instance().GetScreenHeightF());
	radialBlur = std::make_unique<RadialBlur>(Framework::Instance().GetScreenWidthF(),
	                                          Framework::Instance().GetScreenHeightF());

	// --- AnimatedObject ������ ---
	blendTestPlayer = std::make_unique<BlendTestPlayer>("Data/Fbx/BlendTestPlayer/BlendTestPlayer.model");

	// --- StaticObject ������ ---
	testStatic = std::make_unique<TestStatic>("Data/Fbx/StaticAlbino/StaticAlbino.model");

	// --- Sprite ������ ---
	sprTest  = std::make_unique<Sprite>("Data/Texture/bomb/bomb.sprite");
	sprTest2 = std::make_unique<Sprite>("Data/Texture/Icon.sprite");
	sprTest2->SetPos({200, 100});
	sprTest3 = std::make_unique<Sprite>("Data/Texture/Nessie.sprite");
	sprTest3->SetPos({500, 100});
	sprTest3->SetScale({0.2, 0.2});
	sprTest3->UpdateAnimation();


	Enemy::Instance().Initialize();
	Player::Instance().Initialize();


	Particle::Instance().Initialize();

	// --- Emitter �o�^ ---
	Emitter* emitter0                           = new Emitter();
	emitter0->position                          = {0, 3, 3};
	emitter0->emitterData.duration              = 2.0;
	emitter0->emitterData.looping               = false;
	emitter0->emitterData.burstsTime            = 0.5;
	emitter0->emitterData.burstsCount           = 128;
	emitter0->emitterData.particleKind          = 0;
	emitter0->emitterData.particleLifeTimeMin   = 1.0f;
	emitter0->emitterData.particleLifeTimeMax   = 1.0f;
	emitter0->emitterData.particleSpeedMin      = -5.0f;
	emitter0->emitterData.particleSpeedMax      = 5.0f;
	emitter0->emitterData.particleSizeMin       = {0.1f, 0.1f};
	emitter0->emitterData.particleSizeMax       = {1.0f, 1.0f};
	emitter0->emitterData.particleColorMin      = {0.0, 0.0, 0.0, 1};
	emitter0->emitterData.particleColorMax      = {1.2, 1.2, 1.2, 1};
	emitter0->emitterData.particleFrictionMin   = 0;
	emitter0->emitterData.particleFrictionMax   = 1;
	emitter0->emitterData.particleGravity       = 1;
	emitter0->emitterData.particleBillboardType = 0;
	emitter0->emitterData.particleTextureType   = 0;
	EmitterManager::Instance().Register(emitter0);

	UiPause::Instance().Initialize();

	LightningEffect::Instance().Initialize();
	RockEffect::Instance().Initialize();
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

	DamageTextManager::Instance().Update();

	EmitterManager::Instance().Update();
	Particle::Instance().Update();

	// �e�X�g�G�~�b�^�[
	if (InputManager::Instance().GetKeyPressed(Keyboard::F1))
	{
		for(int i = 0; i < 10; i++)
		{
			DirectX::XMFLOAT3 rPos = { (rand() % 10) - 5.0f, 0, rand() % 10 - 5.0f };
			DirectX::XMFLOAT3 rVec = { (rand() % 10) - 5.0f, 5, rand() % 10 - 5.0f };
			RockEffect::Instance().Emit(rPos, rVec,{0.1, 1});
		}
	}
	if (InputManager::Instance().GetKeyPressed(Keyboard::F2))
	{
		for (int i = 0; i < 10; i++)
		{
			DirectX::XMFLOAT3 rPos = { (rand() % 10) - 5.0f, 0, rand() % 10 - 5.0f };
			DirectX::XMFLOAT3 rVec = { (rand() % 10) - 5.0f, 5, rand() % 10 - 5.0f };

			RockData* rock = new RockData();
			rock->SetPos(rPos);
			rock->SetVeloicty(rVec);
			rock->SetLifeTime(10.0f);
			rock->SetEmissivePower(0.0f);
			float rScale = Math::RandomRange(0.25, 0.5);
			rock->SetScale({ rScale, rScale, rScale });
			float rX = rand() % 360;
			float rY = rand() % 360;
			float rZ = rand() % 360;
			rock->SetAngle({ rX, rY, rZ });
			rock->SetColor({ 1.0, 1.0, 1.0, 1 });
			rock->SetUpdateType(RockData::RockFuncEnum::Up);
			RockEffect::Instance().rockMesh1->Register(rock);
		}
	}

	if (InputManager::Instance().GetKeyPressed(Keyboard::F3))
	{
		DirectX::XMFLOAT3 rPos = { (rand() % 10) - 5.0f, 0, rand() % 10 - 5.0f };
		LightningEffect::Instance().Emit(rPos);
	}
	LightningEffect::Instance().Update();
	RockEffect::Instance().Update();
}

void SceneTest::Render()
{
	// �K�v�ȃ|�C���^�擾
	Graphics*            gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc  = gfx->GetDeviceContext();

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

	// ====== shadowMap ======
	{
		shadow->Clear();                   // �V���h�E�}�b�v�N���A
		shadow->UpdateShadowCasterBegin(); // �V���h�E�}�b�v�`�揀��

		for (int i = 0; i < SHADOWMAP_COUNT; i++)
		{
			shadow->Activate(i);
			// �e��t���������f���͂����ŕ`����s��(Render �̈����� true �������)
			{
				// --- animated object ---
				shadow->SetAnimatedShader(); // animated object �̉e�`��J�n
				StageManager::Instance().Render(true);
				Enemy::Instance().Render(true);

				Player::Instance().Render(true);
				//blendTestPlayer->Render(true);

				// --- static object ---
				shadow->SetStaticShader(); // static object �̉e�`��J�n

				RockEffect::Instance().Render(true);
				//testStatic->Render(true);
			}
			shadow->DeActivate();
		}

		// �ʏ�`��p�Ƀe�N�X�`���ƒ萔�o�b�t�@�X�V
		shadow->SetShadowTextureAndConstants();
	}

	// ====== �s�����`�� ======
	frameBuffer->Clear(gfx->GetBgColor());
	frameBuffer->Activate();
	{
		gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
		gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
		gfx->SetBlend(BLEND_STATE::ALPHA);

		// �����ɕs�����I�u�W�F�N�g�̕`��
		StageManager::Instance().Render();

		//testStatic->Render();
		Enemy::Instance().Render();

		Player::Instance().Render();

		RockEffect::Instance().Render();

		if (showCollision)
		{
			Enemy::Instance().DrawDebugPrimitive();
			Player::Instance().DrawDebugPrimitive();
		}
		DebugPrimitive::Instance().Render();
	}
	frameBuffer->DeActivate();

	// ====== �������`�� ======
	wbOitBuffer->Clear();
	wbOitBuffer->Activate(frameBuffer->depthStencilView.Get());
	{
		gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);

		// �����ɔ������I�u�W�F�N�g�̕`��
		Particle::Instance().Render();

		gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_CULL_NONE);
		LightningEffect::Instance().Render();
	}
	wbOitBuffer->DeActivate();

	// ====== �s�����E�������̓��� ======
	frameBuffer->Activate();
	{
		gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
		gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
		gfx->SetBlend(BLEND_STATE::ALPHA);

		ID3D11ShaderResourceView* srvs[2] =
		{
			wbOitBuffer->shaderResourceViews[_accumTexture].Get(),
			wbOitBuffer->shaderResourceViews[_revealTexture].Get(),
		};
		bitBlockTransfer->blit(srvs, 0, ARRAYSIZE(srvs), wbOitBuffer->GetWbOitPS());
	}
	frameBuffer->DeActivate();

	// ====== ���W�A���u���[ ======
	if(radialBlur->GetIsRadial())
	{
		radialBlur->Make(frameBuffer->shaderResourceViews[0].Get());

		// ====== �u���[���������Ă̕`�� ======
		bloom->Make(radialBlur->GetSrv());
		ID3D11ShaderResourceView* shvs[2] =
		{
			radialBlur->GetSrv(),
			bloom->GetSrv()
		};
		bitBlockTransfer->blit(shvs, 0, 2, bloom->GetFinalPassPs());
	}
	else
	{
		// ====== �u���[���������Ă̕`�� ======
		bloom->Make(frameBuffer->shaderResourceViews[0].Get());
		ID3D11ShaderResourceView* shvs[2] =
		{
			frameBuffer->shaderResourceViews[0].Get(),
			bloom->GetSrv()
		};
		bitBlockTransfer->blit(shvs, 0, 2, bloom->GetFinalPassPs());
	}

	// ======�@�u���[���Ȃ��̕`��@======�@

	// �����ŃX�v���C�g�`��
	//sprTest->Render();
	//sprTest2->Render();
	//sprTest3->Render();


	//UiPause::Instance().Render();

	// �����ŕ����`��
	DamageTextManager::Instance().Render();

	// �e�L�X�g�f�[�^�̕����`��
	//DispString::Instance().Draw(L"�Ă��Ƃ߂����[��", {200, 50}, 50);


#if USE_IMGUI
	// --- �f�o�b�OGUI�`�� ---
	DrawDebugGUI();

	ImGuiManager::Instance().Console();

	Player::Instance().DrawDebugImGui(0);

	LightManager::Instance().DrawDebugGui();
	bloom->DrawDebugGui();
	shadow->DrawDebugGui();
	wbOitBuffer->DrawDebugGui();
	radialBlur->DrawDebugGui();

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

	ImGui::Begin("TestScene");
	{
		ImGui::Checkbox("collision", &showCollision);
	}
	ImGui::End();
}

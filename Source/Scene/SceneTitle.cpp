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
// --- Scene ---
#include "SceneTitle.h"
#include "SceneManager.h"
// --- Game ---
#include "../Game/Object/Stage/StageManager.h"
#include "../Game/Object/Stage/StageMain.h"
// --- UserInterface ---
#include "../UserInterface/UiTitle.h"
#include "../../Library/3D/LightManager.h"

void SceneTitle::Initialize()
{
	Framework* frameWork = &Framework::Instance();

	// --- skyMap ������ ---
	skyMap = std::make_unique<SkyMap>(L"Data/Texture/winter_evening_4k.DDS");

	// --- �J���������ݒ� ---
	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(0, 7, 20), // �J�������W
		DirectX::XMFLOAT3(0, 5, 0),   // �^�[�Q�b�g
		DirectX::XMFLOAT3(0, 1, 0)    // ������x�N�g��
	);
	Camera::Instance().cameraType = Camera::CAMERA::FREE;

	// --- ���C�g�����ݒ� ---
	Light* directionLight = new Light(LightType::Directional);
	directionLight->SetDirection(DirectX::XMFLOAT3(0.5, -1, -1));
	directionLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	LightManager::Instance().Register(directionLight);
	LightManager::Instance().SetAmbientColor({0.2f, 0.2f, 0.2f, 1.0f});

	// --- �X�e�[�W������ ---
	StageManager& stageManager = StageManager::Instance();
	StageMain*    stageMain    = new StageMain("Data/Fbx/StageMain/StageMain.model");
	stageManager.Register(stageMain);
	StageMain* StageFence = new StageMain("Data/Fbx/StageFence/StageFence.model");
	stageManager.Register(StageFence);

	// player
	titlePlayer = std::make_unique<TestAnimated>("Data/Fbx/TitlePlayer/TitlePlayer.model");
	titlePlayer->SetPos({ -2, 3, 5 });
	titlePlayer->PlayAnimation(1, true);

	// titleFloor
	titleFloor = std::make_unique<TestAnimated>("Data/Fbx/Floor/Floor.model");
	titleFloor->SetPos({ -2, 3, 5 });
	titleFloor->SetScale({ 40, 40, 40 });

	frameBuffer = std::make_unique<FrameBuffer>(frameWork->GetScreenWidthF(), frameWork->GetScreenHeightF());

	UiTitle::Instance().Initialize();
}

void SceneTitle::Finalize()
{
	StageManager::Instance().Clear();
	LightManager::Instance().Clear();
}

void SceneTitle::Update()
{
#if USE_IMGUI
	// --- imguiManager���� ---
	ImGuiManager::Instance().Update();
#endif
	// --- inputManager���� ---
	InputManager::Instance().Update();

	// --- effectManager���� ---
	EffectManager::Instance().Update();

	// --- �J�������� ---
	//Camera::Instance().Update();

	// �^�C�}�[�̒萔�o�b�t�@�̍X�V
	UpdateTimerConstnat();

	// �X�e�[�W�X�V
	StageManager::Instance().Update();

	// player
	titlePlayer->Update();
	// titleFloor
	titleFloor->Update();


	if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Enter))
	{
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTest));
	}

	UiTitle::Instance().Update();

#if USE_IMGUI
#endif
}

void SceneTitle::Render()
{
	// �K�v�ȃ|�C���^�擾
	Graphics*            gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc  = gfx->GetDeviceContext();

	// renderTarget�̐ݒ�
	dc->OMSetRenderTargets(1, gfx->GetRTVAddress(), gfx->GetDSV());

	// imGuiBuffer���g�p���Ȃ��ꍇ�͂��������L�q����
	{
		float bgcolor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
		// renderTarget�̃N���A
		dc->ClearRenderTargetView(gfx->GetRTV(), bgcolor);
		// depthStencilView�̃N���A
		dc->ClearDepthStencilView(gfx->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	// �J�����̒萔�o�b�t�@�̍X�V
	Camera::Instance().UpdateCameraConstant();
	// ���C�g�̒萔�o�b�t�@�̍X�V
	LightManager::Instance().UpdateConstants();


	// rasterizerState�̐ݒ�
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	// depthStencilState�̐ݒ�
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	// blendState�̐ݒ�
	gfx->SetBlend(BLEND_STATE::ALPHA);


	// �����ɕs�����I�u�W�F�N�g�̕`��
	if (UiTitle::Instance().GetIsStageRender())
	{
		StageManager::Instance().Render();
		skyMap->Render();
	}
	if (UiTitle::Instance().GetIsCharacterRender())
	{
		titlePlayer->Render();
		titleFloor->NoAnimRender();
	}


	// UI �`��
	UiTitle::Instance().Render();

#if USE_IMGUI
	// --- �f�o�b�O�`�� ---
	DrawDebugGUI();
	LightManager::Instance().DrawDebugGui();

#if SHOW_PERFORMANCE
	// --- �p�t�H�[�}���X�`�� ---
	ImGuiManager::Instance().DisplayPerformanceStats();
#endif

	// --- imguiManager �`�� ---
	ImGuiManager::Instance().Render();
#endif
}

// �f�o�b�O�`��
void SceneTitle::DrawDebugGUI()
{
	// ���j���[�o�[�`��
	DrawMenuBar();
}

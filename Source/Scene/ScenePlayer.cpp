#include "ScenePlayer.h"

#include "../../Library/3D/Camera.h"
#include "../../Library/3D/LightManager.h"
#include "../Game/Object/Stage/StageManager.h"
#include "../Game/Object/Stage/StageMain.h"
#include "../../Library/Input/InputManager.h"
#include "../../Library/ImGui/ImGuiManager.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Source/Game/Object/Player/PlayerManager.h"
#include "../../Library/3D/LineRenderer.h"

void ScenePlayer::Initialize()
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

	//�v���C���[������
	PlayerManager& playerManager = PlayerManager::Instance();
	Player* player1 = new Player("Data/Fbx/Jummo/Jummo.model",false);
	playerManager.Register(player1);
	Player* player2 = new Player("Data/Fbx/Jummo/Jummo.model",true);
	player2->SetPosX(10.0f);
	playerManager.Register(player2);
}

void ScenePlayer::Finalize()
{
	StageManager::Instance().Clear();
	LightManager::Instance().Clear();
	PlayerManager::Instance().Clear();
}

void ScenePlayer::Update()
{
#if USE_IMGUI
	// --- imguiManager���� ---
	ImGuiManager::Instance().Update();
#endif
	// --- inputManager���� ---
	InputManager::Instance().Update();
	// --- �J�������� ---
	Camera::Instance().SetTarget(PlayerManager::Instance().GetPositionCenter());
	Camera::Instance().Update();
	// �X�e�[�W�X�V
	StageManager::Instance().Update();
	//�v���C���[�X�V
	PlayerManager::Instance().Update();


}

void ScenePlayer::Render()
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

	// �}�b�v�̕`��
	StageManager::Instance().Render();
	//�v���C���[�̕`��
	PlayerManager::Instance().Render();
	//���̕`��
	LineRenderer::Instance().Render();

#if USE_IMGUI
	// --- �f�o�b�OGUI�`�� ---
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

void ScenePlayer::DrawDebugGUI()
{
	// ���j���[�o�[�`��
	DrawMenuBar();
	PlayerManager::Instance().DrawDebugImGui();
}

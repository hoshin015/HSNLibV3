#pragma once
#include <memory>
// --- Library ---
#include "../../Library/2D/Sprite.h"
#include "../../Library/3D/SkyMap.h"
#include "../../Library/Graphics/FrameBuffer.h"
#include "../../Library/Graphics/FullScreenQuad.h"
#include "../../Library/Graphics/Bloom.h"
#include "../../Library/Graphics/Shadow.h"
#include "../../Library/Graphics/WbOitBuffer.h"
#include "../../Library/Graphics/RadialBlur.h"
#include "../../Library/Graphics/HeatHaze.h"
#include "../../Library/Graphics/ColorFilter.h"
// --- Scene ---
#include "Scene.h"
// --- Game ---
#include "../Game/Object/TestStatic.h"
#include "../Game/Object/StateMachine/BlendTestPlayer/BlendTestPlayer.h"
#include "../Game/Object/Effect/Lightning/LightningMainMesh.h"
// --- Camera ---
#include "../Camera/CameraDerived.h"
#include "../../Library/3D/CameraManager.h"


// ���U���g�V�[��
class SceneResult : public Scene
{
public:
	SceneResult() {}
	~SceneResult() override {};

	// ������
	void Initialize() override;

	// �I����
	void Finalize() override;

	// �X�V
	void Update() override;

	// �`��
	void Render() override;

private:
	// �f�o�b�O�`��
	void DrawDebugGUI();

private:
	std::unique_ptr<Sprite> sprBackground;

};
#pragma once
#include <memory>
// --- Library ---
#include "../../Library/2D/Sprite.h"
#include "../../Library/Graphics/FullScreenQuad.h"
#include "../../Library/Graphics/FrameBuffer.h"
#include "../../Library/Graphics/WbOitBuffer.h"
#include "../../Library/3D/SkyMap.h"
// --- Scene ---
#include "Scene.h"
#include "../../Library/Video/Video.h"
#include "../Game/Object/TestAnimated.h"


class TestAnimated;

// �^�C�g���V�[��
class SceneTitle : public Scene
{
public:
	SceneTitle() {}
	~SceneTitle() override {};

	// ������
	void Initialize() override;

	// �I����
	void Finalize() override;

	// �X�V
	void Update() override;

	// �`��
	void Render() override;

	// �f�o�b�O�`��
	void DrawDebugGUI();

private:
	std::unique_ptr<FullScreenQuad> bitBlockTransfer;
	std::unique_ptr<FrameBuffer> frameBuffer;
	std::unique_ptr<WbOitBuffer> wbOitBuffer;
	std::unique_ptr<TestAnimated> titlePlayer;
	std::unique_ptr<TestAnimated> titleFloor;

	std::unique_ptr<SkyMap> skyMap;

};

#pragma once
#include <memory>
// --- Library ---
#include "../../Library/2D/Sprite.h"
#include "../../Library/Graphics/FrameBuffer.h"
#include "../../Library/Graphics/FullScreenQuad.h"
#include "../../Library/Graphics/Bloom.h"
#include "../../Library/Graphics/Shadow.h"
// --- Scene ---
#include "Scene.h"
// --- Game ---
#include "../Game/Object/TestStatic.h"
#include "../Game/Object/TestAnimated.h"



// �e�X�g�V�[��
class SceneTest : public Scene
{
public:
	SceneTest() {}
	~SceneTest() override {};

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
	std::unique_ptr<FrameBuffer> frameBuffer;
	std::unique_ptr<FullScreenQuad> bitBlockTransfer;
	std::unique_ptr<Bloom> bloom;
	std::unique_ptr<Shadow> shadow;

	std::unique_ptr<TestStatic> testStatic;
	std::unique_ptr<TestAnimated> testAnimated;
	std::unique_ptr<Sprite> sprTest;
	std::unique_ptr<Sprite> sprTest2;
	std::unique_ptr<Sprite> sprTest3;
};
#pragma once
#include <memory>
// --- Library ---
#include "../../Library/2D/Sprite.h"
#include "../../Library/Graphics/FrameBuffer.h"
// --- Scene ---
#include "Scene.h"

// �X�v���C�g�G�f�B�^�[�V�[��
class SceneSpriteEditor : public Scene
{
public:
	SceneSpriteEditor() {}
	~SceneSpriteEditor() override {};

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
	// �X�v���C�g�G�f�B�^�[�p���j���[�o�[�`��
	void DrawSpriteEditorMenuBar();
private:
	bool isMouseHoverSceneView = false;				// �}�E�X���V�[���r���[��ɂ��邩
	std::unique_ptr<FrameBuffer> imGuiFrameBuffer;	// ImGui�`��p�t���[���o�b�t�@
	std::unique_ptr<FrameBuffer> imGuiFrameBuffer2;	// ImGui�`��p�t���[���o�b�t�@

	std::unique_ptr<Sprite> spr1;

	// �I�𒆂̃A�j���[�V����
	int selected = 0;
};
#pragma once
#include <thread>
#include "Scene.h"

#include "../../Library/Graphics/FrameBuffer.h"
#include "../../Library/Graphics/FullScreenQuad.h"
#include "../../Library/Graphics/Bloom.h"
#include "../../Library/Graphics/WbOitBuffer.h"
#include "../../Library/Graphics/ColorFilter.h"

#include "../../Library/2D/Sprite.h"

// �^�C�g���V�[��
class SceneLoading : public Scene
{
public:
	SceneLoading(Scene* nextScene) : nextScene(nextScene){}
	~SceneLoading() override {};

	// ������
	void Initialize() override;

	// �I����
	void Finalize() override;

	// �X�V
	void Update() override;

	// �`��
	void Render() override;

	void EmitUpdate();

private:
	// ���[�f�B���O�X���b�h
	static void LoadingThread(SceneLoading* scene);

private:
	Scene* nextScene = nullptr;
	std::thread* thread = nullptr;

	std::unique_ptr<Sprite> bg;
	std::unique_ptr<Sprite> sprite;

	float loadTime = 10.0f;
	float loadTimer = 0.0f;

	float uiEmitterTimer = 0.0f;
	float uiEmitterTime = 0.1f;
	DirectX::XMFLOAT2 emitterPos = { 640, 360 };


	std::unique_ptr<FrameBuffer> frameBuffer;
	std::unique_ptr<FullScreenQuad> bitBlockTransfer;
	std::unique_ptr<Bloom> bloom;
	std::unique_ptr<WbOitBuffer> wbOitBuffer;
	std::unique_ptr<ColorFilter> colorFilter;
	std::unique_ptr<FrameBuffer> swordTrailBuffer;
	std::unique_ptr<FrameBuffer> swordTrailBufferSub;

};

#pragma once
#include <thread>
#include "Scene.h"

#include "../../Library/Graphics/FrameBuffer.h"
#include "../../Library/Graphics/FullScreenQuad.h"
#include "../../Library/Graphics/Bloom.h"
#include "../../Library/Graphics/WbOitBuffer.h"
#include "../../Library/Graphics/ColorFilter.h"

#include "../../Library/2D/Sprite.h"

// タイトルシーン
class SceneLoading : public Scene
{
public:
	SceneLoading(Scene* nextScene) : nextScene(nextScene){}
	~SceneLoading() override {};

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新
	void Update() override;

	// 描画
	void Render() override;

	void EmitUpdate();

private:
	// ローディングスレッド
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

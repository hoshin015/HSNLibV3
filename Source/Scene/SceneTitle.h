#pragma once
#include <memory>
// --- Library ---
#include "../../Library/2D/Sprite.h"
#include "../../Library/Graphics/FrameBuffer.h"
// --- Scene ---
#include "Scene.h"


// タイトルシーン
class SceneTitle : public Scene
{
public:
	SceneTitle() {}
	~SceneTitle() override {};

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新
	void Update() override;

	// 描画
	void Render() override;

	// デバッグ描画
	void DrawDebugGUI();

private:
	std::unique_ptr<Sprite> spr1;

	std::unique_ptr<FrameBuffer> imGuiFrameBuffer;	// ImGui描画用フレームバッファ
};

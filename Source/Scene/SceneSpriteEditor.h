#pragma once
#include <memory>
// --- Library ---
#include "../../Library/2D/Sprite.h"
#include "../../Library/Graphics/FrameBuffer.h"
// --- Scene ---
#include "Scene.h"

// スプライトエディターシーン
class SceneSpriteEditor : public Scene
{
public:
	SceneSpriteEditor() {}
	~SceneSpriteEditor() override {};

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新
	void Update() override;

	// 描画
	void Render() override;

private:
	// デバッグ描画
	void DrawDebugGUI();
	// スプライトエディター用メニューバー描画
	void DrawSpriteEditorMenuBar();
private:
	bool isMouseHoverSceneView = false;				// マウスがシーンビュー上にあるか
	std::unique_ptr<FrameBuffer> imGuiFrameBuffer;	// ImGui描画用フレームバッファ
	std::unique_ptr<FrameBuffer> imGuiFrameBuffer2;	// ImGui描画用フレームバッファ

	std::unique_ptr<Sprite> spr1;

	// 選択中のアニメーション
	int selected = 0;
};
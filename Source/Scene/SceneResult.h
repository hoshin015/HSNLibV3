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


// リザルトシーン
class SceneResult : public Scene
{
public:
	SceneResult() {}
	~SceneResult() override {};

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

private:
	std::unique_ptr<Sprite> sprBackground;

};
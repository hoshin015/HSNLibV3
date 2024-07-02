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
// --- Scene ---
#include "Scene.h"
// --- Game ---
#include "../Game/Object/TestStatic.h"
#include "../Game/Object/StateMachine/BlendTestPlayer/BlendTestPlayer.h"
#include "../Game/Object/Effect/Lightning/LightningMainMesh.h"
// --- Camera ---
#include "../Camera/CameraDerived.h"


// テストシーン
class SceneTest : public Scene
{
public:
	SceneTest() {}
	~SceneTest() override {};

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
	std::unique_ptr<FrameBuffer> frameBuffer;
	std::unique_ptr<FullScreenQuad> bitBlockTransfer;
	std::unique_ptr<Bloom> bloom;
	std::unique_ptr<Shadow> shadow;
	std::unique_ptr<WbOitBuffer> wbOitBuffer;
	std::unique_ptr<RadialBlur> radialBlur;
	std::unique_ptr<HeatHaze> heatHaze;

	std::unique_ptr<SkyMap> skyMap;

	std::unique_ptr<TestStatic> testStatic;
	std::unique_ptr<Sprite> sprTest;
	std::unique_ptr<Sprite> sprTest2;
	std::unique_ptr<Sprite> sprTest3;

	std::unique_ptr<BlendTestPlayer> blendTestPlayer;

	bool showCollision = false;

	CameraBase* camera;
	PlayerCamera playerCamera;
	LockOnCamera lockOnCamera;
};
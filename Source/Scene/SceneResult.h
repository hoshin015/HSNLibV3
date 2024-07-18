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
#include "../../Library/Graphics/FullScreenQuad.h"
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
	enum class Rank
	{
		S,
		A,
		B,
		C
	};
	int rank;

	std::unique_ptr<Sprite> sprBlack;
	std::unique_ptr<Sprite> sprBackground;
	std::unique_ptr<Sprite> sprTimeBoard;
	std::unique_ptr<Sprite> sprRankBoard;
	std::unique_ptr<Sprite> sprRankS;
	std::unique_ptr<Sprite> sprRankA;
	std::unique_ptr<Sprite> sprRankB;
	std::unique_ptr<Sprite> sprRankC;
	std::unique_ptr<Sprite> sprGoTitle;

	// 背景
	std::unique_ptr<FullScreenQuad> capturedBackground;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> capturedSrv;

	std::unique_ptr<ColorFilter> colorFilter;


	float resultTimer = 0.0f;
	float resultTime = 4.0f;

	Easing::EasingValue blackAlpha =
	{
		0.0f, 1.0f,
		1.0f, 0.0f
	};
	Easing::EasingValue timeRankBoardAlpha =
	{
		1.0f, 2.0f,
		0.0f, 1.0f
	};
	Easing::EasingValue rankAlpha =
	{
		2.0f, 2.2f,
		0.0f,1.0f
	};
	Easing::EasingValue rankScale =
	{
		2.0f, 2.6f,
		2.0f,1.0f
	};
	Easing::EasingValue goTitleAlpha =
	{
		3.0f, 4.0f,
		0.0f, 1.0f
	};
};
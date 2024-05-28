#pragma once
// --- Scene ---
#include "Scene.h"
#include "SceneTitle.h"
#include "SceneTest.h"
#include "SceneModelEditor.h"
#include "SceneSpriteEditor.h"

// シーンマネージャー
class SceneManager
{
private:
	SceneManager() {}
	~SceneManager() {}

public:
	// 唯一のインスタンス取得
	static SceneManager& Instance()
	{
		static SceneManager instance;
		return instance;
	}

	// 更新処理
	void Update();

	// 描画処理
	void Render();

	// シーンクリア
	void Clear();

	// シーン切り替え
	void ChangeScene(Scene* scene);

private:
	Scene* currentScene = nullptr;
	Scene* nextScene = nullptr;
};
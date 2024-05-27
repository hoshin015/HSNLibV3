#pragma once
#include <string>

//--------------------------------------------------------------
//  Scene
//--------------------------------------------------------------
class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}


	// 初期化
	virtual void Initialize() = 0;

	// 終了化
	virtual void Finalize() = 0;

	// 更新処理
	virtual void Update() = 0;

	// 描画処理
	virtual void Render() = 0;

	// 準備完了しているか
	bool IsReady() const { return ready; }
	// 準備完了設定
	void SetReady() { ready = true; }

protected:
	// メニューバーの描画
	void DrawMenuBar();
	// タイマー定数バッファの更新
	void UpdateTimerConstnat();

private:
	bool ready = false;
};
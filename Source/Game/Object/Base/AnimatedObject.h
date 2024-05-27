#pragma once
// --- Game ---
#include "BaseObject.h"
#include "AnimatedModel.h"

// アニメーションする3Dオブジェクトのクラス
class AnimatedObject : public BaseObject
{
public:
	AnimatedObject() = default;
	AnimatedObject(const char* filePath);
	virtual ~AnimatedObject() = default;

	virtual void Update() = 0;
	virtual void Render(bool isShadow) = 0;

	AnimatedModel* GetModel() { return model.get(); }

	// アニメーション再生
	void PlayAnimation(int index, bool loop);
protected:
	// 姿勢行列更新
	void UpdateTransform() override;

	// アニメーション更新
	void UpdateAnimation();


protected:
	std::unique_ptr<AnimatedModel> model;

	// --- アニメーションフラグ ---
	ModelResource::KeyFrame keyFrame;		// 使用されるキーフレーム
	int currentKeyFrame = 0;				// 現在のフレーム
	int currentAnimationIndex = 0;			// 現在再生中のアニメーション番号
	float currentAnimationSeconds = 0.0f;	// 現在再生中の秒数

	bool animationLoopFlag = true;			// ループ再生するかどうか
	bool animationEndFlag = false;			// アニメーションが最後のキーフレームに到達したかどうか
	bool isPlayAnimation = true;			// アニメーションを再生中かどうか

public:
	int GetCurrentKeyFrame() { return currentKeyFrame; }
	int GetCurrentAnimationIndex() { return currentAnimationIndex; }
	bool GetAnimationLoopFlag() { return animationLoopFlag; }
	bool GetIsPlayAnimation() { return isPlayAnimation; }

	void SetKeyFrame(ModelResource::KeyFrame kf) { keyFrame = kf; }
	void SetCurrentKeyFrame(int kf) { currentKeyFrame = kf; }
	void SetCurrentAnimationIndex(int index) { currentAnimationIndex = index; }
	void SetCurrentAnimationSeconds(float s) { currentAnimationSeconds = s; }
	void SetAnimationLoopFlag(bool b) { animationLoopFlag = b; }
	void SetAnimationEndFlag(bool b) { animationEndFlag = b; }
	void SetIsPlayAnimation(bool b) { isPlayAnimation = b; }
};
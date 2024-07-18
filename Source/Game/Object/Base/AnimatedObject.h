#pragma once
// --- Game ---
#include "BaseObject.h"
#include "AnimatedModel.h"
#include "../../../../Library/Resource/Model/Animator.h"

// アニメーションする3Dオブジェクトのクラス
class AnimatedObject : public BaseObject
{
public:
	AnimatedObject() = default;
	AnimatedObject(const char* filePath);
	virtual ~AnimatedObject() = default;

	virtual void Update() = 0;
	virtual void Render(bool isShadow) = 0;

	// デバッグプリミティブ描画
	void DrawDebugPrimitive();

	AnimatedModel* GetModel() { return model.get(); }

	// アニメーション再生
	void PlayAnimation(int index, bool loop);

	// アニメーションブレンド
	void BlendAnimation(const ModelResource::KeyFrame* keyFrames[2], float factor, ModelResource::KeyFrame& keyFrame);

	// ダメージフラグのクリア
	void ClearAnimSphereCollisionDamagedFlag();

	// すべてのダメージフラグをtrueに
	void EnableAnimSphereCollisionDamagedFlag();

	// 効果音フラグのクリア
	void ClearSeFlag();
	void ClearAllSeFlag();
protected:
	// 姿勢行列更新
	void UpdateTransform() override;

	// アニメーション更新
	void UpdateAnimation();

	//
	void UpdateBlendAnimation();

	// 効果音更新
	void UpdateSe();

	// 効果音フラグのクリア
	// void ClearSeFlag();

public:
	DirectX::XMFLOAT3 GetBonePosition(std::string boneName);

protected:
	std::unique_ptr<AnimatedModel> model;

	// --- アニメーションフラグ ---
	Animator animator;

	ModelResource::KeyFrame animatorKeyFrame;//Animatorから取得したKeyFrame
	ModelResource::KeyFrame keyFrame;                       // 使用されるキーフレーム
	int                     currentKeyFrame         = 0;    // 現在のフレーム
	int                     currentAnimationIndex   = 0;    // 現在再生中のアニメーション番号
	float                   currentAnimationSeconds = 0.0f; // 現在再生中の秒数

	float					animationTimeScale = 1.0f;	// アニメーションの再生速度

	bool animationLoopFlag = true;  // ループ再生するかどうか
	bool animationEndFlag  = false; // アニメーションが最後のキーフレームに到達したかどうか
	bool isPlayAnimation   = true;  // アニメーションを再生中かどうか

	// --- アニメーションブレンド用フラグ ---
	int   blendAnimationIndex              = 0;    // 遷移前のアニメーション番号
	float animationBlendTimer               = 0.0f; // アニメーションブレンド用タイマー
	float animationBlendTransitionDurations = 0.0f; // この時間を掛けてアニメーションを遷移させる
	float exitTime                          = 0.0f; // (非ループアニメーション時使用)現在にアニメーションの時間がここまで到達したら次のアニメーションに遷移開始

	// --- ダブルアニメーション用フラグ ---
	bool                    isDoubleAnimations = false;           // 上半身別アニメーション使うかのフラグ
	ModelResource::KeyFrame doubleKeyFrame;                       // 上半身に使用されるキーフレーム
	int                     doubleCurrentKeyFrame         = 0;    // 上半身の現在のフレーム
	int                     doubleCurrentAnimationIndex   = 0;    // 上半身のアニメーション番号
	float                   doubleCurrentAnimationSeconds = 0.0f; // 上半身の現在再生中の秒数

public:
	// --- アニメーションフラグ ---
	Animator& GetAnimator() { return animator; };

	int GetCurrentKeyFrame() { return currentKeyFrame; }
	int GetCurrentAnimationIndex() { return currentAnimationIndex; }
	float GetCurrentAnimationSeconds() { return currentAnimationSeconds; }

	bool GetAnimationLoopFlag() { return animationLoopFlag; }
	bool GetIsPlayAnimation() { return isPlayAnimation; }
	bool GetAnimationEndFlag() { return animationEndFlag; }
	ModelResource::KeyFrame GetAnimatorKeyFrame() { return animatorKeyFrame; }

	void SetKeyFrame(ModelResource::KeyFrame kf) { keyFrame = kf; }
	void SetAnimatorKeyFrame(const ModelResource::KeyFrame& key) { animatorKeyFrame = key; }
	void SetCurrentKeyFrame(int kf) { currentKeyFrame = kf; }
	void SetCurrentAnimationIndex(int index) { currentAnimationIndex = index; }
	void SetCurrentAnimationSeconds(float s) { currentAnimationSeconds = s; }

	void SetAnimationLoopFlag(bool b) { animationLoopFlag = b; }
	void SetAnimationEndFlag(bool b) { animationEndFlag = b; }
	void SetIsPlayAnimation(bool b) { isPlayAnimation = b; }

	// --- アニメーションブレンド用フラグ ---
	int   GetBeforeAnimationIndex() { return blendAnimationIndex; }
	float GetAnimationBlendTimer() { return animationBlendTimer; }
	float GetAnimationBlendTransitionDurations() { return animationBlendTransitionDurations; }
	float GetExitTime() { return exitTime; }

	void SetBeforeAnimationIndex(int index) { blendAnimationIndex = index; }
	void SetAnimationBlendTimer(float time) { animationBlendTimer = time; }
	void SetAnimationBlendTransitionDurations(float time) { animationBlendTransitionDurations = time; }
	void SetExitTime(float time) { exitTime = time; }

	// --- ダブルアニメーション用フラグ ---
	bool GetIsDoubleAnimation() { return isDoubleAnimations; }

	void SetIsDoubleAnimation(bool b) { isDoubleAnimations = b; }
	void SetDoubleCurrentAnimationIndex(int index) { doubleCurrentAnimationIndex = index; }
};

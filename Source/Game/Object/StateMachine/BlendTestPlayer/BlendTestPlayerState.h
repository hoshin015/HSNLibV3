#pragma once
#include "../StateBase.h"
#include "BlendTestPlayer.h"

///////////////////////////
///
///		１層目
///
///////////////////////////


// 通常ステート
class BlendTestPlayerNormalState : public HierarchicalState<BlendTestPlayer>
{
public:
	BlendTestPlayerNormalState(BlendTestPlayer* owner) : HierarchicalState<BlendTestPlayer>(owner) {}
	~BlendTestPlayerNormalState() {}

	void Enter() override;
	void Execute() override;
	void Exit() override;
};

///////////////////////////
///
///		２層目
///
///////////////////////////

// 待機ステートオブジェクト
class BlendTestPlayerTestState : public State<BlendTestPlayer>
{
public:
	BlendTestPlayerTestState(BlendTestPlayer* owner) : State<BlendTestPlayer>(owner) {}
	~BlendTestPlayerTestState() override {}

	void Enter() override;
	void Execute() override;
	void Exit() override;
};
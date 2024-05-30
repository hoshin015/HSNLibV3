#pragma once
#include "../StateBase.h"
#include "Player.h"

///////////////////////////
///
///		１層目
///
///////////////////////////

// 通常ステート
class PlayerNormalState : public HierarchicalState<Player>
{
public:
	PlayerNormalState(Player* owner) : HierarchicalState<Player>(owner) {}
	~PlayerNormalState(){}

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
class PlayerIdleState : public State<Player>
{
public:
	PlayerIdleState(Player* owner) : State<Player>(owner){}
	~PlayerIdleState() override {}

	void Enter() override;
	void Execute() override;
	void Exit() override;
};

// 歩行ステートオブジェクト
class PlayerWalkState : public State<Player>
{
public:
	PlayerWalkState(Player* owner) : State<Player>(owner) {}
	~PlayerWalkState() override {}

	void Enter() override;
	void Execute() override;
	void Exit() override;
};
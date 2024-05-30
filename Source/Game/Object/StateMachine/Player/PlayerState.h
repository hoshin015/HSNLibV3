#pragma once
#include "../StateBase.h"
#include "Player.h"

///////////////////////////
///
///		�P�w��
///
///////////////////////////

// �ʏ�X�e�[�g
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
///		�Q�w��
///
///////////////////////////

// �ҋ@�X�e�[�g�I�u�W�F�N�g
class PlayerIdleState : public State<Player>
{
public:
	PlayerIdleState(Player* owner) : State<Player>(owner){}
	~PlayerIdleState() override {}

	void Enter() override;
	void Execute() override;
	void Exit() override;
};

// ���s�X�e�[�g�I�u�W�F�N�g
class PlayerWalkState : public State<Player>
{
public:
	PlayerWalkState(Player* owner) : State<Player>(owner) {}
	~PlayerWalkState() override {}

	void Enter() override;
	void Execute() override;
	void Exit() override;
};
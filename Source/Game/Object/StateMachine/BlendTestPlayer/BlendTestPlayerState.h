#pragma once
#include "../StateBase.h"
#include "BlendTestPlayer.h"

///////////////////////////
///
///		�P�w��
///
///////////////////////////


// �ʏ�X�e�[�g
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
///		�Q�w��
///
///////////////////////////

// �ҋ@�X�e�[�g�I�u�W�F�N�g
class BlendTestPlayerTestState : public State<BlendTestPlayer>
{
public:
	BlendTestPlayerTestState(BlendTestPlayer* owner) : State<BlendTestPlayer>(owner) {}
	~BlendTestPlayerTestState() override {}

	void Enter() override;
	void Execute() override;
	void Exit() override;
};
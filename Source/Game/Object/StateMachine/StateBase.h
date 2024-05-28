#pragma once

#include <vector>

template<typename T>
class State
{
	State(T* owner) : owner(owner){}
	virtual ~State(){}

	// �X�e�[�g�ɓ��������Ɏ��s
	virtual void Enter() = 0;
	// �X�e�[�g�ŏ�Ɏ��s
	virtual void Execute() = 0;
	// �X�e�[�g����o�鎞�Ɏ��s
	virtual void Exit() = 0;

protected:
	T* owner;
};

// �P�w�ڂ̃X�e�[�g�N���X
template<typename T>
class HierarchicalState : public State<T>
{
public:
	HierarchicalState(T* owner) : State<T>(owner){}
	virtual ~HierarchicalState(){}

	virtual void Enter() = 0;
	virtual void Execute() = 0;
	virtual void Exit() = 0;

	// �T�u�X�e�[�g�Z�b�g
	virtual void SetSubState(int newStateIndex);
	// �T�u�X�e�[�g�ύX
	virtual void ChangeSubState(int newStateIndex);
	// �T�u�X�e�[�g�o�^
	virtual void RegisterSubState(State<T>* state);
	// �T�u�X�e�[�g�̃C���f�b�N�X�擾
	virtual int GetSubStateIndex();

protected:
	// �Q�w�ڂ̃X�e�[�g�x�N�^�[
	std::vector<State<T>*> subStatePool;
	// ���݂̂Q�w�ڃX�e�[�g
	State<T>* subState = nullptr;
};


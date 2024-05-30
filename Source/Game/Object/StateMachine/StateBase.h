#pragma once

#include <vector>

template<typename T>
class State
{
public:
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
	void SetSubState(int newStateIndex)
	{
		currentSubState = subStatePool.at(newStateIndex);
		currentSubState->Enter();
	}
	// �T�u�X�e�[�g�ύX
	void ChangeSubState(int newStateIndex)
	{
		currentSubState->Exit();
		currentSubState = subStatePool.at(newStateIndex);
		currentSubState->Enter();
	}
	// �T�u�X�e�[�g�o�^
	void RegisterSubState(State<T>* state)
	{
		subStatePool.emplace_back(state);
	}
	// �T�u�X�e�[�g�̃C���f�b�N�X�擾
	int GetSubStateIndex()
	{
		int i = 0;
		for (State<T>* state : subStatePool)
		{
			if (state == currentSubState)
			{
				return i;
			}
			i++;
		}

		return -1;
	}

protected:
	// ���݂̂Q�w�ڃX�e�[�g
	State<T>* currentSubState = nullptr;
	// �Q�w�ڂ̃X�e�[�g�x�N�^�[
	std::vector<State<T>*> subStatePool;
};


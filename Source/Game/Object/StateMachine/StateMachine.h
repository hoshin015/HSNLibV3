#pragma once
#include "StateBase.h"

template<typename T>
class StateMachine
{
public:
	StateMachine() {}
	~StateMachine(){}

	// �X�V����
	void Update()
	{
		currentState->Execute();
	}
	// �X�e�[�g�Z�b�g(�X�e�[�g���Z�b�g���邾�� �O�ɃX�e�[�g���Ȃ����ɂ����g��Ȃ�)
	void SetState(int newStateIndex)
	{
		currentState = statePool.at(newStateIndex);
		currentState->Enter();
	}
	// �X�e�[�g�ύX
	void ChangeState(int newStateIndex)
	{
		// �T�u�X�e�[�g�� Exit����
		currentState->subState->Exit();

		currentState->Exit();
		currentState = statePool.at(newStateIndex);
		currentState->Enter();
	}
	// �X�e�[�g�o�^
	void RegisterState(HierarchicalState<T>* state)
	{
		statePool.emplace_back(state);
	}
	// ���݂̃X�e�[�g�ԍ��擾
	int GetStateIndex()
	{
		int i = 0;
		for (State<T>* state : statePool)
		{
			// ���݂̃X�e�[�g�ƈ�v����Ȃ� return
			if (state == currentState)
			{
				return i;
			}
			i++;
		}

		return -1;
	}
	// �Q�w�ڃX�e�[�g�ύX
	void ChangeSubState(int newStateIndex)
	{
		currentState->SetSubState(newStateIndex);
	}
	// �Q�w�ڃX�e�[�g�o�^
	void RegisterSubState(int index, State<T>* subState)
	{
		statePool.at(index)->RegisterSubState(subState);
	}
	// �X�e�[�g�擾
	HierarchicalState<T>* GetState()
	{
		return currentState;
	}
private:
	// ���݂̃X�e�[�g
	HierarchicalState<T>* currentState = nullptr;
	// �e�X�e�[�g��ێ�����z��
	std::vector<HierarchicalState<T>*> statePool;
};


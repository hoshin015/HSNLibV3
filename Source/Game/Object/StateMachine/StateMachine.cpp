#include "StateMachine.h"

template<typename T>
StateMachie<T>::~StateMachie()
{
	for (State<T>* state : statePool)
	{
		delete state;
	}
	statePool.clear();
}

// �X�V����
template<typename T>
void StateMachie<T>::Update()
{
	currentState->Execute();
}

// �X�e�[�g�Z�b�g
template <typename T>
void StateMachie<T>::SetState(int newStateIndex)
{
	currentState = statePool.at(newStateIndex);
	currentState->Enter();
}

// �X�e�[�g�ύX
template <typename T>
void StateMachie<T>::ChangeState(int newStateIndex)
{
	// �T�u�X�e�[�g�� Exit����
	currentState->subState->Exit();

	currentState->Exit();
	currentState = statePool.at(newStateIndex);
	currentState->Enter();
}

// �X�e�[�g�o�^
template <typename T>
void StateMachie<T>::RegisterState(HierarchicalState<T>* state)
{
	statePool.emplace_back(state);
}

// ���݂̃X�e�[�g�ԍ��擾
template <typename T>
int StateMachie<T>::GetStateIndex()
{
	int i = 0;
	for(State<T>* state : statePool)
	{
		// ���݂̃X�e�[�g�ƈ�v����Ȃ� return
		if(state == currentState)
		{
			return i;
		}
		i++;
	}

	return -1;
}

// �Q�w�ڃX�e�[�g�ύX
template <typename T>
void StateMachie<T>::ChangeSubState(int newStateIndex)
{
	currentState->SetSubState(newStateIndex);
}

// �Q�w�ڃX�e�[�g�o�^
template <typename T>
void StateMachie<T>::RegisterSubState(int index, State<T>* subState)
{
	statePool.at(index)->RegisterSubState(subState);
}


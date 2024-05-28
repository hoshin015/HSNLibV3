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

// 更新処理
template<typename T>
void StateMachie<T>::Update()
{
	currentState->Execute();
}

// ステートセット
template <typename T>
void StateMachie<T>::SetState(int newStateIndex)
{
	currentState = statePool.at(newStateIndex);
	currentState->Enter();
}

// ステート変更
template <typename T>
void StateMachie<T>::ChangeState(int newStateIndex)
{
	// サブステートの Exit処理
	currentState->subState->Exit();

	currentState->Exit();
	currentState = statePool.at(newStateIndex);
	currentState->Enter();
}

// ステート登録
template <typename T>
void StateMachie<T>::RegisterState(HierarchicalState<T>* state)
{
	statePool.emplace_back(state);
}

// 現在のステート番号取得
template <typename T>
int StateMachie<T>::GetStateIndex()
{
	int i = 0;
	for(State<T>* state : statePool)
	{
		// 現在のステートと一致するなら return
		if(state == currentState)
		{
			return i;
		}
		i++;
	}

	return -1;
}

// ２層目ステート変更
template <typename T>
void StateMachie<T>::ChangeSubState(int newStateIndex)
{
	currentState->SetSubState(newStateIndex);
}

// ２層目ステート登録
template <typename T>
void StateMachie<T>::RegisterSubState(int index, State<T>* subState)
{
	statePool.at(index)->RegisterSubState(subState);
}


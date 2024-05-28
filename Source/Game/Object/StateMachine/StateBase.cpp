#include "StateBase.h"

// サブステートセット
template <typename T>
void HierarchicalState<T>::SetSubState(int newStateIndex)
{
	subState = subStatePool.at(newStateIndex);
	subState->Enter();
}

// サブステート変更
template <typename T>
void HierarchicalState<T>::ChangeSubState(int newStateIndex)
{
	subState->Exit();
	subState = subStatePool.at(newStateIndex);
	subState->Enter();
}

// サブステート登録
template<typename T>
void HierarchicalState<T>::RegisterSubState(State<T>* state)
{
	subStatePool.emplace_back(state);
}

// サブステートのインデックス取得
template <typename T>
int HierarchicalState<T>::GetSubStateIndex()
{
	int i = 0;
	for(State<T>* state: subStatePool)
	{
		if(state == subState)
		{
			return i;
		}
		i++;
	}

	return -1;
}


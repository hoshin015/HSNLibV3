#pragma once
#include "StateBase.h"

template<typename T>
class StateMachine
{
public:
	StateMachine() {}
	~StateMachine(){}

	// 更新処理
	void Update()
	{
		currentState->Execute();
	}
	// ステートセット(ステートをセットするだけ 前にステートがない時にしか使わない)
	void SetState(int newStateIndex)
	{
		currentState = statePool.at(newStateIndex);
		currentState->Enter();
	}
	// ステート変更
	void ChangeState(int newStateIndex)
	{
		// サブステートの Exit処理
		currentState->subState->Exit();

		currentState->Exit();
		currentState = statePool.at(newStateIndex);
		currentState->Enter();
	}
	// ステート登録
	void RegisterState(HierarchicalState<T>* state)
	{
		statePool.emplace_back(state);
	}
	// 現在のステート番号取得
	int GetStateIndex()
	{
		int i = 0;
		for (State<T>* state : statePool)
		{
			// 現在のステートと一致するなら return
			if (state == currentState)
			{
				return i;
			}
			i++;
		}

		return -1;
	}
	// ２層目ステート変更
	void ChangeSubState(int newStateIndex)
	{
		currentState->SetSubState(newStateIndex);
	}
	// ２層目ステート登録
	void RegisterSubState(int index, State<T>* subState)
	{
		statePool.at(index)->RegisterSubState(subState);
	}
	// ステート取得
	HierarchicalState<T>* GetState()
	{
		return currentState;
	}
private:
	// 現在のステート
	HierarchicalState<T>* currentState = nullptr;
	// 各ステートを保持する配列
	std::vector<HierarchicalState<T>*> statePool;
};


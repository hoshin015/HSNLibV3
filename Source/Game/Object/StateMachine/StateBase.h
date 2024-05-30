#pragma once

#include <vector>

template<typename T>
class State
{
public:
	State(T* owner) : owner(owner){}
	virtual ~State(){}

	// ステートに入った時に実行
	virtual void Enter() = 0;
	// ステートで常に実行
	virtual void Execute() = 0;
	// ステートから出る時に実行
	virtual void Exit() = 0;

protected:
	T* owner;
};

// １層目のステートクラス
template<typename T>
class HierarchicalState : public State<T>
{
public:
	HierarchicalState(T* owner) : State<T>(owner){}
	virtual ~HierarchicalState(){}

	virtual void Enter() = 0;
	virtual void Execute() = 0;
	virtual void Exit() = 0;

	// サブステートセット
	void SetSubState(int newStateIndex)
	{
		currentSubState = subStatePool.at(newStateIndex);
		currentSubState->Enter();
	}
	// サブステート変更
	void ChangeSubState(int newStateIndex)
	{
		currentSubState->Exit();
		currentSubState = subStatePool.at(newStateIndex);
		currentSubState->Enter();
	}
	// サブステート登録
	void RegisterSubState(State<T>* state)
	{
		subStatePool.emplace_back(state);
	}
	// サブステートのインデックス取得
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
	// 現在の２層目ステート
	State<T>* currentSubState = nullptr;
	// ２層目のステートベクター
	std::vector<State<T>*> subStatePool;
};


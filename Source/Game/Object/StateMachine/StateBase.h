#pragma once

#include <vector>

template<typename T>
class State
{
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
	virtual void SetSubState(int newStateIndex);
	// サブステート変更
	virtual void ChangeSubState(int newStateIndex);
	// サブステート登録
	virtual void RegisterSubState(State<T>* state);
	// サブステートのインデックス取得
	virtual int GetSubStateIndex();

protected:
	// ２層目のステートベクター
	std::vector<State<T>*> subStatePool;
	// 現在の２層目ステート
	State<T>* subState = nullptr;
};


#pragma once
#include "StateBase.h"

template<typename T>
class StateMachie
{
public:
	StateMachie() {}
	~StateMachie();

	// 更新処理
	void Update();
	// ステートセット(ステートをセットするだけ 前にステートがない時にしか使わない)
	void SetState(int newStateIndex);
	// ステート変更
	void ChangeState(int newStateIndex);
	// ステート登録
	void RegisterState(HierarchicalState<T>* state);
	// 現在のステート番号取得
	int GetStateIndex();
	// ２層目ステート変更
	void ChangeSubState(int newStateIndex);
	// ２層目ステート登録
	void RegisterSubState(int index, State<T>* subState);
	// ステート取得
	HierarchicalState<T>* GetState() { return currentState; }
private:
	// 現在のステート
	HierarchicalState<T>* currentState = nullptr;
	// 各ステートを保持する配列
	std::vector<HierarchicalState<T>*> statePool;
};


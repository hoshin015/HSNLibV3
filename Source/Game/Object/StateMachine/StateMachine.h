#pragma once
#include "StateBase.h"

template<typename T>
class StateMachie
{
public:
	StateMachie() {}
	~StateMachie();

	// �X�V����
	void Update();
	// �X�e�[�g�Z�b�g(�X�e�[�g���Z�b�g���邾�� �O�ɃX�e�[�g���Ȃ����ɂ����g��Ȃ�)
	void SetState(int newStateIndex);
	// �X�e�[�g�ύX
	void ChangeState(int newStateIndex);
	// �X�e�[�g�o�^
	void RegisterState(HierarchicalState<T>* state);
	// ���݂̃X�e�[�g�ԍ��擾
	int GetStateIndex();
	// �Q�w�ڃX�e�[�g�ύX
	void ChangeSubState(int newStateIndex);
	// �Q�w�ڃX�e�[�g�o�^
	void RegisterSubState(int index, State<T>* subState);
	// �X�e�[�g�擾
	HierarchicalState<T>* GetState() { return currentState; }
private:
	// ���݂̃X�e�[�g
	HierarchicalState<T>* currentState = nullptr;
	// �e�X�e�[�g��ێ�����z��
	std::vector<HierarchicalState<T>*> statePool;
};


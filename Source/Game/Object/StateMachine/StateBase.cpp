#include "StateBase.h"

// �T�u�X�e�[�g�Z�b�g
template <typename T>
void HierarchicalState<T>::SetSubState(int newStateIndex)
{
	subState = subStatePool.at(newStateIndex);
	subState->Enter();
}

// �T�u�X�e�[�g�ύX
template <typename T>
void HierarchicalState<T>::ChangeSubState(int newStateIndex)
{
	subState->Exit();
	subState = subStatePool.at(newStateIndex);
	subState->Enter();
}

// �T�u�X�e�[�g�o�^
template<typename T>
void HierarchicalState<T>::RegisterSubState(State<T>* state)
{
	subStatePool.emplace_back(state);
}

// �T�u�X�e�[�g�̃C���f�b�N�X�擾
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


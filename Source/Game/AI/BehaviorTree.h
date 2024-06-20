#pragma once

#include <string>
#include <vector>
#include <map>
#include <stack>



template<typename T>
class BehaviorData;


// --- �s����� ---
enum class BT_ActionState
{
	Run,		// ���s��
	Failed,		// ���s���s
	Complete,	// ���s����
};


// --- �I�����[�� ---
enum class BT_SelectRule
{
	Non,				// ���� ���[�m�[�h�p
	Priority,			// �D�揇��
	Sequence,			// �V�[�P���X
	SequentialLooping,	// �V�[�P���V�������[�s���O
	Random,				// �����_��
};




// --- �s�������̊��N���X �c���[�̖��[�m�[�h�ɂȂ� ---
template<typename T>
class ActionBase
{
public:
	ActionBase(T* owner) :owner_(owner) {}

	//  --- ���s���� ---
	virtual BT_ActionState Run(float elapsedTime) = 0;
protected:
	T* owner_;
	int step = 0;
};


// --- ���s���菈���̊��N���X �m�[�h�ɐi�ޏ������L�q���� ---
template<typename T>
class JudgmentBase
{
public:
	JudgmentBase(T* owner) :owner_(owner) {}

	// --- ���s���菈��(�������z�֐�) ---
	virtual bool Judgment() = 0;
protected:
	T* owner_;
};


// --- �m�[�h ---
template<typename T>
class NodeBase
{
public:
	/// <summary>
	/// --- �R���X�g���N�^ ---
	/// </summary>
	/// <param name="name">: ���O</param>
	/// <param name="parent">: �e�m�[�h</param>
	/// <param name="sibling">: �Z��m�[�h</param>
	/// <param name="priority">: �D�揇��</param>
	/// <param name="selectRule">: �I�����[��</param>
	/// <param name="judgment">: ����N���X</param>
	/// <param name="action">: ���s�N���X</param>
	/// <param name="hierarchyNo">: �K�w�ԍ�</param>
	NodeBase(
		std::string name,
		NodeBase* parent,
		NodeBase* sibling,
		int priority,	
		BT_SelectRule selectRule,
		JudgmentBase<T>* judgment,
		ActionBase<T>* action,
		int hierarchyNo
	) :
		name(name),
		parent(parent),
		sibling(sibling),
		priority(priority),
		selectRule(selectRule),
		judgment(judgment),
		action(action),
		hierarchyNo(hierarchyNo),
		children(NULL)
	{}

	// --- �f�X�g���N�^ ---
	~NodeBase()
	{
		delete judgment;
		delete action;
	}


	// --- ���O�Q�b�^�[ ---
	std::string GetName() { return name; }

	// --- �e�m�[�h�Q�b�^�[ ---
	NodeBase* GetParent() { return parent; }

	// --- �q�m�[�h�Q�b�^�[ ---
	NodeBase* GetChild(int index)
	{
		if (children.size() <= index)
		{
			return nullptr;
		}

		return children.at(index);
	}

	// --- �q�m�[�h�Q�b�^�[(����) ---
	NodeBase* GetLastChild()
	{
		if (children.size() == 0)
		{
			return nullptr;
		}

		return children.at(children.size() - 1);
	}

	// --- �q�m�[�h�Q�b�^�[(�擪) ---
	NodeBase* GetTopChild()
	{
		if (children.size() == 0)
		{
			return nullptr;
		}

		return children.at(0);
	}

	// --- �Z��m�[�h�Q�b�^�[ ---
	NodeBase* GetSibling() { return sibling; }

	// --- �K�w�ԍ��Q�b�^�[ ---
	int GetHirerchyNo() { return hierarchyNo; }

	// --- �D�揇�ʃQ�b�^�[ ---
	int GetPriority() { return priority; }

	// --- �e�m�[�h�Z�b�^�[ ---
	void SetParent(NodeBase* parent) { this->parent = parent; }

	// --- �q�m�[�h�ǉ� ---
	void AddChild(NodeBase* child) { children.push_back(child); }

	// --- �Z��m�[�h�Z�b�^�[ ---
	void SetSibling(NodeBase* sibling) { this->sibling = sibling; }

	//  --- �s���f�[�^�������Ă��邩 ---
	bool HasAction() { return action != nullptr ? true : false; }

	// --- ���s�۔��� ---
	bool Judgment()
	{
		if (judgment != nullptr)
		{
			return judgment->Judgment();
		}
		return true;
	}

	// --- �D�揇�ʑI��  ��ԗD�揇�ʂ̍����m�[�h��Ԃ�---
	NodeBase* SelectPriority(std::vector<NodeBase*>* list)
	{
		NodeBase* selectNode = nullptr;
		int priority = INT_MAX;

		for (auto& node : *list)
		{
			// --- �l���������ق����ŗD�� ---
			if (node->priority < priority)
			{
				priority = node->priority;
				selectNode = node;
			}
		}

		return selectNode;
	}

	// --- �����_���I�� ---
	NodeBase* SelectRandom(std::vector<NodeBase*>* list)
	{
		int selectNo = 0;
		selectNo = rand() % list->size();

		// --- ���X�g��selectNo�Ԗڂ�Ԃ� ---
		return (*list).at(selectNo);
	}

	// --- �V�[�P���X�I�� �I���\�ȃm�[�h�����ԂɎ��s����---
	NodeBase* SelectSequence(std::vector<NodeBase*>* list, BehaviorData<T>* data)
	{
		int step = 0;

		//  --- �w�肳��Ă��钆�ԃm�[�h�̂��V�[�P���X���ǂ��܂Ŏ��s���ꂽ���擾���� ---
		step = data->GetSequenceStep(name);

		// --- ���ԃm�[�h�ɓo�^����Ă���m�[�h���ȏ�̏ꍇ ( �Ō�܂ł������ꍇ ) ---
		if (step >= children.size())
		{
			// --- ���[���ɂ���ď�����؂�ւ��� ---
			// --- ���[�v�Ȃ�X�e�b�v��0�� ---
			if (selectRule == BT_SelectRule::SequentialLooping)
				step = 0;

			// --- ����ȊO�Ȃ�A���Ɏ��s�ł���m�[�h���Ȃ��̂�null��Ԃ� ---
			else if (selectRule == BT_SelectRule::Sequence)
				return nullptr;
		}


		// --- ���s�\���X�g�ɓo�^����Ă���f�[�^�̐��������[�v���s�� ---
		for (auto itr = list->begin(); itr != list->end(); itr++)
		{
			// --- �q�m�[�h�����s�\���X�g�Ɋ܂܂�Ă��邩 ---
			if (children.at(step)->GetName() == (*itr)->GetName())
			{
				// --- ���݂̎��s�m�[�h�̕ۑ� ---
				data->PushSequenceNode(this);

				// --- ���Ɏ��s����X�e�b�v�̕ۑ����s�� ---
				data->SetSequenceStep(this->name, step + 1);

				// --- ���݂̃X�e�b�v�ԍ��̃m�[�h��Ԃ� ---
				return children.at(step);
			}
		}

		// --- �w�肳�ꂽ���ԃm�[�h�Ɏ��s�\�m�[�h���Ȃ��̂�null��Ԃ� ---
		return nullptr;
	}

	// --- �m�[�h���� ---
	NodeBase<T>* SearchNode(std::string searchName)
	{
		// --- ���O����v ---
		if (name == searchName)
		{
			return this;
		}

		else
		{
			// --- �q�m�[�h�Ō��� ---
			for (auto itr = children.begin(); itr != children.end(); itr++)
			{
				NodeBase<T>* ret = (*itr)->SearchNode(searchName);

				if (ret != nullptr)
				{
					return ret;
				}
			}
		}

		return nullptr;
	}

	// --- �m�[�h���_ ---
	NodeBase* Inference(BehaviorData<T>* data)
	{
		std::vector<NodeBase*> list;
		NodeBase* result = nullptr;

		// --- �q�m�[�h�̐��������[�v ---
		for (int i = 0; i < children.size(); i++)
		{
			// i�Ԗڂ̔���N���X���������� ---
			if (children.at(i)->judgment)
			{
				// --- ����N���X��true��Ԃ��΁A���X�g�Ɏq��i�Ԗڂ�ǉ����Ă��� ---
				if (children.at(i)->judgment->Judgment())
				{
					list.emplace_back(children.at(i));
				}
			}

			else
			{
				// --- ����N���X���Ȃ���Ζ������ɒǉ� ---
				list.emplace_back(children.at(i));
			}
		}


		// --- �I�����[���Ńm�[�h���� ---
		switch (selectRule)
		{
			// --- �D�揇�� ---
		case BT_SelectRule::Priority:
			result = SelectPriority(&list);
			break;

			// --- �����_�� ---
		case BT_SelectRule::Random:
			result = SelectRandom(&list);
			break;

			// --- �V�[�P���X ---
		case BT_SelectRule::Sequence:
		case BT_SelectRule::SequentialLooping:
			result = SelectSequence(&list, data);
			break;
		}


		// --- ���_���ʂ̃m�[�h������� ---
		if (result)
		{
			// --- �s��������Ζ��[�m�[�h�Ȃ̂ŏI�� ---
			if (result->HasAction())
				return result;

			// --- ����ȊO�́A���܂����m�[�h�Ő��_�J�n ---
			else
				result = result->Inference(data);
		}

		return result;
	}

	// --- ���s ---
	BT_ActionState Run(float elapsedTime)
	{
		// --- �s��������� ---
		if (action)
		{
			return action->Run(elapsedTime);
		}

		return BT_ActionState::Failed;
	}

	std::vector<NodeBase*>	children;	// �q�m�[�h
protected:
	std::string			name;			// ���O
	BT_SelectRule		selectRule;		// �I�����[��
	JudgmentBase<T>*	judgment;		// ����N���X
	ActionBase<T>*		action;			// ���s�N���X
	unsigned int		priority;		// �D�揇��
	NodeBase*			parent;			// �e�m�[�h
	NodeBase*			sibling;		// �Z��m�[�h
	int					hierarchyNo;	// �K�w�ԍ�
};



// --- Behavior�ۑ��f�[�^ ---
template<typename T>
class BehaviorData
{
public:
	// --- �R���X�g���N�^ ---
	BehaviorData() { Init(); }

	// --- �V�[�P���X�m�[�h�̃v�b�V�� ---
	void PushSequenceNode(NodeBase<T>* node) { sequenceStack.push(node); }

	// --- �V�[�P���X�m�[�h�̃|�b�v ---
	NodeBase<T>* PopSequenceNode()
	{
		// --- ��Ȃ�null��Ԃ� ---
		if (sequenceStack.empty() != 0)
		{
			return nullptr;
		}

		NodeBase<T>* node = sequenceStack.top();
		if (node != nullptr)
		{
			// --- ���o�����f�[�^���폜 ---
			sequenceStack.pop();
		}
		return node;
	}

	// --- �V�[�P���X�X�e�b�v�̃Q�b�^�[ ---
	int GetSequenceStep(std::string name)
	{
		if (runSequenceStepMap.count(name) == 0)
		{
			runSequenceStepMap.insert(std::make_pair(name, 0));
		}

		return runSequenceStepMap.at(name);
	}

	// --- �V�[�P���X�X�e�b�v�̃Z�b�^�[ ---
	void SetSequenceStep(std::string name, int step)
	{
		runSequenceStepMap.at(name) = step;
	}

	// --- ������ ---
	void Init()
	{
		runSequenceStepMap.clear();
		while (sequenceStack.size() > 0)
		{
			sequenceStack.pop();
		}
	}

private:
	std::stack<NodeBase<T>*> sequenceStack;				// ���s���钆�ԃm�[�h���X�^�b�N
	std::map<std::string, int> runSequenceStepMap;		// ���s���̒��ԃm�[�h�̃X�e�b�v���L�^
};



// --- �r�w�C�r�A�c���[ ---
template<typename T>
class BehaviorTree
{
public:

public:
	BehaviorTree() :root(nullptr), owner(nullptr) {}
	BehaviorTree(T* object) :root(nullptr), owner(object) {}
	~BehaviorTree()
	{
		if (root)
			NodeAllClear(root);
	}

	NodeBase<T>* GetRoot() { return root; }

	// --- ���s�m�[�h�𐄘_���� ---
	NodeBase<T>* ActiveNodeInference(BehaviorData<T>* data)
	{
		// --- �f�[�^�����Z�b�g���ĊJ�n ---
		data->Init();
		return root->Inference(data);
	}

	// --- �V�[�P���X�m�[�h���琄�_�J�n ---
	NodeBase<T>* SequenceBack(NodeBase<T>* sequenceNode, BehaviorData<T>* data)
	{
		return sequenceNode->Inference(data);
	}

	/// <summary>
	/// --- �m�[�h�̒ǉ� ---
	/// </summary>
	/// <param name="parentName">: �e�̖��O</param>
	/// <param name="entryName">: �ǉ�����m�[�h�̖��O</param>
	/// <param name="priority">: �D�揇��</param>
	/// <param name="selectRule">: �I�����[��</param>
	/// <param name="judgment">: ����N���X</param>
	/// <param name="action">: �s���N���X</param>
	void AddNode(
		std::string parentName,
		std::string entryName,
		int priority,
		BT_SelectRule selectRule, 
		JudgmentBase<T>* judgment,
		ActionBase<T>* action
	)
	{
		// --- �e�̖��O���Ȃ������� ---
		if (parentName != "")
		{
			NodeBase<T>* parentNode = root->SearchNode(parentName);

			if (parentNode != nullptr)
			{
				NodeBase<T>* sibling = parentNode->GetLastChild();
				NodeBase<T>* addNode = new NodeBase<T>(entryName, parentNode, sibling, priority, selectRule, judgment, action, parentNode->GetHirerchyNo() + 1);

				parentNode->AddChild(addNode);
			}
		}

		else 
		{
			if (!root)
			{
				root = new NodeBase<T>(entryName, nullptr, nullptr, priority, selectRule, judgment, action, 1);
			}
		}
	}

	// --- ���s ---
	NodeBase<T>* Run(NodeBase<T>* actionNode, BehaviorData<T>* data, float elapsedTime)
	{
		// --- �m�[�h���s ---
		BT_ActionState state = actionNode->Run(elapsedTime);

		// --- ����I�� ---
		if (state == BT_ActionState::Complete)
		{
			// --- �V�[�P���X�̓r�����𔻒f ---
			NodeBase<T>* sequenceNode = data->PopSequenceNode();

			// --- �r������Ȃ��Ȃ���s���I������̂ŏI�� ---
			if (!sequenceNode)
			{
				return nullptr;
			}

			else
			{
				// --- �r���Ȃ炻������n�߂� ---
				return SequenceBack(sequenceNode, data);
			}

		}


		// --- ���s�͏I�� ---
		else if (state == BT_ActionState::Failed)
			return nullptr;

		// ����ێ�
		return actionNode;
	}

private:
	// --- �m�[�h�S�폜 ---
	void NodeAllClear(NodeBase<T>* delNode)
	{
		size_t count = delNode->children.size();
		if (count > 0)
		{
			for (NodeBase<T>* node : delNode->children)
			{
				NodeAllClear(node);
			}
			delete delNode;
		}
		else
		{
			delete delNode;
		}
	}
private:
	// --- ���[�g�m�[�h ���ׂẴm�[�h�̍��{ ---
	NodeBase<T>* root;
	T* owner;
};
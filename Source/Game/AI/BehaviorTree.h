#pragma once

#include <string>
#include <vector>
#include <map>
#include <stack>



template<typename T>
class BehaviorData;


// --- 行動情報 ---
enum class BT_ActionState
{
	Run,		// 実行中
	Failed,		// 実行失敗
	Complete,	// 実行成功
};


// --- 選択ルール ---
enum class BT_SelectRule
{
	Non,				// 無し 末端ノード用
	Priority,			// 優先順位
	Sequence,			// シーケンス
	SequentialLooping,	// シーケンシャルルーピング
	Random,				// ランダム
};




// --- 行動処理の基底クラス ツリーの末端ノードになる ---
template<typename T>
class ActionBase
{
public:
	ActionBase(T* owner) :owner_(owner) {}

	//  --- 実行処理 ---
	virtual BT_ActionState Run(float elapsedTime) = 0;
protected:
	T* owner_;
	int step = 0;
};


// --- 実行判定処理の基底クラス ノードに進む条件を記述する ---
template<typename T>
class JudgmentBase
{
public:
	JudgmentBase(T* owner) :owner_(owner) {}

	// --- 実行判定処理(純粋仮想関数) ---
	virtual bool Judgment() = 0;
protected:
	T* owner_;
};


// --- ノード ---
template<typename T>
class NodeBase
{
public:
	/// <summary>
	/// --- コンストラクタ ---
	/// </summary>
	/// <param name="name">: 名前</param>
	/// <param name="parent">: 親ノード</param>
	/// <param name="sibling">: 兄弟ノード</param>
	/// <param name="priority">: 優先順位</param>
	/// <param name="selectRule">: 選択ルール</param>
	/// <param name="judgment">: 判定クラス</param>
	/// <param name="action">: 実行クラス</param>
	/// <param name="hierarchyNo">: 階層番号</param>
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

	// --- デストラクタ ---
	~NodeBase()
	{
		delete judgment;
		delete action;
	}


	// --- 名前ゲッター ---
	std::string GetName() { return name; }

	// --- 親ノードゲッター ---
	NodeBase* GetParent() { return parent; }

	// --- 子ノードゲッター ---
	NodeBase* GetChild(int index)
	{
		if (children.size() <= index)
		{
			return nullptr;
		}

		return children.at(index);
	}

	// --- 子ノードゲッター(末尾) ---
	NodeBase* GetLastChild()
	{
		if (children.size() == 0)
		{
			return nullptr;
		}

		return children.at(children.size() - 1);
	}

	// --- 子ノードゲッター(先頭) ---
	NodeBase* GetTopChild()
	{
		if (children.size() == 0)
		{
			return nullptr;
		}

		return children.at(0);
	}

	// --- 兄弟ノードゲッター ---
	NodeBase* GetSibling() { return sibling; }

	// --- 階層番号ゲッター ---
	int GetHirerchyNo() { return hierarchyNo; }

	// --- 優先順位ゲッター ---
	int GetPriority() { return priority; }

	// --- 親ノードセッター ---
	void SetParent(NodeBase* parent) { this->parent = parent; }

	// --- 子ノード追加 ---
	void AddChild(NodeBase* child) { children.push_back(child); }

	// --- 兄弟ノードセッター ---
	void SetSibling(NodeBase* sibling) { this->sibling = sibling; }

	//  --- 行動データを持っているか ---
	bool HasAction() { return action != nullptr ? true : false; }

	// --- 実行可否判定 ---
	bool Judgment()
	{
		if (judgment != nullptr)
		{
			return judgment->Judgment();
		}
		return true;
	}

	// --- 優先順位選択  一番優先順位の高いノードを返す---
	NodeBase* SelectPriority(std::vector<NodeBase*>* list)
	{
		NodeBase* selectNode = nullptr;
		int priority = INT_MAX;

		for (auto& node : *list)
		{
			// --- 値が小さいほうが最優先 ---
			if (node->priority < priority)
			{
				priority = node->priority;
				selectNode = node;
			}
		}

		return selectNode;
	}

	// --- ランダム選択 ---
	NodeBase* SelectRandom(std::vector<NodeBase*>* list)
	{
		int selectNo = 0;
		selectNo = rand() % list->size();

		// --- リストのselectNo番目を返す ---
		return (*list).at(selectNo);
	}

	// --- シーケンス選択 選択可能なノードを順番に実行する---
	NodeBase* SelectSequence(std::vector<NodeBase*>* list, BehaviorData<T>* data)
	{
		int step = 0;

		//  --- 指定されている中間ノードのがシーケンスがどこまで実行されたか取得する ---
		step = data->GetSequenceStep(name);

		// --- 中間ノードに登録されているノード数以上の場合 ( 最後までいった場合 ) ---
		if (step >= children.size())
		{
			// --- ルールによって処理を切り替える ---
			// --- ループならステップを0に ---
			if (selectRule == BT_SelectRule::SequentialLooping)
				step = 0;

			// --- それ以外なら、次に実行できるノードがないのでnullを返す ---
			else if (selectRule == BT_SelectRule::Sequence)
				return nullptr;
		}


		// --- 実行可能リストに登録されているデータの数だけループを行う ---
		for (auto itr = list->begin(); itr != list->end(); itr++)
		{
			// --- 子ノードが実行可能リストに含まれているか ---
			if (children.at(step)->GetName() == (*itr)->GetName())
			{
				// --- 現在の実行ノードの保存 ---
				data->PushSequenceNode(this);

				// --- 次に実行するステップの保存を行う ---
				data->SetSequenceStep(this->name, step + 1);

				// --- 現在のステップ番号のノードを返す ---
				return children.at(step);
			}
		}

		// --- 指定された中間ノードに実行可能ノードがないのでnullを返す ---
		return nullptr;
	}

	// --- ノード検索 ---
	NodeBase<T>* SearchNode(std::string searchName)
	{
		// --- 名前が一致 ---
		if (name == searchName)
		{
			return this;
		}

		else
		{
			// --- 子ノードで検索 ---
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

	// --- ノード推論 ---
	NodeBase* Inference(BehaviorData<T>* data)
	{
		std::vector<NodeBase*> list;
		NodeBase* result = nullptr;

		// --- 子ノードの数だけループ ---
		for (int i = 0; i < children.size(); i++)
		{
			// i番目の判定クラスがあったら ---
			if (children.at(i)->judgment)
			{
				// --- 判定クラスがtrueを返せば、リストに子のi番目を追加していく ---
				if (children.at(i)->judgment->Judgment())
				{
					list.emplace_back(children.at(i));
				}
			}

			else
			{
				// --- 判定クラスがなければ無条件に追加 ---
				list.emplace_back(children.at(i));
			}
		}


		// --- 選択ルールでノード決め ---
		switch (selectRule)
		{
			// --- 優先順位 ---
		case BT_SelectRule::Priority:
			result = SelectPriority(&list);
			break;

			// --- ランダム ---
		case BT_SelectRule::Random:
			result = SelectRandom(&list);
			break;

			// --- シーケンス ---
		case BT_SelectRule::Sequence:
		case BT_SelectRule::SequentialLooping:
			result = SelectSequence(&list, data);
			break;
		}


		// --- 推論結果のノードがあれば ---
		if (result)
		{
			// --- 行動があれば末端ノードなので終了 ---
			if (result->HasAction())
				return result;

			// --- それ以外は、決まったノードで推論開始 ---
			else
				result = result->Inference(data);
		}

		return result;
	}

	// --- 実行 ---
	BT_ActionState Run(float elapsedTime)
	{
		// --- 行動があれば ---
		if (action)
		{
			return action->Run(elapsedTime);
		}

		return BT_ActionState::Failed;
	}

	std::vector<NodeBase*>	children;	// 子ノード
protected:
	std::string			name;			// 名前
	BT_SelectRule		selectRule;		// 選択ルール
	JudgmentBase<T>*	judgment;		// 判定クラス
	ActionBase<T>*		action;			// 実行クラス
	unsigned int		priority;		// 優先順位
	NodeBase*			parent;			// 親ノード
	NodeBase*			sibling;		// 兄弟ノード
	int					hierarchyNo;	// 階層番号
};



// --- Behavior保存データ ---
template<typename T>
class BehaviorData
{
public:
	// --- コンストラクタ ---
	BehaviorData() { Init(); }

	// --- シーケンスノードのプッシュ ---
	void PushSequenceNode(NodeBase<T>* node) { sequenceStack.push(node); }

	// --- シーケンスノードのポップ ---
	NodeBase<T>* PopSequenceNode()
	{
		// --- 空ならnullを返す ---
		if (sequenceStack.empty() != 0)
		{
			return nullptr;
		}

		NodeBase<T>* node = sequenceStack.top();
		if (node != nullptr)
		{
			// --- 取り出したデータを削除 ---
			sequenceStack.pop();
		}
		return node;
	}

	// --- シーケンスステップのゲッター ---
	int GetSequenceStep(std::string name)
	{
		if (runSequenceStepMap.count(name) == 0)
		{
			runSequenceStepMap.insert(std::make_pair(name, 0));
		}

		return runSequenceStepMap.at(name);
	}

	// --- シーケンスステップのセッター ---
	void SetSequenceStep(std::string name, int step)
	{
		runSequenceStepMap.at(name) = step;
	}

	// --- 初期化 ---
	void Init()
	{
		runSequenceStepMap.clear();
		while (sequenceStack.size() > 0)
		{
			sequenceStack.pop();
		}
	}

private:
	std::stack<NodeBase<T>*> sequenceStack;				// 実行する中間ノードをスタック
	std::map<std::string, int> runSequenceStepMap;		// 実行中の中間ノードのステップを記録
};



// --- ビヘイビアツリー ---
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

	// --- 実行ノードを推論する ---
	NodeBase<T>* ActiveNodeInference(BehaviorData<T>* data)
	{
		// --- データをリセットして開始 ---
		data->Init();
		return root->Inference(data);
	}

	// --- シーケンスノードから推論開始 ---
	NodeBase<T>* SequenceBack(NodeBase<T>* sequenceNode, BehaviorData<T>* data)
	{
		return sequenceNode->Inference(data);
	}

	/// <summary>
	/// --- ノードの追加 ---
	/// </summary>
	/// <param name="parentName">: 親の名前</param>
	/// <param name="entryName">: 追加するノードの名前</param>
	/// <param name="priority">: 優先順位</param>
	/// <param name="selectRule">: 選択ルール</param>
	/// <param name="judgment">: 判定クラス</param>
	/// <param name="action">: 行動クラス</param>
	void AddNode(
		std::string parentName,
		std::string entryName,
		int priority,
		BT_SelectRule selectRule, 
		JudgmentBase<T>* judgment,
		ActionBase<T>* action
	)
	{
		// --- 親の名前がなかったら ---
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

	// --- 実行 ---
	NodeBase<T>* Run(NodeBase<T>* actionNode, BehaviorData<T>* data, float elapsedTime)
	{
		// --- ノード実行 ---
		BT_ActionState state = actionNode->Run(elapsedTime);

		// --- 正常終了 ---
		if (state == BT_ActionState::Complete)
		{
			// --- シーケンスの途中かを判断 ---
			NodeBase<T>* sequenceNode = data->PopSequenceNode();

			// --- 途中じゃないなら実行が終わったので終了 ---
			if (!sequenceNode)
			{
				return nullptr;
			}

			else
			{
				// --- 途中ならそこから始める ---
				return SequenceBack(sequenceNode, data);
			}

		}


		// --- 失敗は終了 ---
		else if (state == BT_ActionState::Failed)
			return nullptr;

		// 現状維持
		return actionNode;
	}

private:
	// --- ノード全削除 ---
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
	// --- ルートノード すべてのノードの根本 ---
	NodeBase<T>* root;
	T* owner;
};
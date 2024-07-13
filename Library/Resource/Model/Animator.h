#pragma once
#include <functional>
#include <map>
#include <variant>

#include "ModelResource.h"

#define STATE_FUNC(animator) [](Animator& (animator))->Animator::State*

class Animator {
public:
	using Var = std::variant<int, float, bool>;

public:
	struct Motion {
		ModelResource::Animation* motion         = nullptr;
		DirectX::XMFLOAT2         threshold      = {};
		float                     animationSpeed = 1;
		int                       animationIndex = 0;
		bool                      endMotion      = false;
		bool                      loop           = true;
	};

	struct BlendTree {
		std::vector<Motion> motions;
		std::string         parameters[2];
		float               maxSeconds = 0;
		bool                endMotion  = false;
	};

	struct State {
		using TransitionFunction = std::function<State*(Animator&)>;

		enum ObjectType {
			MOTION,
			BLEND_TREE,
		};

		std::shared_ptr<void> object;
		TransitionFunction    transitions;

		ObjectType type           = MOTION;
		float      speed          = 1;
		float      timer          = 0;
		float      transitionTime = 0.5f;

		template<typename T>
		std::shared_ptr<T> GetObj() { return std::static_pointer_cast<T>(object); }
	};

private:
	std::map<std::string, State> _states;
	std::map<std::string, Var>   _parameters;
	ModelResource::SceneView*    _sceneView;
	ModelResource::KeyFrame      _nextKeyFrame;

	State* _currentState;
	State* _nextState;

	std::string _rootMotionName;

	DirectX::XMFLOAT3 _velocity = {};

	float _timer;

	int _currentMotionIndex = 0;
	int _currentKeyFrameIndex = 0;

	bool _rootMotionEnabled = false;
	bool _isEndMotion       = false;

private:
	ModelResource::KeyFrame BlendKeyFrame(
		const ModelResource::KeyFrame* first, const ModelResource::KeyFrame* second, float lerpRate
	) const;
	ModelResource::KeyFrame MotionUpdate(Motion* motion, float rate);
	ModelResource::KeyFrame BlendUpdate(BlendTree* blend, float time);
	ModelResource::KeyFrame StateUpdate(State* state, float elapsedTime);

public:
	Animator() = default;

	/// Stateの追加
	/// @param name Stateの名前 MapのKeyとして使う
	/// @param state Animator::State
	void AddState(const std::string& name, const State& state) { _states[name] = state; }

	/// ユニークIDから親IDを取得するためだけにある
	/// @param sceneView
	void SetModelSceneView(ModelResource::SceneView* sceneView) { _sceneView = sceneView; }

	/// 始まりの遷移の設定
	///	すべてのStateを追加したあとじゃないとバグるかも
	/// @param name Stateの名前
	void SetEntryState(const std::string& name) { _currentState = &_states[name]; }

	/// BlendTreeのパラメータや別のStateに遷移するときに使う
	///	@param name Parameterの名前
	///	@param var 値 int,float,boolのいずれかを入れれる
	void SetParameter(const std::string& name, const Var var) { _parameters[name] = var; }

	/// RootMotionを有効にする
	/// @param name RootとなるBoneの名前
	void EnableRootMotion(const std::string& name) {
		_rootMotionEnabled = true;
		_rootMotionName    = name;
	}

	/// Parameterの取得
	///	Stateの関数を登録するときのみ使用する
	/// @tparam T int,float,bool
	/// @param name Parameterの名前
	/// @return 存在する場合Parameterの値を返す
	/// @return 型が合わないまたは存在しない場合は取得したい形の初期値を返す
	template<typename T>
	T          GetParameter(const std::string& name) {
		if (T* get = std::get_if<T>(&_parameters[name])) return *get;
		return T();
	}

	State&                   GetState(const std::string& name) { return _states[name]; }
	const State*             GetCurrentState() const { return _currentState; }
	const DirectX::XMFLOAT3& GetVelocity() const { return _velocity; }

	float GetTimer() const { return _timer; }
	int GetMotionIndex() const { return _currentMotionIndex; }
	int GetKeyFrameIndex() const { return _currentKeyFrameIndex; }
	bool  GetEndMotion() const { return _isEndMotion; }

	/// アニメーションの再生
	/// @param elapsedTime 経過時間を入れる
	/// @return KeyFrameを返す
	ModelResource::KeyFrame PlayAnimation(float elapsedTime);

	void AnimationEditor();

	/// Sharedを作る関数
	///	長いので作った 使わなくても良い
	/// @tparam T Motion,BlendTree
	/// @param obj Stateに追加したいオブジェクト
	/// @return Sharedを返す
	template<typename T>
	static std::shared_ptr<T> MakeObjPointer(T& obj) { return std::make_shared<T>(std::forward<T>(obj)); }
};

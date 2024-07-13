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

	/// State�̒ǉ�
	/// @param name State�̖��O Map��Key�Ƃ��Ďg��
	/// @param state Animator::State
	void AddState(const std::string& name, const State& state) { _states[name] = state; }

	/// ���j�[�NID����eID���擾���邽�߂����ɂ���
	/// @param sceneView
	void SetModelSceneView(ModelResource::SceneView* sceneView) { _sceneView = sceneView; }

	/// �n�܂�̑J�ڂ̐ݒ�
	///	���ׂĂ�State��ǉ��������Ƃ���Ȃ��ƃo�O�邩��
	/// @param name State�̖��O
	void SetEntryState(const std::string& name) { _currentState = &_states[name]; }

	/// BlendTree�̃p�����[�^��ʂ�State�ɑJ�ڂ���Ƃ��Ɏg��
	///	@param name Parameter�̖��O
	///	@param var �l int,float,bool�̂����ꂩ�������
	void SetParameter(const std::string& name, const Var var) { _parameters[name] = var; }

	/// RootMotion��L���ɂ���
	/// @param name Root�ƂȂ�Bone�̖��O
	void EnableRootMotion(const std::string& name) {
		_rootMotionEnabled = true;
		_rootMotionName    = name;
	}

	/// Parameter�̎擾
	///	State�̊֐���o�^����Ƃ��̂ݎg�p����
	/// @tparam T int,float,bool
	/// @param name Parameter�̖��O
	/// @return ���݂���ꍇParameter�̒l��Ԃ�
	/// @return �^������Ȃ��܂��͑��݂��Ȃ��ꍇ�͎擾�������`�̏����l��Ԃ�
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

	/// �A�j���[�V�����̍Đ�
	/// @param elapsedTime �o�ߎ��Ԃ�����
	/// @return KeyFrame��Ԃ�
	ModelResource::KeyFrame PlayAnimation(float elapsedTime);

	void AnimationEditor();

	/// Shared�����֐�
	///	�����̂ō���� �g��Ȃ��Ă��ǂ�
	/// @tparam T Motion,BlendTree
	/// @param obj State�ɒǉ��������I�u�W�F�N�g
	/// @return Shared��Ԃ�
	template<typename T>
	static std::shared_ptr<T> MakeObjPointer(T& obj) { return std::make_shared<T>(std::forward<T>(obj)); }
};

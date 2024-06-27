#pragma once
#include "ModelResource.h"

class Animator {
public:
	struct Motion {
		ModelResource::Animation* motion;
		DirectX::XMFLOAT2 threshold;
		float             animationSpeed;
	};

	struct BlendTree {
		std::vector<Motion> motions;
		DirectX::XMFLOAT2 parameter;
		float             timer;
		float             maxSeconds;
	};

	struct State {
		enum class ObjectType {
			MOTION,
			BLEND_TREE,
		};

		std::shared_ptr<void> object;
		ObjectType         type;
		float speed;

		//State(std::shared_ptr<void> obj, ObjectType t,float s):object(obj),type(t),speed(s){}
	};

private:
	std::vector<State>        _states;
	ModelResource::SceneView* _sceneView;

public:
	Animator() = default;

	template <typename T>
	void AddObject(T& obj) {
		if constexpr (std::is_same_v<T, Motion>) _states.emplace_back(State { std::make_shared<Motion>(std::forward<Motion>(obj)), State::ObjectType::MOTION,1 });
		if constexpr (std::is_same_v<T, BlendTree>) _states.emplace_back(State { std::make_shared<BlendTree>(std::forward<BlendTree>(obj)), State::ObjectType::BLEND_TREE,1 });
	}

	template<typename... Any>
	void AddAnimation(
		ModelResource::SceneView* sceneView,
		Any&... any
	) {
		_sceneView = sceneView;
		(AddObject(any),...);
	}

	ModelResource::KeyFrame PlayAnimation(float elapsedTime, float rate);

	void AnimationEditor(const std::string& name);
};

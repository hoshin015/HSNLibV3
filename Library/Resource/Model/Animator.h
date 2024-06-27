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

		std::vector<void*> object;
		ObjectType         type;

		float speed;
	};

private:
	std::vector<State>        _states;
	ModelResource::SceneView* _sceneView;

public:
	Animator() = default;

	template <typename T>
	void AddAnimation(T& obj) {
		if constexpr (std::is_same_v<T, Motion>) _states.emplace_back(&obj, State::ObjectType::MOTION);
		if constexpr (std::is_same_v<T, BlendTree>) _states.emplace_back(&obj, State::ObjectType::BLEND_TREE);
	}

	template<typename... Any>
	void AddAnimation(
		ModelResource::SceneView* sceneView,
		Any&... any
	) {
		_sceneView = sceneView;
		AddAnimation(any...);
	}

	ModelResource::KeyFrame PlayAnimation(const std::string& name, float elapsedTime, float rate);

	void AnimationEditor(const std::string& name);
};

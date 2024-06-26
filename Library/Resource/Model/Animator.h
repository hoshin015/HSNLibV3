#pragma once
#include "ModelResource.h"

class Animator{
public:
	struct Motion {
		ModelResource::Animation* motion;

		float threshold;
		float animationSpeed;
	};

	struct BlendState {
		std::vector<Motion> motions;
		float blendRate;
		float timer;
		float maxSeconds;
	};

private:
	std::unordered_map<std::string , BlendState> _state;

public:
	Animator() = default;

	void AddBlendAnimation(const std::string& name, std::initializer_list<ModelResource::Animation*> animations);

	ModelResource::KeyFrame PlayAnimation(const std::string& name,float elapsedTime);
};

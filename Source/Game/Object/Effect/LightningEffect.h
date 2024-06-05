#pragma once
#include <memory>
#include "LightningMainMesh.h"

class LightningEffect
{
public:
	LightningEffect(){}
	~LightningEffect(){}

	void Update();
	void Render(bool isShadow = false) ;

private:
	float lifeTime = 10.0f;

	std::unique_ptr<LightningMainMesh> lightningMainMesh;
};
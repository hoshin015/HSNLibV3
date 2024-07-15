#include "EffectDamageManager.h"
#include "../../../../Library/3D/DebugPrimitive.h"

void EffectDamageManager::Initialize()
{
}

void EffectDamageManager::Update()
{

}

void EffectDamageManager::Render()
{
	for(auto collisions : effectCollisions)
	{
		for(auto sphere : collisions.spheres)
		{
			DebugPrimitive::Instance().AddSphere(sphere.position, sphere.radius, {1,0,0,1});
		}
	}
	Clear();
}

void EffectDamageManager::Clear()
{
	effectCollisions.clear();
}

void EffectDamageManager::Register(EffectCollision effectCollision)
{
	effectCollisions.emplace_back(effectCollision);
}

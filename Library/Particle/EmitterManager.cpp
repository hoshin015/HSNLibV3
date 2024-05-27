#include "EmitterManager.h"

void EmitterManager::Update()
{
	for (auto emitter : emitters)
	{
		emitter->Update();
	}

	// íœ
	for (auto removeEmitter : removes)
	{
		std::vector<Emitter*>::iterator it = std::find(emitters.begin(), emitters.end(), removeEmitter);

		if (it != emitters.end())
		{
			emitters.erase(it);
		}

		delete removeEmitter;
	}
	removes.clear();
}

void EmitterManager::Register(Emitter* emitter)
{
	emitters.emplace_back(emitter);
}

void EmitterManager::Remove(Emitter* emitter)
{
	removes.insert(emitter);
}

void EmitterManager::Clear()
{
	for (auto emitter : emitters)
	{
		delete emitter;
	}
	emitters.clear();
	removes.clear();
}

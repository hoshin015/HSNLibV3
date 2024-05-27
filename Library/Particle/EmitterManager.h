#pragma once
#include <vector>
#include <set>
#include "Emitter.h"

class EmitterManager
{
private:
	EmitterManager() {}
	~EmitterManager() {}

public:
	static EmitterManager& Instance()
	{
		static EmitterManager instance;
		return instance;
	}

	// �X�V
	void Update();

	// �o�^
	void Register(Emitter* emitter);

	// �폜
	void Remove(Emitter* emitter);

	// �S�폜
	void Clear();

private:
	std::vector<Emitter*> emitters;
	std::set<Emitter*> removes;
};

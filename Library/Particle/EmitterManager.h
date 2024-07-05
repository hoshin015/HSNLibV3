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

	// 更新
	void Update();

	// 登録
	void Register(Emitter* emitter);

	// 削除
	void Remove(Emitter* emitter);

	// 全削除
	void Clear();

	// 空かどうかのチェック
	bool Empty() { return emitters.empty(); };

private:
	std::vector<Emitter*> emitters;
	std::set<Emitter*> removes;
};

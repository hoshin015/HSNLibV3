#pragma once
#include "SpriteResource.h"

class LoadSprite
{
private:
	LoadSprite() {}
	~LoadSprite() {}

public:
	static LoadSprite& Instance()
	{
		static LoadSprite instnace;
		return instnace;
	}

	// sprite ÇÃÉçÅ[Éh
	std::shared_ptr<SpriteResource> Load(const char* modelFilename);

	std::wstring ConvertToWideString(const char* narrowString);
};
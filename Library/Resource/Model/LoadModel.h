#pragma once
#include "ModelResource.h"

class LoadModel
{
private:
	LoadModel() {}
	~LoadModel() {}

public:
	static LoadModel& Instance()
	{
		static LoadModel instnace;
		return instnace;
	}

	// fbx ÇÃÉçÅ[Éh
	std::shared_ptr<ModelResource> Load(const char* modelFilename);
};
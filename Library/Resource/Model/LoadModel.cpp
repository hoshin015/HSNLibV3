#include "LoadModel.h"

std::shared_ptr<ModelResource> LoadModel::Load(const char* modelFilename)
{
	std::shared_ptr<ModelResource> modelResource = std::make_unique<ModelResource>();

	modelResource->LoadModelData(modelFilename);

	return modelResource;
}

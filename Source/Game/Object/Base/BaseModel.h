#pragma once
// --- Library ---
#include "../../Library/Resource//Model/LoadFbx.h"
#include "../../Library/3D/ResourceManager.h"

// ���f���f�[�^�̊��N���X
class BaseModel
{
public:
	BaseModel() = default;
	virtual ~BaseModel() = default;

	ModelResource* GetModelResource() { return modelResource.get(); }
protected:
	std::shared_ptr<ModelResource> modelResource;
};
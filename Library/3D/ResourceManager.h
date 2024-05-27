#pragma once
#include <map>
#include <memory>
#include <string>
#include "../Resource/Model/ModelResource.h"
#include "../Resource/Sprite/SpriteResource.h"


// ���\�[�X�}�l�[�W���[
class ResourceManager
{
private:
	ResourceManager() {}
	~ResourceManager() {}

public:
	static ResourceManager& Instance()
	{
		static ResourceManager instance;
		return instance;
	}

	// ���f�����\�[�X�ǂݍ���
	std::shared_ptr<ModelResource> LoadModelResource(const char* filename);
	// �X�v���C�g���\�[�X�Ǎ�
	std::shared_ptr<SpriteResource> LoadSpriteResource(const char* filename);

private:
	using ModelMap = std::map<std::string, std::weak_ptr<ModelResource>>;
	using SpriteMap = std::map <std::string, std::weak_ptr<SpriteResource>>;

	ModelMap models;
	SpriteMap sprites;
};
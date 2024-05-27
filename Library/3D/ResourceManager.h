#pragma once
#include <map>
#include <memory>
#include <string>
#include "../Resource/Model/ModelResource.h"
#include "../Resource/Sprite/SpriteResource.h"


// リソースマネージャー
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

	// モデルリソース読み込み
	std::shared_ptr<ModelResource> LoadModelResource(const char* filename);
	// スプライトリソース読込
	std::shared_ptr<SpriteResource> LoadSpriteResource(const char* filename);

private:
	using ModelMap = std::map<std::string, std::weak_ptr<ModelResource>>;
	using SpriteMap = std::map <std::string, std::weak_ptr<SpriteResource>>;

	ModelMap models;
	SpriteMap sprites;
};
#include <filesystem>
#include "ResourceManager.h"
#include "../Resource/Model/LoadFbx.h"
#include "../Resource/Model/LoadModel.h"
#include "../Resource/Sprite/LoadImageFormat.h"
#include "../Resource/Sprite/LoadSprite.h"


// モデルリソース読み
std::shared_ptr<ModelResource> ResourceManager::LoadModelResource(const char* filename)
{
	// モデルを検索
	ModelMap::iterator it = models.find(filename);
	if (it != models.end())
	{
		// リンク(寿命)が切れていないか確認
		if (!it->second.expired())
		{
			// 読み込み済みのモデルリソースを返す
			return it->second.lock();
		}
	}

	// 形式をチェックして新規モデルリソース作成＆読み込み
	std::shared_ptr<ModelResource> model;
	
	std::filesystem::path path(filename);
	std::string extension = path.extension().string();

	if (extension == ".model")
	{
		model = LoadModel::Instance().Load(filename);
	}
	else if (extension == ".fbx")
	{
		model = LoadFbx::Instance().Load(filename, true);
	}
	

	// マップに登録
	models[filename] = model;

	return model;
}

// スプライト読込
std::shared_ptr<SpriteResource> ResourceManager::LoadSpriteResource(const char* filename)
{
	// モデルを検索
	SpriteMap::iterator it = sprites.find(filename);
	if (it != sprites.end())
	{
		// リンク(寿命)が切れていないか確認
		if (!it->second.expired())
		{
			// 読み込み済みのモデルリソースを返す
			return it->second.lock();
		}
	}

	// 形式をチェックして新規モデルリソース作成＆読み込み
	std::shared_ptr<SpriteResource> sprite;

	std::filesystem::path path(filename);
	std::string extension = path.extension().string();

	if (extension == ".sprite")
	{
		sprite = LoadSprite::Instance().Load(filename);
	}
	else
	{
		sprite = LoadImageFormat::Instance().Load(filename);
	}

	// マップに登録
	sprites[filename] = sprite;

	return sprite;
}

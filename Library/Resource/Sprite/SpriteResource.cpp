#include "SpriteResource.h"


// 独自形式で書き出し
void SpriteResource::OutputSpriteData()
{
	// parentPath の取得
	std::filesystem::path fbxFilePath(filePath);
	std::string parentPath = fbxFilePath.parent_path().string();

	std::string fileName = fbxFilePath.stem().string();

	// sprite の出力
	{
		// sprite の出力ファイル作成
		std::string spriteFilename = parentPath + "/" + fileName + ".sprite";

		std::ofstream ofs(spriteFilename.c_str(), std::ios::binary);
		cereal::BinaryOutputArchive  serialization(ofs);

		serialization(filePath, textureSize, animations);
	}

	// animation の出力
	//if (!animations.empty())
	//{
	//	// フォルダ作成
	//	std::string animPath = parentPath + "/SprAnims";
	//	RemoveAllContents(animPath);
	//	std::filesystem::create_directory(animPath);
	//
	//	for (auto& anim : animations)
	//	{
	//		// animationの出力ファイル作成
	//		std::string animFilename = animPath + "/" + anim.name + ".sprAnim";
	//
	//		std::ofstream ofs(animFilename.c_str(), std::ios::binary);
	//		cereal::BinaryOutputArchive  serialization(ofs);
	//
	//		serialization(anim);
	//	}
	//}
}

// 独自形式読込
void SpriteResource::LoadSpriteData(const char* filePath)
{
	this->filePath = filePath;

	// --- 拡張子用変数作成 ---
	std::string pathExtension = "";

	std::filesystem::path path(filePath);
	std::string parentPath = path.parent_path().string();

	
	{
		std::string spriteFilePath = parentPath + "/" + path.stem().string() + ".sprite";
		std::ifstream ifs(spriteFilePath.c_str(), std::ios::binary);
		cereal::BinaryInputArchive deserialization(ifs);
		deserialization(this->filePath, textureSize, animations);
	}

	// animation の読込
	//std::string animationPath = parentPath + "/SprAnims/";
	//pathExtension = ".sprAnim";
	//
	//for (const auto& entry : std::filesystem::directory_iterator(animationPath))
	//{
	//	if (entry.path().extension() == pathExtension)
	//	{
	//		std::ifstream meshIfs(entry.path().c_str(), std::ios::binary);
	//		cereal::BinaryInputArchive deserialization(meshIfs);
	//
	//		Animation anim;
	//		deserialization(anim);
	//		animations.emplace_back(anim);
	//	}
	//}
}

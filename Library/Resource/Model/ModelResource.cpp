#include "ModelResource.h"



// 独自形式で書き出し
void ModelResource::OutputModelData()
{
	// parentPath の取得
	std::filesystem::path fbxFilePath(filePath);
	std::string parentPath = fbxFilePath.parent_path().string();

	std::string fileName = fbxFilePath.stem().string();

	// sceneView の出力
	{
		// model の出力ファイル作成
		std::string modelFilename = parentPath + "/" + fileName + ".model";
		
		std::ofstream ofs(modelFilename.c_str(), std::ios::binary);
		cereal::BinaryOutputArchive  serialization(ofs);

		serialization(sceneView, scale, coordinateSystemTransformType, coordinateSystemTransform);
	}

	// mesh の出力
	if (!meshes.empty())
	{
		// フォルダ作成
		std::string meshPath = parentPath + "/Meshes";
		RemoveAllContents(meshPath);
		std::filesystem::create_directory(meshPath);

		for (auto& mesh : meshes)
		{
			// meshの出力ファイル作成
			std::string meshFilename = meshPath + "/" + mesh.meshName + ".mesh";

			std::ofstream ofs(meshFilename.c_str(), std::ios::binary);
			cereal::BinaryOutputArchive  serialization(ofs);

			serialization(mesh);
		}
	}

	// material の出力
	if (!materials.empty())
	{
		// フォルダ作成
		std::string materialPath = parentPath + "/Materials";
		RemoveAllContents(materialPath);
		std::filesystem::create_directory(materialPath);

		for (auto [name, material] : materials)	// 構造化束縛
		{
			// meshの出力ファイル作成
			std::string materialFilename = materialPath + "/" + name + ".material";

			std::ofstream ofs(materialFilename.c_str(), std::ios::out);
			cereal::JSONOutputArchive  serialization(ofs);

			serialization(material);
		}
	}

	// animation の出力
	if (!animationClips.empty())
	{
		// フォルダ作成
		std::string animtionPath = parentPath + "/Anims";
		RemoveAllContents(animtionPath);
		std::filesystem::create_directory(animtionPath);

		for (auto& animation : animationClips)
		{
			// animationの出力ファイル作成
			std::string animationFilename = animtionPath + "/" + animation.name + ".anim";

			std::ofstream ofs(animationFilename.c_str(), std::ios::binary);
			cereal::BinaryOutputArchive  serialization(ofs);

			serialization(animation);
		}
	}
}

void ModelResource::LoadModelData(const char* filePath)
{
	this->filePath = filePath;

	// --- 拡張子用変数作成 ---
	std::string pathExtension = "";

	std::filesystem::path path(filePath);
	std::string parentPath = path.parent_path().string();

	// sceneView の読込
	{
		std::string modelFilePath = parentPath + "/" + path.stem().string() + ".model";
		std::ifstream ifs(modelFilePath.c_str(), std::ios::binary);
		cereal::BinaryInputArchive deserialization(ifs);
		deserialization(sceneView, scale, coordinateSystemTransformType, coordinateSystemTransform);
	}

	// mesh の読込
	std::string meshPath = parentPath + "/Meshes/";
	pathExtension = ".mesh";

	for (const auto& entry : std::filesystem::directory_iterator(meshPath))
	{
		if (entry.path().extension() == pathExtension) 
		{
			std::ifstream meshIfs(entry.path().c_str(), std::ios::binary);
			cereal::BinaryInputArchive deserialization(meshIfs);

			Mesh mesh;
			deserialization(mesh);
			meshes.emplace_back(mesh);
		}
	}

	// material の読込
	std::string materialPath = parentPath + "/Materials/";
	pathExtension = ".material";

	for (const auto& entry : std::filesystem::directory_iterator(materialPath)) 
	{
		if (entry.path().extension() == pathExtension) 
		{
			std::ifstream materialIfs(entry.path().c_str(), std::ios::in);
			cereal::JSONInputArchive   deserialization(materialIfs);
			Material material;
			deserialization(material);
			materials.emplace(material.name, std::move(material));
		}
	}

	// animation の読込
	std::string animPath = parentPath + "/Anims/";
	pathExtension = ".anim";

	// Anims ファイルがあるか確認
	if (std::filesystem::exists(animPath))
	{
		if (std::filesystem::is_directory(animPath))
		{
			// ".anim" の拡張子のファイルを全て処理
			for (const auto& entry : std::filesystem::directory_iterator(animPath))
			{

				if (entry.path().extension() == pathExtension)
				{
					std::ifstream animIfs(entry.path().c_str(), std::ios::binary);
					cereal::BinaryInputArchive   deserialization(animIfs);
					Animation anim;
					deserialization(anim);
					animationClips.emplace_back(anim);
				}
			}
		}
	}
	
}

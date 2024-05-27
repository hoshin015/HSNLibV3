#include "SpriteResource.h"


// �Ǝ��`���ŏ����o��
void SpriteResource::OutputSpriteData()
{
	// parentPath �̎擾
	std::filesystem::path fbxFilePath(filePath);
	std::string parentPath = fbxFilePath.parent_path().string();

	std::string fileName = fbxFilePath.stem().string();

	// sprite �̏o��
	{
		// sprite �̏o�̓t�@�C���쐬
		std::string spriteFilename = parentPath + "/" + fileName + ".sprite";

		std::ofstream ofs(spriteFilename.c_str(), std::ios::binary);
		cereal::BinaryOutputArchive  serialization(ofs);

		serialization(filePath, textureSize, animations);
	}

	// animation �̏o��
	//if (!animations.empty())
	//{
	//	// �t�H���_�쐬
	//	std::string animPath = parentPath + "/SprAnims";
	//	RemoveAllContents(animPath);
	//	std::filesystem::create_directory(animPath);
	//
	//	for (auto& anim : animations)
	//	{
	//		// animation�̏o�̓t�@�C���쐬
	//		std::string animFilename = animPath + "/" + anim.name + ".sprAnim";
	//
	//		std::ofstream ofs(animFilename.c_str(), std::ios::binary);
	//		cereal::BinaryOutputArchive  serialization(ofs);
	//
	//		serialization(anim);
	//	}
	//}
}

// �Ǝ��`���Ǎ�
void SpriteResource::LoadSpriteData(const char* filePath)
{
	this->filePath = filePath;

	// --- �g���q�p�ϐ��쐬 ---
	std::string pathExtension = "";

	std::filesystem::path path(filePath);
	std::string parentPath = path.parent_path().string();

	
	{
		std::string spriteFilePath = parentPath + "/" + path.stem().string() + ".sprite";
		std::ifstream ifs(spriteFilePath.c_str(), std::ios::binary);
		cereal::BinaryInputArchive deserialization(ifs);
		deserialization(this->filePath, textureSize, animations);
	}

	// animation �̓Ǎ�
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

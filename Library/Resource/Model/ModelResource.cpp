#include "ModelResource.h"



// �Ǝ��`���ŏ����o��
void ModelResource::OutputModelData()
{
	// parentPath �̎擾
	std::filesystem::path fbxFilePath(filePath);
	std::string parentPath = fbxFilePath.parent_path().string();

	std::string fileName = fbxFilePath.stem().string();

	// sceneView �̏o��
	{
		// model �̏o�̓t�@�C���쐬
		std::string modelFilename = parentPath + "/" + fileName + ".model";
		
		std::ofstream ofs(modelFilename.c_str(), std::ios::binary);
		cereal::BinaryOutputArchive  serialization(ofs);

		serialization(sceneView, scale, coordinateSystemTransformType, coordinateSystemTransform, skeletonSphereCollisions);
	}

	// mesh �̏o��
	if (!meshes.empty())
	{
		// �t�H���_�쐬
		std::string meshPath = parentPath + "/Meshes";
		RemoveAllContents(meshPath);
		std::filesystem::create_directory(meshPath);

		for (auto& mesh : meshes)
		{
			// mesh�̏o�̓t�@�C���쐬
			std::string meshFilename = meshPath + "/" + mesh.meshName + ".mesh";

			std::ofstream ofs(meshFilename.c_str(), std::ios::binary);
			cereal::BinaryOutputArchive  serialization(ofs);

			serialization(mesh);
		}
	}

	// material �̏o��
	if (!materials.empty())
	{
		// �t�H���_�쐬
		std::string materialPath = parentPath + "/Materials";
		RemoveAllContents(materialPath);
		std::filesystem::create_directory(materialPath);

		for (auto [name, material] : materials)	// �\��������
		{
			// mesh�̏o�̓t�@�C���쐬
			std::string materialFilename = materialPath + "/" + name + ".material";

			std::ofstream ofs(materialFilename.c_str(), std::ios::out);
			cereal::JSONOutputArchive  serialization(ofs);

			serialization(material);
		}
	}

	// animation �̏o��
	if (!animationClips.empty())
	{
		// �t�H���_�쐬
		std::string animtionPath = parentPath + "/Anims";
		RemoveAllContents(animtionPath);
		std::filesystem::create_directory(animtionPath);

		for (auto& animation : animationClips)
		{
			// animation�̏o�̓t�@�C���쐬
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

	// --- �g���q�p�ϐ��쐬 ---
	std::string pathExtension = "";

	std::filesystem::path path(filePath);
	std::string parentPath = path.parent_path().string();

	// sceneView �̓Ǎ�
	{
		std::string modelFilePath = parentPath + "/" + path.stem().string() + ".model";
		std::ifstream ifs(modelFilePath.c_str(), std::ios::binary);
		cereal::BinaryInputArchive deserialization(ifs);
		deserialization(sceneView, scale, coordinateSystemTransformType, coordinateSystemTransform, skeletonSphereCollisions);
	}

	// mesh �̓Ǎ�
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

	// material �̓Ǎ�
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

	// animation �̓Ǎ�
	std::string animPath = parentPath + "/Anims/";
	pathExtension = ".anim";

	// Anims �t�@�C�������邩�m�F
	if (std::filesystem::exists(animPath))
	{
		if (std::filesystem::is_directory(animPath))
		{
			// ".anim" �̊g���q�̃t�@�C����S�ď���
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

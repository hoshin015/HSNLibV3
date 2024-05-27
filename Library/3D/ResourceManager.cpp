#include <filesystem>
#include "ResourceManager.h"
#include "../Resource/Model/LoadFbx.h"
#include "../Resource/Model/LoadModel.h"
#include "../Resource/Sprite/LoadImageFormat.h"
#include "../Resource/Sprite/LoadSprite.h"


// ���f�����\�[�X�ǂ�
std::shared_ptr<ModelResource> ResourceManager::LoadModelResource(const char* filename)
{
	// ���f��������
	ModelMap::iterator it = models.find(filename);
	if (it != models.end())
	{
		// �����N(����)���؂�Ă��Ȃ����m�F
		if (!it->second.expired())
		{
			// �ǂݍ��ݍς݂̃��f�����\�[�X��Ԃ�
			return it->second.lock();
		}
	}

	// �`�����`�F�b�N���ĐV�K���f�����\�[�X�쐬���ǂݍ���
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
	

	// �}�b�v�ɓo�^
	models[filename] = model;

	return model;
}

// �X�v���C�g�Ǎ�
std::shared_ptr<SpriteResource> ResourceManager::LoadSpriteResource(const char* filename)
{
	// ���f��������
	SpriteMap::iterator it = sprites.find(filename);
	if (it != sprites.end())
	{
		// �����N(����)���؂�Ă��Ȃ����m�F
		if (!it->second.expired())
		{
			// �ǂݍ��ݍς݂̃��f�����\�[�X��Ԃ�
			return it->second.lock();
		}
	}

	// �`�����`�F�b�N���ĐV�K���f�����\�[�X�쐬���ǂݍ���
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

	// �}�b�v�ɓo�^
	sprites[filename] = sprite;

	return sprite;
}

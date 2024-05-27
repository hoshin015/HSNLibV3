#include "LoadSprite.h"
#include "../../Graphics/Texture.h"

std::shared_ptr<SpriteResource> LoadSprite::Load(const char* spriteFilename)
{
	std::shared_ptr<SpriteResource> spriteResource = std::make_unique<SpriteResource>();

	spriteResource->LoadSpriteData(spriteFilename);

	LoadTextureFromFile(ConvertToWideString(spriteResource->GetFilePath().c_str()).c_str(), spriteResource->GetSrvAddres(), spriteResource->GetSize());

	return spriteResource;
}

std::wstring LoadSprite::ConvertToWideString(const char* narrowString)
{
    // ����narrowString��nullptr�������ꍇ�A���std::wstring��Ԃ�
    if (narrowString == nullptr) {
        return L"";
    }

    // �K�v�ȃo�b�t�@�T�C�Y���擾����
    int numWideChars = MultiByteToWideChar(CP_UTF8, 0, narrowString, -1, nullptr, 0);

    // ���C�h������p�̃o�b�t�@���m��
    std::wstring wideString(numWideChars, L'\0');

    // �}���`�o�C�g����������C�h������ɕϊ�
    MultiByteToWideChar(CP_UTF8, 0, narrowString, -1, &wideString[0], numWideChars);

    return wideString;
}

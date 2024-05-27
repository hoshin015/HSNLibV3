#include "LoadImageFormat.h"
#include "../../Graphics/Texture.h"

// �Ή����Ă���t�H�[�}�b�g�̉摜 �̃��[�h
std::shared_ptr<SpriteResource> LoadImageFormat::Load(const char* imgFilename)
{
	std::shared_ptr<SpriteResource> spriteResource = std::make_shared<SpriteResource>();
	spriteResource->SetFilePath(imgFilename);

	LoadTextureFromFile(ConvertToWideString(imgFilename).c_str(), spriteResource->GetSrvAddres(), spriteResource->GetSize());

	return spriteResource;
}

std::wstring LoadImageFormat::ConvertToWideString(const char* narrowString)
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

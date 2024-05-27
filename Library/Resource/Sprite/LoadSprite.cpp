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
    // もしnarrowStringがnullptrだった場合、空のstd::wstringを返す
    if (narrowString == nullptr) {
        return L"";
    }

    // 必要なバッファサイズを取得する
    int numWideChars = MultiByteToWideChar(CP_UTF8, 0, narrowString, -1, nullptr, 0);

    // ワイド文字列用のバッファを確保
    std::wstring wideString(numWideChars, L'\0');

    // マルチバイト文字列をワイド文字列に変換
    MultiByteToWideChar(CP_UTF8, 0, narrowString, -1, &wideString[0], numWideChars);

    return wideString;
}

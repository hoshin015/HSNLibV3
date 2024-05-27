#pragma once
#include "SpriteResource.h"

class LoadImageFormat
{
private:
	LoadImageFormat() {}
	~LoadImageFormat() {}

public:
	static LoadImageFormat& Instance()
	{
		static LoadImageFormat instance;
		return instance;
	}

	// 対応しているフォーマットの画像 のロード
	std::shared_ptr<SpriteResource> Load(const char* imgFilename);

	std::wstring ConvertToWideString(const char* narrowString);
};
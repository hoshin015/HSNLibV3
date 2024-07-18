#pragma once
#include <DirectXMath.h>
#include <memory>
#include <string>
#include <vector>
#include "../../Library/2D/Sprite.h"

class DamageTextManager
{
private:
	DamageTextManager(){}
	~DamageTextManager(){}

public:
	static DamageTextManager& Instance()
	{
		static DamageTextManager instance;
		return instance;
	}

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Render();

	// クリア
	void Clear();

	struct DamageTextInfo
	{
		std::string text;
		DirectX::XMFLOAT3 worldPosition;
		int type;
		DirectX::XMFLOAT2 screenPosition;
		DirectX::XMFLOAT2 screenPositionBuffer;
		float timer = 0.0f;
	};

	// 文字データ登録
	void Register(DamageTextInfo info);

private:
	float fadeOutTime = 0.7f;
	float lifeTime = 0.8f;

	std::unique_ptr<Sprite> textSprite;
	std::unique_ptr<Sprite> weakSprite;
	std::vector<DamageTextInfo> damageTexts;
};

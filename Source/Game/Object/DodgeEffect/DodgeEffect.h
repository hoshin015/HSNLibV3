#pragma once
#include <vector>
#include "../../../../Library/Resource/Model/ModelResource.h"
#include "../Base/AnimatedModel.h"


class DodgeEffect
{
private:
	DodgeEffect();
	~DodgeEffect(){}

public:
	static DodgeEffect& Instance()
	{
		static DodgeEffect instance;
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

	struct DodgeData
	{
		DirectX::XMFLOAT4X4 trans;
		ModelResource::KeyFrame key;
		float alpha = 1.0f;
	};

	// 登録
	void Register(DodgeData* dodge);

private:
	std::vector<DodgeData*> dodgeData;

	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	std::unique_ptr<AnimatedModel> model;
};



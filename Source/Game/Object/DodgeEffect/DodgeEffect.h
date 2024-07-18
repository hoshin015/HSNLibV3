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

	// ‰Šú‰»
	void Initialize();

	// XV
	void Update();

	// •`‰æ
	void Render();

	// ƒNƒŠƒA
	void Clear();

	struct DodgeData
	{
		DirectX::XMFLOAT4X4 trans;
		ModelResource::KeyFrame key;
		float alpha = 1.0f;
	};

	// “o˜^
	void Register(DodgeData* dodge);

private:
	std::vector<DodgeData*> dodgeData;

	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	std::unique_ptr<AnimatedModel> model;
};



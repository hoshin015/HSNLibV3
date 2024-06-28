#pragma once
#include <memory>
#include "RockMainMesh.h"

class RockEffect
{
private:
	RockEffect() {}
	~RockEffect() {}

public:
	static RockEffect& Instance()
	{
		static RockEffect instance;
		return instance;
	}

	// ������
	void Initialize();

	// �X�V
	void Update();
	// �`��
	void Render(bool isShadow = false);

	// ���b�N�ǉ�
	void Emit(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 vel, DirectX::XMFLOAT2 scaleMinMax);

	struct RockEmitter
	{
		DirectX::XMFLOAT3 position;
		float timer = 0.0f;
		float destoryTime = 10.0f;
		int generateCount = 10;
	};


	std::unique_ptr<RockMainMesh> rockMesh1;
private:
};
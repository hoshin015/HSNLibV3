#pragma once
#include <memory>
#include "LightningMainMesh.h"

class LightningEffect
{
private:
	LightningEffect(){}
	~LightningEffect(){}

public:
	static LightningEffect& Instance()
	{
		static LightningEffect instance;
		return instance;
	}

	// ������
	void Initialize();

	// �X�V
	void Update();
	// �`��
	void Render(bool isShadow = false);

	// ���C�g�j���O�ǉ�
	void Emit();

	struct LightningEmitter
	{
		float timer = 0.0f;
		float destoryTime = 10.0f;
		bool addLightning1 = false;
	};


	std::unique_ptr<LightningMesh1> lightningMesh1;
	std::unique_ptr<LightningMesh1> lightningMesh2;
	std::unique_ptr<LightningMesh1> lightningMesh3;
private:
	float lifeTime = 10.0f;

	float lightning1GenerateTime = 1.2f;

	std::vector<LightningEmitter> lightningEmitters;
};
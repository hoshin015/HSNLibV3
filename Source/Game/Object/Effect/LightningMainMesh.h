#pragma once

#include "../Base/StaticObject.h"

class LightningMainMeshChild;

// lightning ���܂Ƃ߂Ĉ������߂̃N���X
class LightningMainMesh : public StaticObject
{
public:
	LightningMainMesh(const char* filePath) : StaticObject(filePath), path(filePath){}
	~LightningMainMesh() override {}

	// lightningData �𐶐�
	void Initialize();

	// lightningData �̒��g���܂Ƃ߂čX�V
	void Update() override;
	// lightningData �̒��g���܂Ƃ߂ĕ`��
	void Render(bool isShadow = false) override;

private:
	std::string path = "";

	DirectX::XMFLOAT4X4 m[MAX_INSTANCE] = {};	// �p���s��z��
	float e[MAX_INSTANCE] = {};					// �G�~�b�V�u�͔z��

	std::vector<LightningMainMeshChild*> lightningData;
};


// lightning �P�P�̏��
class LightningMainMeshChild : public StaticObject
{
public:
	LightningMainMeshChild(const char* filePath) : StaticObject(filePath) {}
	~LightningMainMeshChild()override {}

	// �X�V
	void Update() override;

	// �`��(override �̕K�v�����邩�炵�Ă��邪�g�p���Ȃ�)
	void Render(bool isShadow = false) override{};

private:
	float lifeTimer = 2.0f;
	float emissivePower = 1.0f;

public:
	float GetlifeTimer() { return lifeTimer; }
	void SetLifeTimer(float t) { lifeTimer = t; }

	float GetEmissivePower() { return emissivePower; }
	void SetEmissivePower(float e) { emissivePower = e; }
};
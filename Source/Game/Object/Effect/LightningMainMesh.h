#pragma once

#include "../Base/StaticObject.h"
#include "../../Library/Easing.h"

class LightningMainMeshChildParent;

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

	DirectX::XMFLOAT4X4 lightning1Transforms[MAX_INSTANCE] = {};	// �p���s��z��
	float lightning1Emissives[MAX_INSTANCE] = {};					// �G�~�b�V�u�͔z��

	std::vector<LightningMainMeshChildParent*> lightningData;



	float lightningMainTimer = 0.0f;	// lightning �̑S�̂��Ǘ�����^�C�}�[
	float lightningMainTime = 5.0f;		// lightning �̐�������

	// �e�G�~�b�^��������
	float lightningMainMeshChild1GenerateTime = 0.6f;
	bool IsLightningMainMeshChild1Generate = true;

	float lightningMainMeshChild2GenerateTime = 0.0f;
	bool IsLightningMainMeshChild2Generate = true;

	float lightningMainMeshChild3GenerateTime = 0.3f;
	bool IsLightningMainMeshChild3Generate = true;
};


// lightning �P�P�̏��
class LightningMainMeshChildParent : public StaticObject
{
public:
	LightningMainMeshChildParent(const char* filePath) : StaticObject(filePath) {}
	virtual ~LightningMainMeshChildParent() = default;

	// �X�V
	void Update() = 0;

	// �`��(override �̕K�v�����邩�炵�Ă��邪�g�p���Ȃ�)
	void Render(bool isShadow = false) override{};

protected:
	float lifeTimer = 0.0f;
	float lifeTime = 2.0f;
	float emissivePower = 1.0f;

public:
	float GetlifeTimer() { return lifeTimer; }
	void SetLifeTimer(float t) { lifeTimer = t; }

	float GetlifeTime() { return lifeTime; }
	void SetLifeTime(float t) { lifeTime = t; }

	float GetEmissivePower() { return emissivePower; }
	void SetEmissivePower(float e) { emissivePower = e; }
};

class LightningMainMeshChild1 : public LightningMainMeshChildParent
{
public:
	LightningMainMeshChild1(const char* filePath) : LightningMainMeshChildParent(filePath){}
	~LightningMainMeshChild1()override {}

	// �X�V
	void Update() override;

private:
	Easing::EasingValue lightning1Scale =
	{
		0.0f, 0.5f,
		4.0f, 0.5f,
	};
};

class LightningMainMeshChild2 : public LightningMainMeshChildParent
{
public:
	LightningMainMeshChild2(const char* filePath) : LightningMainMeshChildParent(filePath) { lifeTime = 0.2f; }
	~LightningMainMeshChild2()override {}

	// �X�V
	void Update() override;
};
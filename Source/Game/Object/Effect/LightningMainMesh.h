#pragma once

#include "../Base/StaticObject.h"
#include "../../Library/Easing.h"


class LightningMainMesh;

// 1��1�̏��
class LightningData : public BaseObject
{
public:
	LightningData()
	{
		funcs.push_back(&LightningData::LightningNoUpdate);
		funcs.push_back(&LightningData::LightningAreaUpdate);
		funcs.push_back(&LightningData::LightningMainUpdate);
	}

	~LightningData() override
	{
	}

	void Update() override;

	void Render(bool isShadow = false) override
	{
	};

	void UpdateTransform();

	// �֐��|�C���^���g�����֐�
	enum class LightningFuncEnum
	{
		NO,
		Area,
		Main,
		END,
	};

	typedef void (LightningData::*UpdateFunc)();
	std::vector<UpdateFunc>       funcs;
	void                          LightningNoUpdate();
	void                          LightningAreaUpdate();
	void                          LightningMainUpdate();

private:
	DirectX::XMMATRIX MSC;

	float lifeTime;
	float lifeTimer = 0.0f;

	float             emissivePower = 1.0f;
	DirectX::XMFLOAT4 color         = {1, 1, 1, 1};

	LightningMainMesh* owner;

	LightningFuncEnum updateType = LightningFuncEnum::NO;

public:
	// lifeTime
	void SetLifeTime(float lt) { lifeTime = lt; }

	// onwer
	void               SetOwner(LightningMainMesh* o) { owner = o; }
	LightningMainMesh* GetOwner() { return owner; }

	// emissivePower
	float GetEmissivePower() { return emissivePower; }
	void  SetEmissivePower(float ep) { this->emissivePower = ep; }

	// color
	DirectX::XMFLOAT4 GetColor() { return color; }
	void              SetColor(DirectX::XMFLOAT4 c) { color = c; }

	// msc
	void SetMSC(DirectX::XMMATRIX MSC);

	// updateType
	void SetUpdateType(LightningFuncEnum t) { updateType = t; }
};

// lightning �܂Ƃ߂�N���X
class LightningMainMesh : public StaticObject
{
public:
	LightningMainMesh(const char* filePath) : StaticObject(filePath)
	{
	}

	~LightningMainMesh() override
	{
	}

	// lightningData �𐶐�
	void Initialize();

	// lightningData �̒��g���܂Ƃ߂čX�V
	void Update() override;
	// lightningData �̒��g���܂Ƃ߂ĕ`��
	void Render(bool isShadow = false) override;

	// �o�^
	void Register(LightningData* lightningData);
	// ����
	void Remove(LightningData* lightningData);
	// �S�폜
	void Clear();

protected:
	std::vector<LightningData*> lightningInfo;
	std::set<LightningData*>    removes;

	DirectX::XMFLOAT4X4 lightningTransforms[MAX_INSTANCE] = {}; // �p���s��z��
	float               lightningEmissives[MAX_INSTANCE]  = {}; // �G�~�b�V�u�͔z��
	DirectX::XMFLOAT4   lightningColors[MAX_INSTANCE]     = {}; // �J���[�z��
};


class LightningMesh1 : public LightningMainMesh
{
public:
	LightningMesh1(const char* filePath) : LightningMainMesh(filePath)
	{
	}

	~LightningMesh1() override
	{
	}
};

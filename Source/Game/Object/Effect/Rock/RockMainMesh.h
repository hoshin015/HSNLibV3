#pragma once

#include "../../Base/StaticObject.h"
#include "../../Library/Easing.h"


class RockMainMesh;

// 1��1�̏��
class RockData : public BaseObject
{
public:
	RockData()
	{
		funcs.push_back(&RockData::RockDefaultUpdate);
		funcs.push_back(&RockData::RockUpUpdate);
	}

	~RockData() override
	{
	}

	void Update() override;

	void Render(bool isShadow = false) override
	{
	};

	void UpdateTransform();

	// �֐��|�C���^���g�����֐�
	enum class RockFuncEnum
	{
		Default,
		Up,
	};

	typedef void (RockData::*UpdateFunc)();
	std::vector<UpdateFunc>       funcs;
	void                          RockDefaultUpdate();
	void                          RockUpUpdate();

private:
	DirectX::XMMATRIX MSC;

	float lifeTime;
	float lifeTimer = 0.0f;

	float             emissivePower = 1.0f;
	DirectX::XMFLOAT4 color         = {1, 1, 1, 1};

	DirectX::XMFLOAT3 velocity = { 0,0,0 };
	float gravity = 15.0f;

	RockMainMesh* owner;

	RockFuncEnum updateType = RockData::RockFuncEnum::Default;

	Easing::EasingValue val1;

public:
	// lifeTime
	void SetLifeTime(float lt) { lifeTime = lt; }

	// velocity
	void SetVeloicty(DirectX::XMFLOAT3 v) { velocity = v; }
	DirectX::XMFLOAT3 GetVelocity() { return velocity; }

	// onwer
	void          SetOwner(RockMainMesh* o) { owner = o; }
	RockMainMesh* GetOwner() { return owner; }

	// emissivePower
	float GetEmissivePower() { return emissivePower; }
	void  SetEmissivePower(float ep) { this->emissivePower = ep; }

	// color
	DirectX::XMFLOAT4 GetColor() { return color; }
	void              SetColor(DirectX::XMFLOAT4 c) { color = c; }

	// msc
	void SetMSC(DirectX::XMMATRIX MSC){ this->MSC = MSC; };

	// updateType
	void SetUpdateType(RockFuncEnum t) { updateType = t; }

	// val1
	void SetVal1(Easing::EasingValue v) { val1 = v; }

	// gravity
	void SetGravity(float g) { gravity = g; }

public:
	bool isDamaged = false;
	float radius = 2.0f;
	float damage = 10.0f;
};

// Rock �܂Ƃ߂�N���X
class RockMainMesh : public StaticObject
{
public:
	RockMainMesh(const char* filePath) : StaticObject(filePath)
	{
	}

	~RockMainMesh() override
	{
	}

	// RockData �𐶐�
	void Initialize();

	// RockData �̒��g���܂Ƃ߂čX�V
	void Update() override;
	// RockData �̒��g���܂Ƃ߂ĕ`��
	void Render(bool isShadow = false) override;

	// �o�^
	void Register(RockData* rockData);
	// ����
	void Remove(RockData* rockData);
	// �S�폜
	void Clear();

protected:
	std::vector<RockData*> rockInfo;
	std::set<RockData*>    removes;

	DirectX::XMFLOAT4X4 rockTransforms[MAX_INSTANCE] = {}; // �p���s��z��
	float               rockEmissives[MAX_INSTANCE]  = {}; // �G�~�b�V�u�͔z��
	DirectX::XMFLOAT4   rockColors[MAX_INSTANCE]     = {}; // �J���[�z��
	DirectX::XMFLOAT4   rockUvScrolls[MAX_INSTANCE]     = {}; // uvScroll�z��
};

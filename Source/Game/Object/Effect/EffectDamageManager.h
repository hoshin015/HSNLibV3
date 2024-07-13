#pragma once
#include <DirectXMath.h>
#include <string>
#include <vector>

class EffectDamageManager
{
private:
	EffectDamageManager(){}
	~EffectDamageManager(){}

public:
	static EffectDamageManager& Instance()
	{
		static EffectDamageManager instance;
		return instance;
	}

	// �G�t�F�N�g�p�@Collision �f�[�^
	struct EffectCollision
	{
		struct SphereData
		{
			DirectX::XMFLOAT3 position;
			float radius;
			float damage;
		};
		std::vector<SphereData> spheres;
	};
	

	// ������
	void Initialize();

	// �X�V
	void Update();

	// �`��
	void Render();

	// �S�폜
	void Clear();

	// �o�^
	void Register(EffectCollision effectCollision);

private:
	std::vector<EffectCollision> effectCollisions;
};
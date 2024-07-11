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


	struct RockEmitter
	{
		DirectX::XMFLOAT3 position;	// �������W
		DirectX::XMFLOAT3 angle;	// �����p�x(���̊p�x�����]���܂���)
		DirectX::XMFLOAT3 scale;	// �����X�P�[��
		float gravity;				// �d��
		float lifeTime = 10.0f;		// ����[s](���̎��Ԃ����Ə����܂�)
	};

	// ���b�N�ǉ�
	void Emit(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 vel, DirectX::XMFLOAT2 scaleMinMax);

	// ���b�N�ǉ�
	void Emit(RockEmitter rockEmitter);

	std::unique_ptr<RockMainMesh> rockMesh1;
private:
};
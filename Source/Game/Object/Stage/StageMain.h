#pragma once
#include <memory>
// --- Game ---
#include "Stage.h"

// �X�e�[�W
class StageMain : public Stage
{
public:
	StageMain(const char* filePath) : Stage(filePath) {}
	~StageMain() override {};

	// �X�V����
	void Update() override;

	// �`�揈��
	void Render(bool isShadow = false) override;

	// ���C�L���X�g
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;
};
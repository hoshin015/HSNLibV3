#pragma once
#include <vector>
// --- Game ---
#include "Stage.h"

// �X�e�[�W�}�l�[�W���[
class StageManager
{
private:
	StageManager() {}
	~StageManager() {}

public:
	// �B��̃C���X�^���X�擾
	static StageManager& Instance()
	{
		static StageManager instance;
		return instance;
	}

	// �X�V����
	void Update();

	// �`�揈��
	void Render(bool isShadow = false);
	void Render(const size_t index, const bool isShadow = false);

	// �X�e�[�W�o�^
	void Register(Stage* stage);

	// �X�e�[�W�S�폜
	void Clear();

	// ���C�L���X�g
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);
	bool RayCast(const size_t index, const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

	// �f�o�b�OGUI�`��
	void DrawDebugImGui();

private:
	std::vector<Stage*> stages;
};
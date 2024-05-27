#pragma once

#include <vector>
#include "Light.h"


// ���C�g�}�l�[�W���[
class LightManager
{
private:
	LightManager() {}
	~LightManager();

public:
	// �B��̃C���X�^���X�擾
	static LightManager& Instance()
	{
		static LightManager instance;
		return instance;
	}



public:
	// ���C�g�o�^
	void Register(Light* light);

	// ���C�g�폜
	void Remove(Light* light);

	// ���C�g�S�폜
	void Clear();

	// ���C�g����ς�
	void PushLightData(LightConstants& cBuffer);
	// �萔�o�b�t�@�̍X�V
	void UpdateConstants();


	// �f�o�b�O���̕\��
	void DrawDebugGui();

	// �f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive();

	// ����
	DirectX::XMFLOAT4 GetAmbientColor() { return ambientColor; }
	void SetAmbientColor(DirectX::XMFLOAT4 ambientColor) { this->ambientColor = ambientColor; }

	// ���C�g�̏��
	int GetLightCount() const { return static_cast<int>(lights.size()); }
	Light* GetLight(int index) const { return lights.at(index); }

private:
	std::vector<Light*> lights;
	DirectX::XMFLOAT4 ambientColor = { 0.2f, 0.2f, 0.2f, 1.0f };
};
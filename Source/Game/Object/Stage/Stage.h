#pragma once
// --- Library ---
#include "../../Library/Math/Collision.h"
// --- Game --- 
#include "../Base/AnimatedObject.h"

// �X�e�[�W
class Stage : public AnimatedObject
{
public:
	Stage(const char* filePath) : AnimatedObject(filePath) {};
	virtual ~Stage() {}

	// �X�V����
	virtual void Update() = 0;

	// �`�揈��
	virtual void Render(bool isShadow) = 0;

	// ���C�L���X�g
	virtual bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) = 0;
};
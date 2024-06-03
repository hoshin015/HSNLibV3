#include "ModelEditorObject.h"
#include "../../../Library/3D/DebugPrimitive.h"

void ModelEditorObject::Update()
{
	// �A�j���[�V�����X�V
	UpdateAnimation();

	// �p���s��X�V
	UpdateTransform();
}

void ModelEditorObject::Render(bool isShadow)
{
	model->Render(transform, &keyFrame, isShadow);

	DrawDebugPrimitive();
}

void ModelEditorObject::DrawDebugPrimitive()
{
	for(auto& sphere : model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex).animSphereCollisions)
	{
		if(sphere.startFrame <= currentKeyFrame && sphere.endFrame >= currentKeyFrame)
		{
			DirectX::XMFLOAT3 pos = GetBonePosition(sphere.bindBoneName);
			DebugPrimitive::Instance().AddSphere(pos, sphere.radius, sphere.color);
		}
	}

	for(auto& bone : model->GetModelResource()->GetSkeletonSphereCollisions())
	{
		DirectX::XMFLOAT3 pos = GetBonePosition(bone.name);
		DebugPrimitive::Instance().AddSphere(pos, bone.radius, bone.color);
	}
}

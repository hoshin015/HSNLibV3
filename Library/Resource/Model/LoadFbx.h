#pragma once
#include <fbxsdk.h>
#include <memory>
#include <map>

#include "ModelResource.h"


class LoadFbx
{
private:
	LoadFbx() {}
	~LoadFbx() {}

public:
	static LoadFbx& Instance()
	{
		static LoadFbx instnace;
		return instnace;
	}

	// fbx �̃��[�h
	std::shared_ptr<ModelResource> Load(const char* fbxFilename, bool triangulate);
private:
	// Fbx �̃f�[�^���i�[����m�[�h
	struct FbxNodeGroup
	{
		struct FbxDataNode
		{
			std::string		name;				// ���O
			uint64_t		uniqueId = 0;		// ��ӂ� id
			int64_t			parentIndex = -1;	// �e�� id
			FbxNodeAttribute::EType attribute = FbxNodeAttribute::EType::eUnknown;
		};
		std::vector<FbxDataNode> fbxDataNodes;

		int64_t GetIndex(int64_t uniqueId) const
		{
			int64_t index = 0;
			// �S�Ă� Node �\���̂��`�F�b�N
			for (const FbxDataNode& node : fbxDataNodes)
			{
				if (node.uniqueId == uniqueId)
				{
					return index;
				}
				index++;
			}

			// ��v���� Node ���Ȃ���� -1 ��Ԃ�
			return -1;
		}
	};

	// �{�[���e���x�\����
	struct BoneInfluence
	{
		uint32_t	boneIndex;	// �{�[���̔ԍ�
		float		boneWeight;	// ���̃{�[���ɑ΂���Ǐ]�x
	};
	using BoneInfluencesPerControlPoint = std::vector<BoneInfluence>;

private:
	FbxNodeGroup fbxNodeGroup;
	
private: 
	// ���b�V���̎擾
	void FetchMesh(FbxScene* fbxScene, ModelResource* modelResource);
	// �}�e���A���̎擾
	void FetchMaterial(FbxScene* fbxScene, ModelResource* modelResource);
	// �{�[���e���x�̎擾
	void FetchBoneInfluence(FbxMesh* fbxMesh, std::vector<BoneInfluencesPerControlPoint>& boneInfluences);
	// �X�P���g���̎擾(���b�V���̒��_�ɉe����^����S�Ẵ{�[���̎擾)
	void FetchSkeleton(FbxMesh* fbxMesh, ModelResource::Skeleton& skeleton, ModelResource* modelResource);
	// �A�j���[�V�����̎擾
	void FetchAnimations(FbxScene* fbxScene, ModelResource* modelResource, float samplingRate);


	// �_�~�[�}�e���A���̍쐬
	void MakeDummyMaterial(ModelResource* modelResource);
public:
	// �A�j���[�V�����ǉ�
	void AppendAnimation(const char* animationFileName, ModelResource* modelResource, float samplingRate);
	// �_�~�[�e�N�X�`���̐���
	void MakeDummyTexture(ID3D11ShaderResourceView** shaderResourceView, DWORD value/*0xAABBGGRR*/, UINT dimension);
};
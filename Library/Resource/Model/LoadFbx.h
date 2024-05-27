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

	// fbx のロード
	std::shared_ptr<ModelResource> Load(const char* fbxFilename, bool triangulate);
private:
	// Fbx のデータを格納するノード
	struct FbxNodeGroup
	{
		struct FbxDataNode
		{
			std::string		name;				// 名前
			uint64_t		uniqueId = 0;		// 一意の id
			int64_t			parentIndex = -1;	// 親の id
			FbxNodeAttribute::EType attribute = FbxNodeAttribute::EType::eUnknown;
		};
		std::vector<FbxDataNode> fbxDataNodes;

		int64_t GetIndex(int64_t uniqueId) const
		{
			int64_t index = 0;
			// 全ての Node 構造体をチェック
			for (const FbxDataNode& node : fbxDataNodes)
			{
				if (node.uniqueId == uniqueId)
				{
					return index;
				}
				index++;
			}

			// 一致する Node がなければ -1 を返す
			return -1;
		}
	};

	// ボーン影響度構造体
	struct BoneInfluence
	{
		uint32_t	boneIndex;	// ボーンの番号
		float		boneWeight;	// そのボーンに対する追従度
	};
	using BoneInfluencesPerControlPoint = std::vector<BoneInfluence>;

private:
	FbxNodeGroup fbxNodeGroup;
	
private: 
	// メッシュの取得
	void FetchMesh(FbxScene* fbxScene, ModelResource* modelResource);
	// マテリアルの取得
	void FetchMaterial(FbxScene* fbxScene, ModelResource* modelResource);
	// ボーン影響度の取得
	void FetchBoneInfluence(FbxMesh* fbxMesh, std::vector<BoneInfluencesPerControlPoint>& boneInfluences);
	// スケルトンの取得(メッシュの頂点に影響を与える全てのボーンの取得)
	void FetchSkeleton(FbxMesh* fbxMesh, ModelResource::Skeleton& skeleton, ModelResource* modelResource);
	// アニメーションの取得
	void FetchAnimations(FbxScene* fbxScene, ModelResource* modelResource, float samplingRate);


	// ダミーマテリアルの作成
	void MakeDummyMaterial(ModelResource* modelResource);
public:
	// アニメーション追加
	void AppendAnimation(const char* animationFileName, ModelResource* modelResource, float samplingRate);
	// ダミーテクスチャの生成
	void MakeDummyTexture(ID3D11ShaderResourceView** shaderResourceView, DWORD value/*0xAABBGGRR*/, UINT dimension);
};
#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "../../MyCereal.h"


//--------------------------------------------------------------
// CoordinateSystemTransform
//--------------------------------------------------------------
enum class CoordinateSystemTransform
{
	RHS_Y_UP,
	LHS_Y_UP,
	RHS_Z_UP,
	LHS_Z_UP,
};

class ModelResource
{
public:
	// --- 定数 ---
	static const int MAX_BONES = 256;
	static const int MAX_BONE_INFLUENCES = 4;

public:
	ModelResource() {}
	virtual ~ModelResource() {}

	// ノード
	struct Node
	{
		std::string		name = "";			// 名前
		uint64_t		uniqueId = 0;		// 一意の id
		int64_t			parentIndex = -1;	// 親の index

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(name, uniqueId, parentIndex);
		}
	};
	// Node をラップ
	struct SceneView
	{
		std::vector<Node> nodes;
		
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(nodes);
		}

		int64_t GetIndex(uint64_t uniqueId) const
		{
			int64_t index = 0;
			// 全ての Node 構造体をチェック
			for (const Node& node : nodes)
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
	// マテリアル情報
	struct Material
	{
		UINT				uniqueId = 0;
		std::string			name = "";
		std::string			textureFilenames[4];
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceViews[4];

		DirectX::XMFLOAT4	Ka = { 0.2f, 0.2f, 0.2f, 1.0f };
		DirectX::XMFLOAT4	Kd = { 0.8f, 0.8f, 0.8f, 1.0f };
		DirectX::XMFLOAT4	Ks = { 1.0f, 1.0f, 1.0f, 1.0f };

		std::string			vertexShaderName;
		std::string			pixelShaderName;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(CEREAL_NVP(uniqueId), CEREAL_NVP(name), CEREAL_NVP(Ka), CEREAL_NVP(Kd), CEREAL_NVP(Ks), CEREAL_NVP(textureFilenames), CEREAL_NVP(vertexShaderName), CEREAL_NVP(pixelShaderName));
		}
	};
	// ボーン情報
	struct Bone
	{
		std::string		name = "";
		uint64_t		uniqueId = 0;
		int64_t			parentIndex = -1;	// bones の中の親のindex
		int64_t			nodeIndex = 0;

		// メッシュ空間からボーン空間への変換行列(ボーン情報で結局必要なのはこれ)
		DirectX::XMFLOAT4X4 offsetTransform =
		{
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1
		};

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(name, uniqueId, parentIndex, nodeIndex, offsetTransform);
		}
	};
	// スケルトン情報(ボーン情報のまとまり)
	struct Skeleton
	{
		std::vector<Bone> bones;

		INT GetIndex(INT uniqueId) const
		{
			INT index = 0;
			// 全ての Node 構造体をチェック
			for (const Bone& bone : bones)
			{
				if (bone.uniqueId == uniqueId)
				{
					return index;
				}
				index++;
			}

			// 一致する Node がなければ -1 を返す
			return -1;
		}

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(bones);
		}
	};

	// 頂点情報
	struct Vertex
	{
		DirectX::XMFLOAT3 position = { 0,0,0 };
		DirectX::XMFLOAT3 normal = { 0,1,0 };
		DirectX::XMFLOAT4 tangent = { 1,0,0,1 };
		DirectX::XMFLOAT2 texcoord = { 0,0 };
		float boneWeights[MAX_BONE_INFLUENCES]{ 1,0,0,0 };
		uint32_t boneIndices[MAX_BONE_INFLUENCES]{};

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(position, normal, tangent, texcoord, boneWeights, boneIndices);
		}
	};
	// サブセット情報
	struct Subset
	{
		std::string		materialName = "";
		uint32_t		startIndex = 0;
		uint32_t		indexCount = 0;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(materialName, startIndex, indexCount);
		}
	};
	// メッシュ情報
	struct Mesh
	{
		std::string meshName = "";
		uint64_t	uniqueId = 0;
		int64_t		nodeIndex = 0;

		// モデル空間の原点からみた姿勢
		DirectX::XMFLOAT4X4 defaultGlobalTransform =
		{
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1,
		};

		std::vector<Vertex>						vertices;
		std::vector<uint32_t>					indices;
		std::vector<Subset>						subsets;
		Skeleton								skeleton;

		Microsoft::WRL::ComPtr<ID3D11Buffer>	vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>	indexBuffer;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(meshName, uniqueId, nodeIndex, defaultGlobalTransform, vertices, indices, subsets, skeleton);
		}
	};
	
	// キーフレームデータ(キーデータの集合)
	struct KeyFrame
	{
		// アニメーションキーデータ
		struct Node
		{
			uint64_t uniqueId = 0;
			std::string name;

			// 'global_transform'はノードのローカル空間からシーンのグローバル空間への変換に使われる。
			DirectX::XMFLOAT4X4 globalTransform =
			{
				1,0,0,0,
				0,1,0,0,
				0,0,1,0,
				0,0,0,1
			};

			DirectX::XMFLOAT3 scaling = { 1,1,1 };
			DirectX::XMFLOAT4 rotation = { 0,0,0,1 };
			DirectX::XMFLOAT3 translation = { 0,0,0 };

			template<class Archive>
			void serialize(Archive& archive)
			{
				archive(uniqueId, name, globalTransform, scaling, rotation, translation);
			}
		};

		std::vector<Node> nodes;	

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(nodes);
		}
	};
	// アニメーションデータ(キーフレームデータの集合)
	struct Animation
	{
		std::string name;
		float samplingRate = 0;
		float secondsLength;

		std::vector<KeyFrame> sequence;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(name, samplingRate, secondsLength, sequence);
		}
	};

public:
	void SetFilePath(std::string path) { filePath = path; }
	std::string GetFilePath() { return filePath; }

	void SetScale(float s) { scale = s; }
	float GetScale() { return scale; }
	const float& GetScale() const { return scale; }

	//--------------------------------------------------------------
	// CoordinateSystemTransform
	//--------------------------------------------------------------
	void SetCoordinateSystemTransformType(CoordinateSystemTransform type) { coordinateSystemTransformType = static_cast<int>(type); }
	const int GetCoordinateSystemTransformType() { return coordinateSystemTransformType; }

	void SetCoordinateSystemTransform(DirectX::XMFLOAT4X4 transform) { coordinateSystemTransform = transform; }
	DirectX::XMFLOAT4X4& GetCoordinateSystemTransform() { return coordinateSystemTransform; }
	const DirectX::XMFLOAT4X4& GetCoordinateSystemTransform() const { return coordinateSystemTransform; }

	// NodeGroup
	SceneView& GetSceneView() { return  sceneView;}
	// Materials
	std::unordered_map<std::string, Material>& GetMaterials() { return materials; }
	// Meshes
	std::vector<Mesh>& GetMeshes() { return meshes; }
	const std::vector<Mesh>& GetMeshes() const { return meshes; }
	// AnimationClips
	std::vector<Animation>& GetAnimationClips() { return animationClips; }
private:
	std::string				filePath;	// このリソースのパス

	// スケール
	float					scale = 1.0f;
	// 軸変換
	int						coordinateSystemTransformType;
	DirectX::XMFLOAT4X4		coordinateSystemTransform;
	
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(scale, coordinateSystemTransformType, coordinateSystemTransform);
	}


	// モデル構築に必要なデータ
	SceneView									sceneView;
	std::unordered_map<std::string, Material>	materials;	// マテリアルの名前(string)でマテリアル(Material)にアクセスできる
	std::vector<Mesh>							meshes;
	std::vector<Animation>						animationClips;

	

public:
	// 独自形式で書き出し
	void OutputModelData();	
	// 独自形式の読込
	void LoadModelData(const char* filePath);
};
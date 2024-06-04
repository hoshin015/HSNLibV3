#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "../../MyCereal.h"
#include "../../Audio/AudioManager.h"


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


//--------------------------------------------------------------
// SkeletonSphereCollision
//--------------------------------------------------------------
struct SkeletonSphereCollision
{
	// ���̃^�C�v(���̃^�C�v�ɂ���ă_���[�W���󂯂����̔{���ƕ\������F��ς���)
	enum class SkeletonType
	{
		Normal,         // �ʏ�
		WeakPoint1,     // ��_�P
		WeakPoint2,     // ��_�Q
		WeakPoint3,     // ��_�R
		HardenedPoint1, // �d���P
		HardenedPoint2, // �d��2
		HardenedPoint3, // �d��3
		NUM
	};

	DirectX::XMFLOAT4 color[static_cast<int>(SkeletonType::NUM)] =
	{
		{1, 1, 1, 1},
		{0, 0, 1, 1},
		{0.3, 0.3, 1, 1},
		{0.6, 0.6, 1, 1},
		{0, 1, 0, 1},
		{0.3, 1, 0.3, 1},
		{0.6, 1, 0.6, 1},
	};

	SkeletonType      skeletonType = SkeletonType::Normal; // ���̃^�C�v
	std::string       name;                                // �m�[�h�̖��O
	float             radius = 1.0f;                       // �����蔻��̔��a
	DirectX::XMFLOAT3 position;                            // �{�[���ɘA�������Ȃ��ꍇ�̍��W

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(skeletonType, name, radius, position);
	}
};

//--------------------------------------------------------------
// AnimSphereCollision
//--------------------------------------------------------------
struct AnimSphereCollision
{
	std::string       name;         // ���O
	float             radius;       // ���̔��a
	int               startFrame;   // �J�n�t���[��
	int               endFrame;     // �I���t���[��
	std::string       bindBoneName; // �{�[���ɘA�������ē������ꍇ�͂����ɖ��O�������
	DirectX::XMFLOAT3 position;     // �{�[���ɘA�������Ȃ��ꍇ�̍��W
	DirectX::XMFLOAT4 color;        // �F

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(name, radius, startFrame, endFrame, bindBoneName, position, color);
	}
};

//--------------------------------------------------------------
// AnimSe
//--------------------------------------------------------------
struct AnimSe
{
	MUSIC_LABEL musicType = MUSIC_LABEL::BGM_TEST; // �Đ�����SE
	std::string name;                              // ���O
	int         startFrame;                        // �J�n�t���[��
	int         endFrame;                          // �I���t���[��	
	bool        isPlay = false;                    // �Đ����ꂽ���ǂ���

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(musicType, name, startFrame, endFrame);
	}
};


class ModelResource
{
public:
	// --- �萔 ---
	static const int MAX_BONES           = 256;
	static const int MAX_BONE_INFLUENCES = 4;

public:
	ModelResource()
	{
	}

	virtual ~ModelResource()
	{
	}

	// �m�[�h
	struct Node
	{
		std::string name        = ""; // ���O
		uint64_t    uniqueId    = 0;  // ��ӂ� id
		int64_t     parentIndex = -1; // �e�� index

		template <class Archive>
		void serialize(Archive& archive)
		{
			archive(name, uniqueId, parentIndex);
		}
	};

	// Node �����b�v
	struct SceneView
	{
		std::vector<Node> nodes;

		template <class Archive>
		void serialize(Archive& archive)
		{
			archive(nodes);
		}

		int64_t GetIndex(uint64_t uniqueId) const
		{
			int64_t index = 0;
			// �S�Ă� Node �\���̂��`�F�b�N
			for (const Node& node : nodes)
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

		int64_t GetUniqueId(std::string name) const
		{
			// �S�Ă� Node �\���̂��`�F�b�N
			for (const Node& node : nodes)
			{
				if (node.name == name)
				{
					return node.uniqueId;
				}
			}

			// ��v���� Node ���Ȃ���� -1 ��Ԃ�
			return -1;
		}
	};

	// �}�e���A�����
	struct Material
	{
		UINT                                             uniqueId = 0;
		std::string                                      name     = "";
		std::string                                      textureFilenames[4];
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViews[4];

		DirectX::XMFLOAT4 Ka = {0.2f, 0.2f, 0.2f, 1.0f};
		DirectX::XMFLOAT4 Kd = {0.8f, 0.8f, 0.8f, 1.0f};
		DirectX::XMFLOAT4 Ks = {1.0f, 1.0f, 1.0f, 1.0f};

		std::string vertexShaderName;
		std::string pixelShaderName;

		template <class Archive>
		void serialize(Archive& archive)
		{
			archive(CEREAL_NVP(uniqueId), CEREAL_NVP(name), CEREAL_NVP(Ka), CEREAL_NVP(Kd), CEREAL_NVP(Ks),
			        CEREAL_NVP(textureFilenames), CEREAL_NVP(vertexShaderName), CEREAL_NVP(pixelShaderName));
		}
	};

	// �{�[�����
	struct Bone
	{
		std::string name        = "";
		uint64_t    uniqueId    = 0;
		int64_t     parentIndex = -1; // bones �̒��̐e��index
		int64_t     nodeIndex   = 0;

		// ���b�V����Ԃ���{�[����Ԃւ̕ϊ��s��(�{�[�����Ō��ǕK�v�Ȃ̂͂���)
		DirectX::XMFLOAT4X4 offsetTransform =
		{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};

		template <class Archive>
		void serialize(Archive& archive)
		{
			archive(name, uniqueId, parentIndex, nodeIndex, offsetTransform);
		}
	};

	// �X�P���g�����(�{�[�����̂܂Ƃ܂�)
	struct Skeleton
	{
		std::vector<Bone> bones;

		INT GetIndex(INT uniqueId) const
		{
			INT index = 0;
			// �S�Ă� Node �\���̂��`�F�b�N
			for (const Bone& bone : bones)
			{
				if (bone.uniqueId == uniqueId)
				{
					return index;
				}
				index++;
			}

			// ��v���� Node ���Ȃ���� -1 ��Ԃ�
			return -1;
		}

		template <class Archive>
		void serialize(Archive& archive)
		{
			archive(bones);
		}
	};

	// ���_���
	struct Vertex
	{
		DirectX::XMFLOAT3 position = {0, 0, 0};
		DirectX::XMFLOAT3 normal   = {0, 1, 0};
		DirectX::XMFLOAT4 tangent  = {1, 0, 0, 1};
		DirectX::XMFLOAT2 texcoord = {0, 0};
		float             boneWeights[MAX_BONE_INFLUENCES]{1, 0, 0, 0};
		uint32_t          boneIndices[MAX_BONE_INFLUENCES]{};

		template <class Archive>
		void serialize(Archive& archive)
		{
			archive(position, normal, tangent, texcoord, boneWeights, boneIndices);
		}
	};

	// �T�u�Z�b�g���
	struct Subset
	{
		std::string materialName = "";
		uint32_t    startIndex   = 0;
		uint32_t    indexCount   = 0;

		template <class Archive>
		void serialize(Archive& archive)
		{
			archive(materialName, startIndex, indexCount);
		}
	};

	// ���b�V�����
	struct Mesh
	{
		std::string meshName  = "";
		uint64_t    uniqueId  = 0;
		int64_t     nodeIndex = 0;

		// ���f����Ԃ̌��_����݂��p��
		DirectX::XMFLOAT4X4 defaultGlobalTransform =
		{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1,
		};

		std::vector<Vertex>   vertices;
		std::vector<uint32_t> indices;
		std::vector<Subset>   subsets;
		Skeleton              skeleton;

		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

		template <class Archive>
		void serialize(Archive& archive)
		{
			archive(meshName, uniqueId, nodeIndex, defaultGlobalTransform, vertices, indices, subsets, skeleton);
		}
	};

	// �L�[�t���[���f�[�^(�L�[�f�[�^�̏W��)
	struct KeyFrame
	{
		// �A�j���[�V�����L�[�f�[�^
		struct Node
		{
			uint64_t    uniqueId = 0;
			std::string name;

			// 'global_transform'�̓m�[�h�̃��[�J����Ԃ���V�[���̃O���[�o����Ԃւ̕ϊ��Ɏg����B
			DirectX::XMFLOAT4X4 globalTransform =
			{
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1
			};

			DirectX::XMFLOAT3 scaling     = {1, 1, 1};
			DirectX::XMFLOAT4 rotation    = {0, 0, 0, 1};
			DirectX::XMFLOAT3 translation = {0, 0, 0};

			template <class Archive>
			void serialize(Archive& archive)
			{
				archive(uniqueId, name, globalTransform, scaling, rotation, translation);
			}
		};

		std::vector<Node> nodes;

		template <class Archive>
		void serialize(Archive& archive)
		{
			archive(nodes);
		}
	};

	// �A�j���[�V�����f�[�^(�L�[�t���[���f�[�^�̏W��)
	struct Animation
	{
		std::string name;
		float       samplingRate = 0;
		float       secondsLength;

		std::vector<KeyFrame>            sequence;
		std::vector<AnimSphereCollision> animSphereCollisions; // �R���W�����f�[�^
		std::vector<AnimSe>              animSes;              // ���f�[�^

		template <class Archive>
		void serialize(Archive& archive)
		{
			archive(name, samplingRate, secondsLength, sequence, animSphereCollisions, animSes);
		}
	};

public:
	void        SetFilePath(std::string path) { filePath = path; }
	std::string GetFilePath() { return filePath; }

	void         SetScale(float s) { scale = s; }
	float        GetScale() { return scale; }
	const float& GetScale() const { return scale; }

	//--------------------------------------------------------------
	// CoordinateSystemTransform
	//--------------------------------------------------------------
	void SetCoordinateSystemTransformType(CoordinateSystemTransform type)
	{
		coordinateSystemTransformType = static_cast<int>(type);
	}

	const int GetCoordinateSystemTransformType() { return coordinateSystemTransformType; }

	void SetCoordinateSystemTransform(DirectX::XMFLOAT4X4 transform) { coordinateSystemTransform = transform; }
	DirectX::XMFLOAT4X4& GetCoordinateSystemTransform() { return coordinateSystemTransform; }
	const DirectX::XMFLOAT4X4& GetCoordinateSystemTransform() const { return coordinateSystemTransform; }

	// NodeGroup
	SceneView& GetSceneView() { return sceneView; }
	// Materials
	std::unordered_map<std::string, Material>& GetMaterials() { return materials; }
	// Meshes
	std::vector<Mesh>&       GetMeshes() { return meshes; }
	const std::vector<Mesh>& GetMeshes() const { return meshes; }
	// AnimationClips
	std::vector<Animation>& GetAnimationClips() { return animationClips; }

	// skeletonSphereCollisions
	std::vector<SkeletonSphereCollision>& GetSkeletonSphereCollisions() { return skeletonSphereCollisions; }

private:
	std::string filePath; // ���̃��\�[�X�̃p�X

	// �X�P�[��
	float scale = 1.0f;
	// ���ϊ�
	int                 coordinateSystemTransformType;
	DirectX::XMFLOAT4X4 coordinateSystemTransform;

	// �R���W�����f�[�^
	std::vector<SkeletonSphereCollision> skeletonSphereCollisions;

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(scale, coordinateSystemTransformType, coordinateSystemTransform, skeletonSphereCollisions);
	}


	// ���f���\�z�ɕK�v�ȃf�[�^
	SceneView                                 sceneView;
	std::unordered_map<std::string, Material> materials; // �}�e���A���̖��O(string)�Ń}�e���A��(Material)�ɃA�N�Z�X�ł���
	std::vector<Mesh>                         meshes;
	std::vector<Animation>                    animationClips;


public:
	// �Ǝ��`���ŏ����o��
	void OutputModelData();
	// �Ǝ��`���̓Ǎ�
	void LoadModelData(const char* filePath);
};

#include <functional>
#include "LoadFbx.h"
#include "../../ErrorLogger.h"
#include "../../ImGui/ConsoleData.h"
#include "../../Graphics/Graphics.h"
#include "../../RegisterNum.h"

using namespace DirectX;

// ===== �⏕�֐� =============================================
#pragma region �⏕�֐�

//--------------------------------------------------------------
// Fbx �̍s�񂩂� DirectX �̍s��ɕϊ�����֐�
//--------------------------------------------------------------
inline XMFLOAT4X4 toXmfloat4x4(const FbxAMatrix& fbxamatrix)
{
	XMFLOAT4X4 xmfloat4x4;
	for (int row = 0; row < 4; ++row)
	{
		for (int column = 0; column < 4; ++column)
		{
			xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
		}
	}
	return xmfloat4x4;
}
inline XMFLOAT3 toXmfloat3(const FbxDouble3& fbxdouble3)
{
	XMFLOAT3 xmfloat3;
	xmfloat3.x = static_cast<float>(fbxdouble3[0]);
	xmfloat3.y = static_cast<float>(fbxdouble3[1]);
	xmfloat3.z = static_cast<float>(fbxdouble3[2]);
	return xmfloat3;
}
inline XMFLOAT4 toXmfloat4(const FbxDouble4& fbxdouble4)
{
	XMFLOAT4 xmfloat4;
	xmfloat4.x = static_cast<float>(fbxdouble4[0]);
	xmfloat4.y = static_cast<float>(fbxdouble4[1]);
	xmfloat4.z = static_cast<float>(fbxdouble4[2]);
	xmfloat4.w = static_cast<float>(fbxdouble4[3]);
	return xmfloat4;
}

#pragma endregion
// ===========================================================



//------------------------------------------------------------
//	Fbx ��ǂݍ���� ModelResource �ɕϊ�����֐�
//------------------------------------------------------------
// fbxFilename	: �ǂݍ��� fbx �̃p�X
// triangulate	: �O�p�����邩�ǂ���
std::shared_ptr<ModelResource> LoadFbx::Load(const char* fbxFilename, bool triangulate)
{
	// �m�[�h�f�[�^���N���A(�����V�[���łQ��ڈȍ~�̃��[�h�̑Ή��̂���)
	fbxNodeGroup.fbxDataNodes.clear();

	std::shared_ptr<ModelResource> modelResource = std::make_unique<ModelResource>();

	// fbx �t�@�C���p�X�̕ۑ�
	modelResource->SetFilePath(fbxFilename);

	// ���ϊ�
	modelResource->SetCoordinateSystemTransformType(CoordinateSystemTransform::LHS_Y_UP);
	modelResource->SetCoordinateSystemTransform({ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 });
	
	// FbxManager �̍쐬
	FbxManager* fbxManager = FbxManager::Create();
	// FbxScene �̍쐬
	FbxScene* fbxScene = FbxScene::Create(fbxManager, "");
	// FbxImporter �̍쐬
	FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");

	// �t�@�C���̏�����
	bool importStatus = false;
	importStatus = fbxImporter->Initialize(fbxFilename);
	_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

	// �t�@�C���̃C���|�[�g
	importStatus = fbxImporter->Import(fbxScene);
	_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

	// 3D���f���̕s�v�ȃ|���S���̏���
	FbxGeometryConverter fbxConverter(fbxManager);
	if (triangulate)
	{
		// �|���S�����R�p�`�ɕϊ�����(�d������)
		fbxConverter.Triangulate(fbxScene, true/*replace*/, false/*legacy*/);
		fbxConverter.RemoveBadPolygonsFromMeshes(fbxScene);
	}

	std::function<void(FbxNode*)> traverse
	{
		[&](FbxNode* fbxNode)
		{
			// node �� emplace_back()
			FbxNodeGroup::FbxDataNode& node = fbxNodeGroup.fbxDataNodes.emplace_back();
			// node �̑����擾
			node.attribute = fbxNode->GetNodeAttribute() ? fbxNode->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eBoundary;
			// node �̖��O�擾
			node.name = fbxNode->GetName();
			// node �� id �擾
			node.uniqueId = fbxNode->GetUniqueID();
			// �e node �̃C���f�b�N�X�擾
			node.parentIndex = fbxNodeGroup.GetIndex(fbxNode->GetParent() ? fbxNode->GetParent()->GetUniqueID() : 0);

			ModelResource::Node& nodeData = modelResource->GetSceneView().nodes.emplace_back();
			nodeData.name = node.name;
			nodeData.uniqueId = node.uniqueId;
			nodeData.parentIndex = node.parentIndex;


			// �q node �� traverse()
			for (int childIndex = 0; childIndex < fbxNode->GetChildCount(); ++childIndex)
			{
				traverse(fbxNode->GetChild(childIndex));
			}
		}
	};
	traverse(fbxScene->GetRootNode());

#if 1
	ConsoleData::Instance().logs.push_back("Load Fbx : " + static_cast<std::string>(fbxFilename));
	for (const FbxNodeGroup::FbxDataNode& node : fbxNodeGroup.fbxDataNodes)
	{
		FbxNode* fbxNode{ fbxScene->FindNodeByName(node.name.c_str()) };

		std::string nodeName = fbxNode->GetName();
		uint64_t uid = fbxNode->GetUniqueID();
		uint64_t parentUid = fbxNode->GetParent() ? fbxNode->GetParent()->GetUniqueID() : 0;
		uint64_t parentIndex = node.parentIndex;
		int32_t type = fbxNode->GetNodeAttribute() ? fbxNode->GetNodeAttribute()->GetAttributeType() : 0;		std::stringstream debugString;
		debugString << " : nodeName -" << nodeName << ": uId -" << uid << " : parentUid - " << parentUid << " : pIndex" << parentIndex << " : type - " << type << "\n";
		OutputDebugStringA(debugString.str().c_str());
		ConsoleData::Instance().logs.push_back(debugString.str());
	}
#endif

	// ���b�V���̎擾
	FetchMesh(fbxScene, modelResource.get());
	// �}�e���A���̎擾
	FetchMaterial(fbxScene, modelResource.get());
	// �A�j���[�V�����̎擾
	FetchAnimations(fbxScene, modelResource.get(), 30);


	// �j��
	fbxImporter->Destroy();
	fbxScene->Destroy();
	fbxManager->Destroy();

	return modelResource;
}

//------------------------------------------------------------
//	fbxScene ���� mesh �f�[�^���擾
//------------------------------------------------------------
// fbxScene			: mesh �f�[�^�̎擾���� FbxScene
// modelResource	: �擾�������b�V���f�[�^�̊i�[��
void LoadFbx::FetchMesh(FbxScene* fbxScene, ModelResource* modelResource)
{
	// ���ׂĂ� FbxDataNode �\���� ���`�F�b�N
	for (const FbxNodeGroup::FbxDataNode& node : fbxNodeGroup.fbxDataNodes)
	{
		// node�\���� �̃^�C�v�� mesh �ȊO�Ȃ� continue
		if (node.attribute != FbxNodeAttribute::EType::eMesh)
		{
			continue;
		}
		// fbxNode �̎擾
		FbxNode* fbxNode = fbxScene->FindNodeByName(node.name.c_str());
		// fbxMesh �̎擾
		FbxMesh* fbxMesh = fbxNode->GetMesh();

		// mesh�\���̂��쐬�� meshes �� push
		ModelResource::Mesh& mesh = modelResource->GetMeshes().emplace_back();
		mesh.meshName = fbxMesh->GetNode()->GetName();
		mesh.uniqueId = fbxMesh->GetNode()->GetUniqueID();
		mesh.defaultGlobalTransform = toXmfloat4x4(fbxMesh->GetNode()->EvaluateGlobalTransform());	// ���f����Ԃ̌��_����݂��p��
		mesh.nodeIndex = modelResource->GetSceneView().GetIndex(mesh.uniqueId);


		// ���_�ɑ΂���{�[���̉e���x�x�N�^�[�̍쐬���ď����擾
		std::vector<BoneInfluencesPerControlPoint> boneInfluences;
		FetchBoneInfluence(fbxMesh, boneInfluences);
		// bone �f�[�^�̎擾
		FetchSkeleton(fbxMesh, mesh.skeleton, modelResource);


		// Subset �̃x�N�^�[�� mesh �� subsets ����쐬
		std::vector<ModelResource::Subset>& subsets = mesh.subsets;
		// material �̐����擾
		const int materialCount = fbxMesh->GetNode()->GetMaterialCount();
		// subsets �̃T�C�Y���}�e���A���̐��ɂ���
		subsets.resize(materialCount > 0 ? materialCount : 1);

		for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex)
		{
			// index �� material ���擾
			const FbxSurfaceMaterial* fbxMaterial = fbxMesh->GetNode()->GetMaterial(materialIndex);
			// subset �� name ���Z�b�g
			subsets.at(materialIndex).materialName = fbxMaterial->GetName();
		}
		if (materialCount > 0)
		{
			// mesh �̃|���S���̐����擾
			const int polygonCount = fbxMesh->GetPolygonCount();
			for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
			{
				// �|���S���C���f�b�N�X�ɑΉ����� material �� index ���擾
				// �|���S����index��n����matrial��index��Ԃ��Ă����
				// �|���S��x3����indexCount�����Z����
				const int materialIndex = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex);
				// �Ή����� materialIndex �� indexCount �𒸓_�̐�(3)���Z����
				subsets.at(materialIndex).indexCount += 3;
			}
			// subsets ���� startIndex �̒���
			uint32_t offset{ 0 };
			for (ModelResource::Subset& subset : subsets)
			{
				subset.startIndex = offset;
				offset += subset.indexCount;
				subset.indexCount = 0;
			}
		}
		else
		// �����ł���Ă�̈Ⴄ�C������(FetchMaterial�ɓ��ꂽ�ق����Y��Ȃ̂ł�)
		{
			// �}�e���A�����Ȃ���΃_�~�[�}�e���A�����쐬
			MakeDummyMaterial(modelResource);
		}

		// �|���S�������擾
		const int polygonCount = fbxMesh->GetPolygonCount();
		mesh.vertices.resize(polygonCount * 3LL);	// vertices �̃T�C�Y���|���S���̂R�{�Ƀ��T�C�Y
		mesh.indices.resize(polygonCount * 3LL);	// indices �̃T�C�Y���|���S���̂R�{�Ƀ��T�C�Y

		// UVSet �̖��O���擾
		FbxStringList uvNames;
		fbxMesh->GetUVSetNames(uvNames);

		// �R���g���[���|�C���g(���_���W)���擾
		const FbxVector4* controlPoints = fbxMesh->GetControlPoints();

		// �|���S���̐������J��Ԃ�
		for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
		{
			// �|���S���C���f�b�N�X�ɑΉ����� material �� index ���擾
			const int materialIndex = materialCount > 0 ? fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex) : 0;
			// �Ή����� materialIndex �� subset ���擾
			ModelResource::Subset& subset = subsets.at(materialIndex);
			const uint32_t offset = subset.startIndex + subset.indexCount;

			// ���_�̐�(3)�����J��Ԃ�
			for (int positionInPolygon = 0; positionInPolygon < 3; ++positionInPolygon)
			{
				// ���݂̒��_�� index ���v�Z
				const int vertexIndex = polygonIndex * 3 + positionInPolygon;

				// ���_���(vertex)�̍쐬
				ModelResource::Vertex vertex;
				const int polygonVertex = fbxMesh->GetPolygonVertex(polygonIndex, positionInPolygon);	// polygonIndex �̃|���S���� positionInPolygon �̒��_�� index ���擾
				vertex.position.x = static_cast<float>(controlPoints[polygonVertex][0]);	// index �ɑΉ��������_���W���Z�b�g
				vertex.position.y = static_cast<float>(controlPoints[polygonVertex][1]);
				vertex.position.z = static_cast<float>(controlPoints[polygonVertex][2]);

				// ���_�ɑΉ����� �{�[���̃C���f�b�N�X�ƃE�F�C�g���Z�b�g����
				const BoneInfluencesPerControlPoint& influencesPerControlPoint = boneInfluences.at(polygonVertex);
				for (size_t influenceIndex = 0; influenceIndex < influencesPerControlPoint.size(); ++influenceIndex)
				{
					if (influenceIndex < ModelResource::MAX_BONE_INFLUENCES)
					{
						// vertex �� weight �� index �� influencesPerControlPoint �̒l���Z�b�g����
						vertex.boneWeights[influenceIndex] = influencesPerControlPoint.at(influenceIndex).boneWeight;
						vertex.boneIndices[influenceIndex] = influencesPerControlPoint.at(influenceIndex).boneIndex;
					}
				}

				// mesh �ɖ@����񂪂���Ȃ� vertex �ɃZ�b�g����
				if (fbxMesh->GetElementNormalCount() > 0)
				{
					FbxVector4 normal;
					fbxMesh->GetPolygonVertexNormal(polygonIndex, positionInPolygon, normal);
					vertex.normal.x = static_cast<float>(normal[0]);
					vertex.normal.y = static_cast<float>(normal[1]);
					vertex.normal.z = static_cast<float>(normal[2]);
				}
				// mesh ��UV(�e�N�X�`�����W)��񂪂���Ȃ� vertex �ɃZ�b�g����
				if (fbxMesh->GetElementUVCount() > 0)
				{
					FbxVector2 uv;
					bool unmappedUv;
					fbxMesh->GetPolygonVertexUV(polygonIndex, positionInPolygon, uvNames[0], uv, unmappedUv);
					vertex.texcoord.x = static_cast<float>(uv[0]);
					vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
				}
				// �@���x�N�g���̒l�̃Z�b�g(�@���x�N�g���������Ȃ��ꍇ�� GenerateTangentsData �Ő���)
				if (fbxMesh->GenerateTangentsData(0, false))
				{
					const FbxGeometryElementTangent* tangent = fbxMesh->GetElementTangent(0);
					vertex.tangent.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[0]);
					vertex.tangent.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[1]);
					vertex.tangent.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[2]);
					vertex.tangent.w = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[3]);
				}

				// mesh �� ���_���x�N�^�[(vertices)�ɍ쐬�������_���(vertex)���Z�b�g����
				mesh.vertices.at(vertexIndex) = std::move(vertex);
				// mesh �� �C���f�b�N�X���x�N�^�[(indices)�Ɍ��݂� index ���Z�b�g����
				mesh.indices.at(static_cast<size_t>(offset) + positionInPolygon) = vertexIndex;
				subset.indexCount++;
			}
		}
	}
}

//------------------------------------------------------------
//	fbxScene ���� material �f�[�^���擾
//------------------------------------------------------------
// fbxScene			: mesh �f�[�^�̎擾���� FbxScene
// modelResource	: �擾�����}�e���A���f�[�^�̊i�[��
void LoadFbx::FetchMaterial(FbxScene* fbxScene, ModelResource* modelResource)
{
	// ���ׂĂ� FbxDataNode �\���� ���`�F�b�N
	for (const FbxNodeGroup::FbxDataNode& node : fbxNodeGroup.fbxDataNodes)
	{
		if (node.attribute != FbxNodeAttribute::EType::eMesh) continue;

		// fbxNode �̎擾
		const FbxNode* fbxNode = fbxScene->FindNodeByName(node.name.c_str());

		// �擾���� node ������ material �̐����擾
		const int materialCount = fbxNode->GetMaterialCount();
		for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex)
		{
			// �}�e���A���̓��b�V���������Ă��邽�� mesh �ȊO���ׂȂ��Ă悢
			const FbxSurfaceMaterial* fbxMaterial = fbxNode->GetMaterial(materialIndex);

			// �}�e���A���̍쐬
			ModelResource::Material material;
			material.name = fbxMaterial->GetName();

			// Diffuse �v���p�e�B���擾
			FbxProperty fbxProperty;
			fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
			if (fbxProperty.IsValid())
			{
				const FbxDouble3 color = fbxProperty.Get<FbxDouble3>();
				material.Kd.x = static_cast<float>(color[0]);
				material.Kd.y = static_cast<float>(color[1]);
				material.Kd.z = static_cast<float>(color[2]);
				material.Kd.w = 1.0f;

				// fbxProperty (Diffuse) �Ɋ֘A�Â���ꂽ�e�N�X�`���̎擾
				const FbxFileTexture* fbxTexture = fbxProperty.GetSrcObject<FbxFileTexture>();
				material.textureFilenames[_deffuseTexture] = fbxTexture ? fbxTexture->GetRelativeFileName() : "";
			}
			// Specular �v���p�e�B���擾
			fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
			if (fbxProperty.IsValid())
			{
				const FbxDouble3 color = fbxProperty.Get<FbxDouble3>();
				material.Ks.x = static_cast<float>(color[0]);
				material.Ks.y = static_cast<float>(color[1]);
				material.Ks.z = static_cast<float>(color[2]);
				material.Ks.w = 1.0f;

				// fbxProperty �Ɋ֘A�Â���ꂽ�e�N�X�`���̎擾
				const FbxFileTexture* fbxTexture = fbxProperty.GetSrcObject<FbxFileTexture>();
				material.textureFilenames[_specularTexture] = fbxTexture ? fbxTexture->GetRelativeFileName() : "";
			}
			// Ambient �v���p�e�B���擾
			fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sAmbient);
			if (fbxProperty.IsValid())
			{
				const FbxDouble3 color = fbxProperty.Get<FbxDouble3>();
				material.Ka.x = static_cast<float>(color[0]);
				material.Ka.y = static_cast<float>(color[1]);
				material.Ka.z = static_cast<float>(color[2]);
				material.Ka.w = 1.0f;

				// fbxProperty �Ɋ֘A�Â���ꂽ�e�N�X�`���̎擾
				//const FbxFileTexture* fbxTexture = fbxProperty.GetSrcObject<FbxFileTexture>();
				//material.textureFilenames[] = fbxTexture ? fbxTexture->GetRelativeFileName() : "";
			}

			modelResource->GetMaterials().emplace(material.name, std::move(material));
		}
	}
}


//------------------------------------------------------------
//	fbxMesh ���� �e���_�ɑ΂��� bone �̉e���͂��擾
//------------------------------------------------------------
// fbxMesh			: ���_�f�[�^ bone �f�[�^�̎擾���� FbxMesh
// boneInfluences	: �擾�����e���̓f�[�^�̊i�[��
void LoadFbx::FetchBoneInfluence(FbxMesh* fbxMesh, std::vector<BoneInfluencesPerControlPoint>& boneInfluences)
{
	// --- �{�[���e���x�\���̂̃T�C�Y��mesh�̎����_�̐��ɕύX ---
	const int controlPointsCount = fbxMesh->GetControlPointsCount();
	boneInfluences.resize(controlPointsCount);

	// ���ׂĂ� deformer (FbxSkin) ������ 
	const int skinCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int skinIndex = 0; skinIndex < skinCount; ++skinIndex)
	{
		// �������� deformer �̎擾	
		const FbxSkin* fbxSkin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(skinIndex, FbxDeformer::eSkin));

		// ���� deformer �����S�Ă� cluster ���������� --- (���ׂẴ{�[���̏���)
		const int clusterCount = fbxSkin->GetClusterCount();
		for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)
		{
			// ���݂� index �� cluster ���擾 --- (��������{�[���̎擾)
			const FbxCluster* fbxCluster = fbxSkin->GetCluster(clusterIndex);

			// cluster ���e����^���钸�_�̐����擾(1�� cluster ���R�̒��_�ɉe����^����ꍇ�R��Ԃ�) --- (�{�[�����e����^����S�Ă̒��_������)
			const int controlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();

			for (int controlPointIndicesIndex = 0; controlPointIndicesIndex < controlPointIndicesCount; ++controlPointIndicesIndex)
			{
				// index �� weight �� push ����
				int controlPointIndex = fbxCluster->GetControlPointIndices()[controlPointIndicesIndex];
				double controlPointWeight = fbxCluster->GetControlPointWeights()[controlPointIndicesIndex];
				BoneInfluence& boneInfluence = boneInfluences.at(controlPointIndex).emplace_back();
				boneInfluence.boneIndex = static_cast<uint32_t>(clusterIndex);
				boneInfluence.boneWeight = static_cast<float>(controlPointWeight);
			}
		}
	}
}

//------------------------------------------------------------
//	fbxMesh ���� bone �f�[�^���擾
//------------------------------------------------------------
// fbxMesh		: bone �f�[�^�̎擾���� FbxMesh
// skeleton		: �擾�����{�[���f�[�^�̊i�[��
void LoadFbx::FetchSkeleton(FbxMesh* fbxMesh, ModelResource::Skeleton& skeleton, ModelResource* modelResource)
{
	// ���ׂĂ� deformer (FbxSkin) ������ 
	const int deformerCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int deformerIndex = 0; deformerIndex < deformerCount; ++deformerIndex)
	{
		// �������� deformer �̎擾	
		FbxSkin* skin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
	
		// ���� deformer �����S�Ă� cluster ���������� --- (���ׂẴ{�[���̏���)
		const int clusterCount = skin->GetClusterCount();
	
		// �������Ă��郁�b�V��������Skeleton��Bone�̑傫�����{�[���̐��ɂ���
		skeleton.bones.resize(clusterCount);
	
		for (int clusterIndex = 0; clusterIndex < clusterCount; clusterIndex++)
		{
			// ���݂� index �� cluster ���擾 --- (��������{�[���̎擾)
			FbxCluster* cluster = skin->GetCluster(clusterIndex);
	
			// �{�[���̏����擾���ăZ�b�g
			ModelResource::Bone& bone = skeleton.bones.at(clusterIndex);
			bone.name = cluster->GetLink()->GetName();
			bone.uniqueId = cluster->GetLink()->GetUniqueID();
			bone.parentIndex = skeleton.GetIndex(cluster->GetLink()->GetParent()->GetUniqueID());
			bone.nodeIndex = modelResource->GetSceneView().GetIndex (bone.uniqueId);
	
			// ���b�V���̏����O���[�o���s����擾
			FbxAMatrix referenceGlobalInitPosition;
			cluster->GetTransformMatrix(referenceGlobalInitPosition);
	
			// �{�[���̏����O���[�o���s����擾
			FbxAMatrix clusterGlobalInitPosition;
			cluster->GetTransformLinkMatrix(clusterGlobalInitPosition);
	
			// ���b�V����Ԃ̒��_���W���O���[�o����Ԃ̒��_���W�ɂ���s���
			// �O���[�o����Ԃ̒��_���W���{�[����Ԃ̒��_���W�ɂ���s����|���Z���邱�Ƃ�
			// ���b�V����Ԃ���{�[����Ԃ֕ϊ�����I�t�Z�b�g�s����쐬����
			bone.offsetTransform = toXmfloat4x4(clusterGlobalInitPosition.Inverse() * referenceGlobalInitPosition);
	
			// �X�L�j���O��̒��_���W = ���b�V����Ԃ̒��_���W * �I�t�Z�b�g�s��
		}
	}
}

//------------------------------------------------------------
//	fbxScene ���� Animation �f�[�^���擾
//------------------------------------------------------------
// fbxMesh			: bone �f�[�^�̎擾���� FbxMesh
// modelResource	: �擾�����{�[���f�[�^�̊i�[��
// samplingRate		: �A�j���[�V�����̃T���v�����O�� (1 / samplingRate) �b�ōs��
void LoadFbx::FetchAnimations(FbxScene* fbxScene, ModelResource* modelResource, float samplingRate)
{
	// �S�ẴA�j���[�V�����X�^�b�N�̖��O�̎擾
	FbxArray<FbxString*> animationStackNames;
	fbxScene->FillAnimStackNameArray(animationStackNames);
	
	
	// �S�ẴA�j���[�V�����X�^�b�N����������
	const int animationStackCount = animationStackNames.GetCount();
	
	for (int animationStackIndex = 0; animationStackIndex < animationStackCount; animationStackIndex++)
	{
		// �S�ẴA�j���[�V�����X�^�b�N�̖��O�̎擾
		FbxArray<FbxString*> animationStackNames;
		fbxScene->FillAnimStackNameArray(animationStackNames);

		// �S�ẴA�j���[�V�����X�^�b�N����������
		const int animationStackCount = animationStackNames.GetCount();

		for (int animationStackIndex = 0; animationStackIndex < animationStackCount; animationStackIndex++)
		{
			// --- ��������e�A�j���[�V�����X�^�b�N�ɑ΂��鏈�� ---

			// �o�͐�� animationClips �ɓ������O������΃X�L�b�v
			bool isContinue = false;
			for (int i = 0; i < modelResource->GetAnimationClips().size(); i++)
			{
				std::string animationStackName = animationStackNames[animationStackIndex]->Buffer();
				std::string animationName = modelResource->GetAnimationClips().at(i).name;
				if (animationStackName == animationName) isContinue = true;
			}
			if (isContinue) continue;

			// animeClip�̖��O�� �A�j���[�V�����X�^�b�N���������
			ModelResource::Animation& animationClip = modelResource->GetAnimationClips().emplace_back();
			animationClip.name = animationStackNames[animationStackIndex]->Buffer();

			// index �̃A�j���[�V�����X�^�b�N���擾
			FbxAnimStack* animationStack = fbxScene->FindMember<FbxAnimStack>(animationClip.name.c_str());
			fbxScene->SetCurrentAnimationStack(animationStack);

			// --- �T���v�����O���[�g�ƃT���v�����O�Ԋu�̐ݒ� ---

			// �^�C�����[�h�̎擾�i���Ԃ̕\�����@�̎w��j
			const FbxTime::EMode timeMode = fbxScene->GetGlobalSettings().GetTimeMode();
			// �^�C�����[�h�ɑΉ�����P�b�̕\�������邽�߂̕ϐ���p��
			FbxTime oneSecond;
			oneSecond.SetTime(0, 0, 1, 0, 0, timeMode);
			// �T���v�����O���[�g�̎w��
			animationClip.samplingRate = samplingRate > 0 ? samplingRate : static_cast<float>(oneSecond.GetFrameRate(timeMode));
			// �T���v�����O�Ԋu�̌v�Z(���̕b�����ƂɃT���v�����O���s��)
			const FbxTime samplingInterval = static_cast<FbxLongLong>(oneSecond.Get() / animationClip.samplingRate);
			// �A�j���[�V�����̊J�n���ԂƏI�����Ԃ̎擾
			const FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(animationClip.name.c_str());
			const FbxTime startTime = takeInfo->mLocalTimeSpan.GetStart();
			const FbxTime stopTime = takeInfo->mLocalTimeSpan.GetStop();

			// �A�j���[�V�����I�����Ԃ̕ۑ�
			double doubleSeconds = stopTime.GetSecondDouble() - startTime.GetSecondDouble();
			//double doubleSeconds = fbxScene->GetTakeInfo(animationClip.name.c_str())->mLocalTimeSpan.GetStop().GetSecondDouble();
			animationClip.secondsLength = static_cast<float>(doubleSeconds);

			// --- �e�L�[�t���[���ɑ΂��鏈�� ---
			// �A�j���[�V�����̊J�n���Ԃ���I�����Ԃ܂ŁA�T���v�����O�Ԋu���ƂɃL�[�t���[���𐶐����Ă���
			for (FbxTime time = startTime; time < stopTime; time += samplingInterval)
			{
				// �L�[�t���[���𐶐����ăV�[�N�G���X��emplace_back���Ă���
				ModelResource::KeyFrame& keyFrame = animationClip.sequence.emplace_back();

				// �L�[�t���[���� node �̐��� ���̃��f��������node�̐��ɂ���
				const size_t nodeCount = modelResource->GetSceneView().nodes.size();
				keyFrame.nodes.resize(nodeCount);

				// ���ׂĂ� node �̏���
				for (size_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
				{
					FbxNode* fbxNode = fbxScene->FindNodeByName(modelResource->GetSceneView().nodes.at(nodeIndex).name.c_str());
					if (fbxNode)
					{
						ModelResource::KeyFrame::Node& node = keyFrame.nodes.at(nodeIndex);

						//	uniqueId �Ɩ��O�̃Z�b�g
						node.uniqueId = fbxNode->GetUniqueID();
						node.name = fbxNode->GetName();

						// node �� �O���[�o���ϊ��s��ɂ����
						node.globalTransform = toXmfloat4x4(fbxNode->EvaluateGlobalTransform(time));

						// node ���[�J�� �� scaling rotation translation �ɂ��ꂼ��ϊ��s��������
						const FbxAMatrix& localTransform = fbxNode->EvaluateLocalTransform(time);
						node.scaling = toXmfloat3(localTransform.GetS());
						node.rotation = toXmfloat4(localTransform.GetQ());
						node.translation = toXmfloat3(localTransform.GetT());
					}
				}
			}
		}
	}
	// �S�ẴA�j���[�V�����X�^�b�N���̉��
	for (int animationStackIndex = 0; animationStackIndex < animationStackCount; animationStackIndex++)
	{
		delete animationStackNames[animationStackIndex];
	}
}

//------------------------------------------------------------
//	fbxScene ���� Animation �f�[�^���擾���Ēǉ����s��
//------------------------------------------------------------
// animationFileName	: �擾���� fbx �̃p�X
// modelResource		: �擾�����{�[���f�[�^�̊i�[��
// samplingRate			: �A�j���[�V�����̃T���v�����O�� (1 / samplingRate) �b�ōs��
void LoadFbx::AppendAnimation(const char* animationFileName, ModelResource*  modelResource, float samplingRate)
{
	FbxManager* fbxManager = FbxManager::Create();
	FbxScene* fbxScene = FbxScene::Create(fbxManager, "");

	FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
	bool importStatus = false;
	importStatus = fbxImporter->Initialize(animationFileName);
	_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());
	importStatus = fbxImporter->Import(fbxScene);
	_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

	FetchAnimations(fbxScene, modelResource, samplingRate);

	fbxManager->Destroy();
}

//------------------------------------------------------------
//	�_�~�[�}�e���A���̐���
//------------------------------------------------------------
void LoadFbx::MakeDummyMaterial(ModelResource* modelResource)
{
	ModelResource::Material material;
	material.name = "dummyMaterial";
	material.Kd.x = 1.0f;
	material.Kd.y = 1.0f;
	material.Kd.z = 1.0f;
	material.Kd.w = 1.0f;

	modelResource->GetMaterials().emplace(material.name, std::move(material));
}

//------------------------------------------------------------
//	�_�~�[�e�N�X�`���̐���
//------------------------------------------------------------
void LoadFbx::MakeDummyTexture(ID3D11ShaderResourceView** shaderResourceView, DWORD value, UINT dimension)
{
	// --- Graphics �擾 ---
	Graphics* gfx = &Graphics::Instance();
	ID3D11Device* device = gfx->GetDevice();

	//--- �_�~�[�e�N�X�`���̐��� ----
	HRESULT hr = S_OK;

	D3D11_TEXTURE2D_DESC texture2dDesc{};
	texture2dDesc.Width = dimension;
	texture2dDesc.Height = dimension;
	texture2dDesc.MipLevels = 1;
	texture2dDesc.ArraySize = 1;
	texture2dDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture2dDesc.SampleDesc.Count = 1;
	texture2dDesc.SampleDesc.Quality = 0;
	texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	size_t texels = dimension * dimension;
	std::unique_ptr<DWORD[]> sysmem{ std::make_unique<DWORD[]>(texels) };
	for (size_t i = 0; i < texels; ++i) sysmem[i] = value;

	D3D11_SUBRESOURCE_DATA subresourceData{};
	subresourceData.pSysMem = sysmem.get();
	subresourceData.SysMemPitch = sizeof(DWORD) * dimension;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
	hr = device->CreateTexture2D(&texture2dDesc, &subresourceData, &texture2d);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
	shaderResourceViewDesc.Format = texture2dDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(texture2d.Get(), &shaderResourceViewDesc, shaderResourceView);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}


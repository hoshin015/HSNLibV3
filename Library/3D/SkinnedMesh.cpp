//#include <functional>
//#include <sstream>
//#include <fstream>
//#include <filesystem>
//#include "SkinnedMesh.h"
//#include "../Graphics/Graphics.h"
//#include "../Graphics/Shader.h"
//#include "../Graphics/Texture.h"
//#include "../ImGui/ConsoleData.h"	
//#include "../ErrorLogger.h"
//
//
////--------------------------------------------------------------
//// �{�[���e���x
////--------------------------------------------------------------
//
//// �{�[���e���x�\����
//struct boneInfluence
//{
//	uint32_t boneIndex;	// �{�[���̔ԍ�
//	float boneWeight;	// ���̃{�[���ɑ΂���Ǐ]�x
//};
//using boneInfluencesPerControlPoint = std::vector<boneInfluence>;
//
//// boneInfluence�̎擾
//void FetchBoneInfluences(const FbxMesh* fbxMesh, std::vector<boneInfluencesPerControlPoint>& boneInfluences)
//{
//	// --- �{�[���e���x�\���̂̃T�C�Y��mesh�̎����_�̐��ɕύX ---
//	const int controlPointsCount = fbxMesh->GetControlPointsCount();
//	boneInfluences.resize(controlPointsCount);
//
//	// ���ׂĂ� deformer (FbxSkin) ������ 
//	const int skinCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
//	for (int skinIndex = 0; skinIndex < skinCount; ++skinIndex)
//	{
//		// �������� deformer �̎擾	
//		const FbxSkin* fbxSkin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(skinIndex, FbxDeformer::eSkin));
//
//		// ���� deformer �����S�Ă� cluster ���������� --- (���ׂẴ{�[���̏���)
//		const int clusterCount = fbxSkin->GetClusterCount();
//		for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)
//		{
//			// ���݂� index �� cluster ���擾 --- (��������{�[���̎擾)
//			const FbxCluster* fbxCluster = fbxSkin->GetCluster(clusterIndex);
//
//			// cluster ���e����^���钸�_�̐����擾(1�� cluster ���R�̒��_�ɉe����^����ꍇ�R��Ԃ�) --- (�{�[�����e����^����S�Ă̒��_������)
//			const int controlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();
//
//			for (int controlPointIndicesIndex = 0; controlPointIndicesIndex < controlPointIndicesCount; ++controlPointIndicesIndex)
//			{
//				// index �� weight �� push ����
//				int controlPointIndex = fbxCluster->GetControlPointIndices()[controlPointIndicesIndex];
//				double controlPointWeight = fbxCluster->GetControlPointWeights()[controlPointIndicesIndex];
//				boneInfluence& boneInfluence = boneInfluences.at(controlPointIndex).emplace_back();
//				boneInfluence.boneIndex = static_cast<uint32_t>(clusterIndex);
//				boneInfluence.boneWeight = static_cast<float>(controlPointWeight);
//			}
//		}
//	}
//}
//
////--------------------------------------------------------------
//// Fbx �̍s�񂩂� DirectX �̍s��ɕϊ�����֐�
////--------------------------------------------------------------
//inline XMFLOAT4X4 toXmfloat4x4(const FbxAMatrix& fbxamatrix)
//{
//	XMFLOAT4X4 xmfloat4x4;
//	for (int row = 0; row < 4; ++row)
//	{
//		for (int column = 0; column < 4; ++column)
//		{
//			xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
//		}
//	}
//	return xmfloat4x4;
//}
//inline XMFLOAT3 toXmfloat3(const FbxDouble3& fbxdouble3)
//{
//	XMFLOAT3 xmfloat3;
//	xmfloat3.x = static_cast<float>(fbxdouble3[0]);
//	xmfloat3.y = static_cast<float>(fbxdouble3[1]);
//	xmfloat3.z = static_cast<float>(fbxdouble3[2]);
//	return xmfloat3;
//}
//inline XMFLOAT4 toXmfloat4(const FbxDouble4& fbxdouble4)
//{
//	XMFLOAT4 xmfloat4;
//	xmfloat4.x = static_cast<float>(fbxdouble4[0]);
//	xmfloat4.y = static_cast<float>(fbxdouble4[1]);
//	xmfloat4.z = static_cast<float>(fbxdouble4[2]);
//	xmfloat4.w = static_cast<float>(fbxdouble4[3]);
//	return xmfloat4;
//}
//
//
////--------------------------------------------------------------
//// SkinnedMesh
////--------------------------------------------------------------
//SkinnedMesh::SkinnedMesh(const char* fbxFilename, bool triangulate, float samplingRate)
//{
//	// fbxPath �̕ۑ�
//	fbxPath = fbxFilename;
//
//	// parentPath �̎擾
//	std::filesystem::path fbxFilePath(fbxFilename);
//	parentPath = fbxFilePath.parent_path().string();
//
//	// model Path�쐬
//	std::filesystem::path path(fbxPath);
//	std::string modelFilePath = parentPath + "/" + path.stem().string() + ".model";
//
//
//	if (std::filesystem::exists(modelFilePath.c_str()))
//	{
//		LoadModel();
//	}
//	else
//	{
//		LoadFbx(fbxFilename, triangulate, samplingRate);
//	}
//
//	
//	CreateComObjects(fbxFilename);
//
//	
//	std::string message = "create " + fbxFilePath.stem().string() + " finish";
//	ConsoleData::Instance().logs.push_back(message);
//}
//
//
//// ModelLoad����
//void SkinnedMesh::LoadModel()
//{
//	// --- �g���q�p�ϐ��쐬 ---
//	std::string pathExtension = "";
//
//	// --- sceneView �̃��[�h ---
//	std::filesystem::path path(fbxPath);
//	std::string modelFilePath = parentPath + "/" + path.stem().string() + ".model";
//	
//#if 0
//	std::ifstream ifs(modelFilePath.c_str(), std::ios::binary);
//	cereal::BinaryInputArchive deserialization(ifs);
//#else
//	std::ifstream ifs(modelFilePath.c_str(), std::ios::in);
//	cereal::JSONInputArchive deserialization(ifs);
//#endif
//	deserialization(sceneView, coordinateSystemIndex, fbxUnit, skeletonSpheres);
//
//	// --- ���b�V���쐬 ---
//	std::string meshPath = parentPath + "/Mesh/";
//	pathExtension = ".mesh";
//
//	// ".mesh" �̊g���q�̃t�@�C����S�ď���
//	for (const auto& entry : std::filesystem::directory_iterator(meshPath)) {
//		if (entry.path().extension() == pathExtension) {
//
//#if 1
//			std::ifstream meshIfs(entry.path().c_str(), std::ios::binary);
//			cereal::BinaryInputArchive   meshDeserialization(meshIfs);
//#else
//			std::ifstream meshIfs(entry.path().c_str(), std::ios::in);
//			cereal::JSONInputArchive   meshDeserialization(meshIfs);
//#endif
//			Mesh mesh;
//			meshDeserialization(mesh);
//			meshes.push_back(mesh);
//		}
//	}
//
//	// --- �}�e���A���쐬 ---
//	std::string materialPath = parentPath + "/Material/";
//	pathExtension = ".material";
//
//	// ".material" �̊g���q�̃t�@�C����S�ď���
//	for (const auto& entry : std::filesystem::directory_iterator(materialPath)) {
//		if (entry.path().extension() == pathExtension) {
//
//			std::ifstream materialIfs(entry.path().c_str(), std::ios::in);
//			cereal::JSONInputArchive   meshDeserialization(materialIfs);
//			Material material;
//			meshDeserialization(material);
//			materials.emplace(material.uniqueId, std::move(material));
//		}
//	}
//
//	// --- �A�j���[�V�����N���b�v�쐬 ---
//	std::string animPath = parentPath + "/Anim/";
//	pathExtension = ".anim";
//
//	// ".anim" �̊g���q�̃t�@�C����S�ď���
//	for (const auto& entry : std::filesystem::directory_iterator(animPath)) {
//		if (entry.path().extension() == pathExtension) {
//
//#if 1
//			std::ifstream animIfs(entry.path().c_str(), std::ios::binary);
//			cereal::BinaryInputArchive   animDeserialization(animIfs);
//#else
//			std::ifstream animIfs(entry.path().c_str(), std::ios::in);
//			cereal::JSONInputArchive   animDeserialization(animIfs);
//#endif
//			Animation anim;
//
//			animDeserialization(anim);
//
//			// �A�j���[�V�����̖��O���t�@�C�����Ɠ����ɂ���
//			//anim.name = entry.path().stem().string();
//
//			animationClips.push_back(anim);
//		}
//	}
//}
//
//// FbxLoad����
//void SkinnedMesh::LoadFbx(const char* fbxFilename, bool triangulate, float samplingRate)
//{
//	// FbxManager �̍쐬
//	FbxManager* fbxManager = FbxManager::Create();
//	// FbxScene �̍쐬
//	FbxScene* fbxScene = FbxScene::Create(fbxManager, "");
//	// FbxImporter �̍쐬
//	FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
//
//	// �t�@�C���̏�����
//	bool importStatus = false;
//	importStatus = fbxImporter->Initialize(fbxFilename);
//	_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());
//
//	// �t�@�C���̃C���|�[�g
//	importStatus = fbxImporter->Import(fbxScene);
//	_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());
//
//	// 3D���f���̕s�v�ȃ|���S���̏���
//	FbxGeometryConverter fbxConverter(fbxManager);
//	if (triangulate)
//	{
//		// �|���S�����R�p�`�ɕϊ�����(�d������)
//		fbxConverter.Triangulate(fbxScene, true/*replace*/, false/*legacy*/);
//		fbxConverter.RemoveBadPolygonsFromMeshes(fbxScene);
//	}
//
//	std::function<void(FbxNode*)> traverse
//	{
//		[&](FbxNode* fbxNode)
//		{
//			// node �� emplace_back()
//			SkinnedScene::Node& node{sceneView.nodes.emplace_back() };
//			// node �̑����擾
//			node.attribute = fbxNode->GetNodeAttribute() ? fbxNode->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eBoundary;
//			// node �̖��O�擾
//			node.name = fbxNode->GetName();
//			// node �� id �擾
//			node.uniqueId = fbxNode->GetUniqueID();
//			// �e node �̃C���f�b�N�X�擾
//			node.parentIndex = sceneView.indexof(fbxNode->GetParent() ? fbxNode->GetParent()->GetUniqueID() : 0);
//			// �q node �� traverse()
//			for (int childIndex = 0; childIndex < fbxNode->GetChildCount(); ++childIndex)
//			{
//				traverse(fbxNode->GetChild(childIndex));
//			}
//		}
//	};
//	traverse(fbxScene->GetRootNode());
//
//#if 1
//	ConsoleData::Instance().logs.push_back("Load Fbx : " + static_cast<std::string>(fbxFilename));
//	for (const SkinnedScene::Node& node : sceneView.nodes)
//	{
//		FbxNode* fbxNode{ fbxScene->FindNodeByName(node.name.c_str()) };
//
//		std::string nodeName = fbxNode->GetName();
//		uint64_t uid = fbxNode->GetUniqueID();
//		uint64_t parentUid = fbxNode->GetParent() ? fbxNode->GetParent()->GetUniqueID() : 0;
//		int32_t type = fbxNode->GetNodeAttribute() ? fbxNode->GetNodeAttribute()->GetAttributeType() : 0;		std::stringstream debugString;
//		debugString << " : nodeName -" << nodeName << ": uId -" << uid << " : parentUid - " << parentUid << " : type - " << type << "\n";
//		OutputDebugStringA(debugString.str().c_str());
//		ConsoleData::Instance().logs.push_back(debugString.str());
//	}
//#endif
//
//	// mesh �̎擾 ( ���ňꏏ�Ƀ{�[���̏����擾���Ă���)
//	FetchMeshes(fbxFilename, fbxScene, meshes);
//
//	// material �̎擾
//	FetchMaterials(fbxFilename, fbxScene, materials);
//
//	// �A�j���[�V�������̎擾
//	FetchAnimations(fbxFilename, fbxScene, animationClips, samplingRate);
//
//	// �j��
//	fbxImporter->Destroy();
//	fbxScene->Destroy();
//	fbxManager->Destroy();
//
//
//	// model�̏o�̓t�@�C���쐬
//	std::filesystem::path path(fbxPath);
//	std::string modelFilePath = parentPath + "/" + path.stem().string() + ".model";
//
//	// �o��
//#if 0
//	std::ofstream ofs(modelFilePath.c_str(), std::ios::binary);
//	cereal::BinaryOutputArchive serialization(ofs);
//#else
//	std::ofstream ofs(modelFilePath.c_str(), std::ios::out);
//	cereal::JSONOutputArchive  serialization(ofs);
//#endif
//	serialization(sceneView, coordinateSystemIndex, fbxUnit, skeletonSpheres);
//}
//
//// �`��
//void SkinnedMesh::Render(const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& materialColor, const Animation::KeyFrame* keyFrame)
//{
//	// --- Graphics �擾 ---
//	Graphics* gfx = &Graphics::Instance();
//	ID3D11DeviceContext* dc = gfx->GetDeviceContext();
//
//	// --- ���b�V�����Ƃ̕`�� ---
//	for (const Mesh& mesh : meshes)
//	{
//		uint32_t stride = sizeof(Vertex);
//		uint32_t offset = 0;
//
//		dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
//		dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
//		dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//		dc->IASetInputLayout(inputLayout.Get());
//
//		
//		//dc->VSSetShader(vertexShader.Get(), nullptr, 0);
//		//dc->PSSetShader(pixelShader.Get(), nullptr, 0);
//		
//		
//		Constants data;
//
//		// �A�j���[�V�����̃L�[�t���[��������Ȃ炻�̎p����K�p����
//		if (keyFrame && keyFrame->nodes.size() > 0)
//		{
//			const Animation::KeyFrame::Node& meshNode = keyFrame->nodes.at(mesh.nodeIndex);
//			XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&meshNode.globalTransform) * XMLoadFloat4x4(&world));
//
//			const size_t boneCount = mesh.bindPose.bones.size();
//			_ASSERT_EXPR(boneCount < MAX_BONES, L"The value of the 'boneCount' has exceeded MAX_BONES.");
//
//			for (size_t boneIndex = 0; boneIndex < boneCount; boneIndex++)
//			{
//				const Skeleton::Bone& bone = mesh.bindPose.bones.at(boneIndex);
//				const Animation::KeyFrame::Node& boneNode = keyFrame->nodes.at(bone.nodeIndex);
//				XMStoreFloat4x4(
//					&data.boneTransforms[boneIndex],
//					XMLoadFloat4x4(&bone.offsetTransform) *
//					XMLoadFloat4x4(&boneNode.globalTransform) *
//					XMMatrixInverse(nullptr, XMLoadFloat4x4(&mesh.defaultGlobalTransform))
//				);
//			}
//		}
//		else
//		{
//			XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh.defaultGlobalTransform) * XMLoadFloat4x4(&world));
//			for (size_t boneIndex = 0; boneIndex < MAX_BONES; boneIndex++)
//			{
//				data.boneTransforms[boneIndex] =
//				{
//					1,0,0,0,
//					0,1,0,0,
//					0,0,1,0,
//					0,0,0,1
//				};
//			}
//		}
//		for (const Mesh::Subset& subset : mesh.subsets)
//		{
//			const Material& material = materials.at(subset.materialUniqueId);
//			XMStoreFloat4(&data.materialColor, XMLoadFloat4(&materialColor) * XMLoadFloat4(&material.Kd));
//
//			dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
//			dc->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
//
//			// isBloom
//			dc->UpdateSubresource(isBloomConstantBuffer.Get(), 0, 0, &isBloomConstants, 0, 0);
//			dc->PSSetConstantBuffers(2, 1, isBloomConstantBuffer.GetAddressOf());
//
//			dc->PSSetShaderResources(0, 1, material.shaderResourceViews[0].GetAddressOf());
//			dc->PSSetShaderResources(1, 1, material.shaderResourceViews[1].GetAddressOf());
//			dc->PSSetShaderResources(2, 1, material.shaderResourceViews[2].GetAddressOf());
//			dc->PSSetShaderResources(3, 1, material.shaderResourceViews[3].GetAddressOf());
//
//			dc->DrawIndexed(subset.indexCount, subset.startIndexLocation, 0);
//		}
//	}
//}
//
//// mesh �擾
//void SkinnedMesh::FetchMeshes(const char* fbxFilename, FbxScene* fbxScene, std::vector<Mesh>& meshes)
//{
//	// ------- cereal���� -------
//
//	// Mesh �t�H���_�̍쐬�ƃp�X�쐬
//	std::filesystem::path filename(fbxFilename);	// fbx �Ɠ��� path �ɕۑ����邽�߂� fbx�� path �ŏ�����
//	std::string parentPath = filename.parent_path().string();	// path�擾
//	parentPath += "/Mesh";
//	std::filesystem::create_directory(parentPath);
//
//	// ���ׂĂ� node�\���� ���`�F�b�N
//	for (const SkinnedScene::Node& node : sceneView.nodes)
//	{
//		// node�\���� �̃^�C�v�� mesh �ȊO�Ȃ� continue
//		if (node.attribute != FbxNodeAttribute::EType::eMesh)
//		{
//			continue;
//		}
//		// fbxNode �̎擾
//		FbxNode* fbxNode = fbxScene->FindNodeByName(node.name.c_str());
//		// fbxMesh �̎擾
//		FbxMesh* fbxMesh = fbxNode->GetMesh();
//
//		// mesh�\���̂��쐬�� meshes �� push
//		Mesh& mesh = meshes.emplace_back();
//		// �� nodes.uniqueId ������Ă���̓����H
//		mesh.uniqueId = fbxMesh->GetNode()->GetUniqueID();	// mesh �̐e node �� id �擾
//		mesh.name = fbxMesh->GetNode()->GetName();			// mesh �̐e node �� ���O�擾
//		mesh.nodeIndex = sceneView.indexof(mesh.uniqueId);	// ���̃m�[�h�� sceneView �� nodes �x�N�^�[�̂ǂ��ɂ��邩������
//		mesh.defaultGlobalTransform = toXmfloat4x4(fbxMesh->GetNode()->EvaluateGlobalTransform());	// ���f����Ԃ̌��_����݂��p��
//
//		// bone ����
//		std::vector<boneInfluencesPerControlPoint> boneInfluences;	// �{�[���̏��������Ă���
//		FetchBoneInfluences(fbxMesh, boneInfluences);	// �{�[���̏��(index �� weight)��boneInfluences�ɃZ�b�g
//		FetchSkeleton(fbxMesh, mesh.bindPose);
//
//		// mesh �̎��� mesh.subsets �� subsets �Ƃ����ϐ��ɂ���
//		std::vector<Mesh::Subset>& subsets = mesh.subsets;
//		// material �̐����擾
//		const int materialCount = fbxMesh->GetNode()->GetMaterialCount();
//		// subsets �̃T�C�Y���}�e���A���̐��ɂ���
//		subsets.resize(materialCount > 0 ? materialCount : 1);
//
//		for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex)
//		{
//			// index �� material ���擾
//			const FbxSurfaceMaterial* fbxMaterial = fbxMesh->GetNode()->GetMaterial(materialIndex);
//			// subsets �� ���O�� id ���Z�b�g
//			subsets.at(materialIndex).materialName = fbxMaterial->GetName();
//			subsets.at(materialIndex).materialUniqueId = fbxMaterial->GetUniqueID();
//		}
//		if (materialCount > 0)
//		{
//			// mesh �̃|���S���̐����擾
//			const int polygonCount = fbxMesh->GetPolygonCount();
//			for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
//			{
//				// �|���S���C���f�b�N�X�ɑΉ����� material �� index ���擾
//				// �|���S����index��n����matrial��index��Ԃ��Ă����
//				// �|���S��x3����indexCount�����Z����
//				const int materialIndex = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex);
//				// �Ή����� materialIndex �� indexCount �𒸓_�̐�(3)���Z����
//				subsets.at(materialIndex).indexCount += 3;
//			}
//			// subsets ���� startIndexLocation �̒���
//			uint32_t offset{ 0 };
//			for (Mesh::Subset& subset : subsets)
//			{
//				subset.startIndexLocation = offset;
//				offset += subset.indexCount;
//				subset.indexCount = 0;
//			}
//		}
//		else
//		{
//			// �_�~�[�}�e���A������
//			MakeDummyMaterial();
//		}
//
//		// �|���S�������擾
//		const int polygonCount = fbxMesh->GetPolygonCount();
//		mesh.vertices.resize(polygonCount * 3LL);	// vertices �̃T�C�Y���|���S���̂R�{�Ƀ��T�C�Y
//		mesh.indices.resize(polygonCount * 3LL);	// indices �̃T�C�Y���|���S���̂R�{�Ƀ��T�C�Y
//
//		// UVSet �̖��O���擾
//		FbxStringList uvNames;
//		fbxMesh->GetUVSetNames(uvNames);
//
//		// �R���g���[���|�C���g(���_���W)���擾
//		const FbxVector4* controlPoints = fbxMesh->GetControlPoints();
//
//		// �|���S���̐������J��Ԃ�
//		for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
//		{
//			// �|���S���C���f�b�N�X�ɑΉ����� material �� index ���擾
//			const int materialIndex = materialCount > 0 ? fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex) : 0;
//			// �Ή����� materialIndex �� subset ���擾
//			Mesh::Subset& subset = subsets.at(materialIndex);
//			const uint32_t offset = subset.startIndexLocation + subset.indexCount;
//
//			// ���_�̐�(3)�����J��Ԃ�
//			for (int positionInPolygon = 0; positionInPolygon < 3; ++positionInPolygon)
//			{
//				// ���݂̒��_�� index ���v�Z
//				const int vertexIndex = polygonIndex * 3 + positionInPolygon;
//
//				// ���_���(vertex)�̍쐬
//				Vertex vertex;
//				const int polygonVertex = fbxMesh->GetPolygonVertex(polygonIndex, positionInPolygon);	// polygonIndex �̃|���S���� positionInPolygon �̒��_�� index ���擾
//				vertex.position.x = static_cast<float>(controlPoints[polygonVertex][0]);	// index �ɑΉ��������_���W���Z�b�g
//				vertex.position.y = static_cast<float>(controlPoints[polygonVertex][1]);
//				vertex.position.z = static_cast<float>(controlPoints[polygonVertex][2]);
//
//				// ���_�ɑΉ����� �{�[���̃C���f�b�N�X�ƃE�F�C�g���Z�b�g����
//				const boneInfluencesPerControlPoint& influencesPerControlPoint = boneInfluences.at(polygonVertex);
//				for (size_t influenceIndex = 0; influenceIndex < influencesPerControlPoint.size(); ++influenceIndex)
//				{
//					if (influenceIndex < MAX_BONE_INFLUENCES)
//					{
//						// vertex �� weight �� index �� influencesPerControlPoint �̒l���Z�b�g����
//						vertex.boneWeights[influenceIndex] = influencesPerControlPoint.at(influenceIndex).boneWeight;
//						vertex.boneIndices[influenceIndex] = influencesPerControlPoint.at(influenceIndex).boneIndex;
//					}
//				}
//
//				// mesh �ɖ@����񂪂���Ȃ� vertex �ɃZ�b�g����
//				if (fbxMesh->GetElementNormalCount() > 0)
//				{
//					FbxVector4 normal;
//					fbxMesh->GetPolygonVertexNormal(polygonIndex, positionInPolygon, normal);
//					vertex.normal.x = static_cast<float>(normal[0]);
//					vertex.normal.y = static_cast<float>(normal[1]);
//					vertex.normal.z = static_cast<float>(normal[2]);
//				}
//				// mesh ��UV(�e�N�X�`�����W)��񂪂���Ȃ� vertex �ɃZ�b�g����
//				if (fbxMesh->GetElementUVCount() > 0)
//				{
//					FbxVector2 uv;
//					bool unmappedUv;
//					fbxMesh->GetPolygonVertexUV(polygonIndex, positionInPolygon, uvNames[0], uv, unmappedUv);
//					vertex.texcoord.x = static_cast<float>(uv[0]);
//					vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
//				}
//				// �@���x�N�g���̒l�̃Z�b�g(�@���x�N�g���������Ȃ��ꍇ�� GenerateTangentsData �Ő���)
//				if (fbxMesh->GenerateTangentsData(0, false))
//				{
//					const FbxGeometryElementTangent* tangent = fbxMesh->GetElementTangent(0);
//					vertex.tangent.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[0]);
//					vertex.tangent.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[1]);
//					vertex.tangent.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[2]);
//					vertex.tangent.w = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[3]);
//				}
//
//				// mesh �� ���_���x�N�^�[(vertices)�ɍ쐬�������_���(vertex)���Z�b�g����
//				mesh.vertices.at(vertexIndex) = std::move(vertex);
//				// mesh �� �C���f�b�N�X���x�N�^�[(indices)�Ɍ��݂� index ���Z�b�g����
//				mesh.indices.at(static_cast<size_t>(offset) + positionInPolygon) = vertexIndex;
//				subset.indexCount++;
//			}
//		}
//
//		// �o�E���f�B���O�{�b�N�X�̒l�ݒ�
//		for (const Vertex& v : mesh.vertices)
//		{
//			mesh.boundingBox[0].x = std::min<float>(mesh.boundingBox[0].x, v.position.x);
//			mesh.boundingBox[0].y = std::min<float>(mesh.boundingBox[0].y, v.position.y);
//			mesh.boundingBox[0].z = std::min<float>(mesh.boundingBox[0].z, v.position.z);
//			mesh.boundingBox[1].x = std::min<float>(mesh.boundingBox[1].x, v.position.x);
//			mesh.boundingBox[1].y = std::min<float>(mesh.boundingBox[1].y, v.position.y);
//			mesh.boundingBox[1].z = std::min<float>(mesh.boundingBox[1].z, v.position.z);
//		}
//
//		// ------- cereal���� -------
//
//		// mesh�̏o�̓t�@�C���쐬
//		std::string meshFilename = parentPath + "/" + mesh.name + ".mesh";
//
//		// �o��
//#if 1
//		std::ofstream ofs(meshFilename.c_str(), std::ios::binary);
//		cereal::BinaryOutputArchive  serialization(ofs);
//#else
//		std::ofstream ofs(meshFilename.c_str(), std::ios::out);
//		cereal::JSONOutputArchive  serialization(ofs);
//#endif
//		serialization(mesh);
//	}
//}
//
//// material �擾
//void SkinnedMesh::FetchMaterials(const char* fbxFilename, FbxScene* fbxScene, std::unordered_map<uint64_t, Material>& materials)
//{
//	// ------- cereal���� -------
//
//	// Material �t�H���_�̍쐬�ƃp�X�쐬
//	std::filesystem::path filename(fbxFilename);	// fbx �Ɠ��� path �ɕۑ����邽�߂� fbx�� path �ŏ�����
//	std::string parentPath = filename.parent_path().string();	// path�擾
//	parentPath += "/Material";
//	std::filesystem::create_directory(parentPath);
//
//	// node�\���� �̐����擾
//	const size_t nodeCount = sceneView.nodes.size();
//	// ���ׂĂ� node�\���� ���`�F�b�N
//	for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
//	{
//		// node �� nodeIndex �� index �� node �ɃA�N�Z�X
//		const SkinnedScene::Node& node = sceneView.nodes.at(nodeIndex);
//		// �擾���� node �Ɠ��� FbxNode ���擾
//		const FbxNode* fbxNode =fbxScene->FindNodeByName(node.name.c_str());
//
//		// node ������ material �̐����擾
//		const int materialCount = fbxNode->GetMaterialCount();
//		for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex)
//		{
//			const FbxSurfaceMaterial* fbxMaterial = fbxNode->GetMaterial(materialIndex);
//
//			// material �̍쐬
//			Material material;
//			material.name = fbxMaterial->GetName();			// ���O�̎擾
//			material.uniqueId = fbxMaterial->GetUniqueID();	// id �̎擾
//
//			// Diffse�v���p�e�B���擾
//			FbxProperty fbxProperty;
//			fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
//			if (fbxProperty.IsValid())
//			{
//				const FbxDouble3 color = fbxProperty.Get<FbxDouble3>();
//				material.Kd.x = static_cast<float>(color[0]);
//				material.Kd.y = static_cast<float>(color[1]);
//				material.Kd.z = static_cast<float>(color[2]);
//				material.Kd.w = 1.0f;
//
//				// fbxProperty �Ɋ֘A�Â���ꂽ�e�N�X�`���̎擾
//				const FbxFileTexture* fbxTexture = fbxProperty.GetSrcObject<FbxFileTexture>();
//				material.textureFilenames[0] = fbxTexture ? fbxTexture->GetRelativeFileName() : "";
//			}
//			// Ambient�v���p�e�B���擾
//			fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sAmbient);
//			if (fbxProperty.IsValid())
//			{
//				const FbxDouble3 color = fbxProperty.Get<FbxDouble3>();
//				material.Ka.x = static_cast<float>(color[0]);
//				material.Ka.y = static_cast<float>(color[1]);
//				material.Ka.z = static_cast<float>(color[2]);
//				material.Ka.w = 1.0f;
//
//				// fbxProperty �Ɋ֘A�Â���ꂽ�e�N�X�`���̎擾
//				const FbxFileTexture* fbxTexture = fbxProperty.GetSrcObject<FbxFileTexture>();
//				material.textureFilenames[3] = fbxTexture ? fbxTexture->GetRelativeFileName() : "";
//			}
//			// Specular�v���p�e�B���擾
//			fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
//			if (fbxProperty.IsValid())
//			{
//				const FbxDouble3 color = fbxProperty.Get<FbxDouble3>();
//				material.Ks.x = static_cast<float>(color[0]);
//				material.Ks.y = static_cast<float>(color[1]);
//				material.Ks.z = static_cast<float>(color[2]);
//				material.Ks.w = 1.0f;
//
//				// fbxProperty �Ɋ֘A�Â���ꂽ�e�N�X�`���̎擾
//				const FbxFileTexture* fbxTexture = fbxProperty.GetSrcObject<FbxFileTexture>();
//				material.textureFilenames[2] = fbxTexture ? fbxTexture->GetRelativeFileName() : "";
//			}
//
//			// ------- cereal���� -------
//
//			// material�̏o�̓t�@�C���쐬
//			std::string materialFilename = parentPath + "/" + material.name + ".material";
//
//			// �o��
//			std::ofstream ofs(materialFilename.c_str(), std::ios::out);
//			cereal::JSONOutputArchive  serialization(ofs);
//			serialization(material);
//
//
//			// push
//			materials.emplace(material.uniqueId, std::move(material));
//		}
//	}
//}
//
//// �o�C���h�|�[�Y�̏����擾
//void SkinnedMesh::FetchSkeleton(FbxMesh* fbxMesh, Skeleton& bindPose)
//{
//	// ���ׂĂ� deformer (FbxSkin) ������ 
//	const int deformerCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
//	for (int deformerIndex = 0; deformerIndex < deformerCount; ++deformerIndex)
//	{
//		// �������� deformer �̎擾	
//		FbxSkin* skin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
//
//		// ���� deformer �����S�Ă� cluster ���������� --- (���ׂẴ{�[���̏���)
//		const int clusterCount = skin->GetClusterCount();
//
//		// �������Ă��郁�b�V��������Skeleton��Bone�̑傫�����{�[���̐��ɂ���
//		bindPose.bones.resize(clusterCount);
//		
//		for (int clusterIndex = 0; clusterIndex < clusterCount; clusterIndex++)
//		{
//			// ���݂� index �� cluster ���擾 --- (��������{�[���̎擾)
//			FbxCluster* cluster = skin->GetCluster(clusterIndex);
//
//			// �{�[���̏����擾���ăZ�b�g
//			Skeleton::Bone& bone = bindPose.bones.at(clusterIndex);
//			bone.name = cluster->GetLink()->GetName();
//			bone.uniqueId = cluster->GetLink()->GetUniqueID();
//			bone.parentIndex = bindPose.indexof(cluster->GetLink()->GetParent()->GetUniqueID());
//			bone.nodeIndex = sceneView.indexof(bone.uniqueId);
//
//			// ���b�V���̏����O���[�o���s����擾
//			FbxAMatrix referenceGlobalInitPosition;
//			cluster->GetTransformMatrix(referenceGlobalInitPosition);
//
//			// �{�[���̏����O���[�o���s����擾
//			FbxAMatrix clusterGlobalInitPosition;
//			cluster->GetTransformLinkMatrix(clusterGlobalInitPosition);
//
//			// ���b�V����Ԃ̒��_���W���O���[�o����Ԃ̒��_���W�ɂ���s���
//			// �O���[�o����Ԃ̒��_���W���{�[����Ԃ̒��_���W�ɂ���s����|���Z���邱�Ƃ�
//			// ���b�V����Ԃ���{�[����Ԃ֕ϊ�����I�t�Z�b�g�s����쐬����
//			bone.offsetTransform = toXmfloat4x4(clusterGlobalInitPosition.Inverse() * referenceGlobalInitPosition);
//
//			// �X�L�j���O��̒��_���W = ���b�V����Ԃ̒��_���W * �I�t�Z�b�g�s��
//		}
//	}
//}
//
//// �A�j���[�V�������̎擾
//void SkinnedMesh::FetchAnimations(const char* fbxFilename, FbxScene* fbxScene, std::vector<Animation>& animationClips, float samplingRate)
//{
//	// ------- cereal���� -------
//
//	// Anim �t�H���_�̍쐬�ƃp�X�쐬
//	std::filesystem::path filename(fbxFilename);	// fbx �Ɠ��� path �ɕۑ����邽�߂� fbx�� path �ŏ�����
//	std::string parentPath = filename.parent_path().string();	// path�擾
//	parentPath += "/Anim";
//	std::filesystem::create_directory(parentPath);
//
//	// �S�ẴA�j���[�V�����X�^�b�N�̖��O�̎擾
//	FbxArray<FbxString*> animationStackNames;
//	fbxScene->FillAnimStackNameArray(animationStackNames);
//
//	// �S�ẴA�j���[�V�����X�^�b�N����������
//	const int animationStackCount = animationStackNames.GetCount();
//
//	for (int animationStackIndex = 0; animationStackIndex < animationStackCount; animationStackIndex++)
//	{
//		// --- ��������e�A�j���[�V�����X�^�b�N�ɑ΂��鏈�� ---
//		bool isContinue = false;
//		for (int i = 0; i < animationClips.size(); i++)
//		{
//			std::string animationStackName = animationStackNames[animationStackIndex]->Buffer();
//			std::string animationName = animationClips.at(i).name;
//			if (animationStackName == animationName) isContinue = true;
//		}
//		if (isContinue) continue;
//
//		// animeClip�̖��O�� �A�j���[�V�����X�^�b�N���������
//		Animation& animationClip = animationClips.emplace_back();
//		animationClip.name = animationStackNames[animationStackIndex]->Buffer();
//
//		// index �̃A�j���[�V�����X�^�b�N���擾
//		FbxAnimStack* animationStack = fbxScene->FindMember<FbxAnimStack>(animationClip.name.c_str());
//		fbxScene->SetCurrentAnimationStack(animationStack);
//
//		// --- �T���v�����O���[�g�ƃT���v�����O�Ԋu�̐ݒ� ---
//
//		// �^�C�����[�h�̎擾�i���Ԃ̕\�����@�̎w��j
//		const FbxTime::EMode timeMode = fbxScene->GetGlobalSettings().GetTimeMode();
//		// �^�C�����[�h�ɑΉ�����P�b�̕\�������邽�߂̕ϐ���p��
//		FbxTime oneSecond;
//		oneSecond.SetTime(0, 0, 1, 0, 0, timeMode);
//		// �T���v�����O���[�g�̎w��
//		animationClip.samplingRate = samplingRate > 0 ? samplingRate : static_cast<float>(oneSecond.GetFrameRate(timeMode));
//		// �T���v�����O�Ԋu�̌v�Z(���̕b�����ƂɃT���v�����O���s��)
//		const FbxTime samplingInterval = static_cast<FbxLongLong>(oneSecond.Get() / animationClip.samplingRate);
//		// �A�j���[�V�����̊J�n���ԂƏI�����Ԃ̎擾
//		const FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(animationClip.name.c_str());
//		const FbxTime startTime = takeInfo->mLocalTimeSpan.GetStart();
//		const FbxTime stopTime = takeInfo->mLocalTimeSpan.GetStop();
//
//		// �A�j���[�V�����I�����Ԃ̕ۑ�
//		double doubleSeconds = fbxScene->GetTakeInfo(animationClip.name.c_str())->mLocalTimeSpan.GetStop().GetSecondDouble();
//		animationClip.secondsLength = static_cast<float>(doubleSeconds);
//		
//		// --- �e�L�[�t���[���ɑ΂��鏈�� ---
//		// �A�j���[�V�����̊J�n���Ԃ���I�����Ԃ܂ŁA�T���v�����O�Ԋu���ƂɃL�[�t���[���𐶐����Ă���
//		for (FbxTime time = startTime; time < stopTime; time += samplingInterval)
//		{
//			// �L�[�t���[���𐶐����ăV�[�N�G���X��emplace_back���Ă���
//			Animation::KeyFrame& keyFrame = animationClip.sequence.emplace_back();
//				
//			// �L�[�t���[���� node �̐��� ���̃��f��������node�̐��ɂ���
//			const size_t nodeCount = sceneView.nodes.size();
//			keyFrame.nodes.resize(nodeCount);
//			
//			// ���ׂĂ� node �̏���
//			for (size_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
//			{
//				FbxNode* fbxNode = fbxScene->FindNodeByName(sceneView.nodes.at(nodeIndex).name.c_str());
//				if (fbxNode)
//				{
//					Animation::KeyFrame::Node& node = keyFrame.nodes.at(nodeIndex);
//
//					//	uniqueId �Ɩ��O�̃Z�b�g
//					node.uniqueId = fbxNode->GetUniqueID();
//					node.name = fbxNode->GetName();
//
//					// node �� �O���[�o���ϊ��s��ɂ����
//					node.globalTransform = toXmfloat4x4(fbxNode->EvaluateGlobalTransform(time));
//
//					// node ���[�J�� �� scaling rotation translation �ɂ��ꂼ��ϊ��s��������
//					const FbxAMatrix& localTransform = fbxNode->EvaluateLocalTransform(time);
//					node.scaling = toXmfloat3(localTransform.GetS());
//					node.rotation = toXmfloat4(localTransform.GetQ());
//					node.translation = toXmfloat3(localTransform.GetT());
//				}
//			}
//		}
//
//		// ------- cereal���� -------
//
//		// anim�̏o�̓t�@�C���쐬
//		std::string animationFilename = parentPath + "/" + animationClip.name + ".anim";
//
//		// �o��
//#if 1
//		std::ofstream ofs(animationFilename.c_str(), std::ios::binary);
//		cereal::BinaryOutputArchive  serialization(ofs);
//#else
//		std::ofstream ofs(animationFilename.c_str(), std::ios::out);
//		cereal::JSONOutputArchive  serialization(ofs);
//#endif
//		serialization(animationClip);
//	}
//	// �S�ẴA�j���[�V�����X�^�b�N���̉��
//	for (int animationStackIndex = 0; animationStackIndex < animationStackCount; animationStackIndex++)
//	{
//		delete animationStackNames[animationStackIndex];
//	}
//}
//
//
//// �A�j���[�V�����̍X�V(�A�j���[�V�����̂��� node �̕ϊ��s��̍X�V)
//void SkinnedMesh::UpdateAnimation(Animation::KeyFrame& keyFrame)
//{
//	// node �̐������J��Ԃ�
//	size_t nodeCount = keyFrame.nodes.size();
//	for (size_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
//	{
//		// ���݂� index �� node ���擾
//		Animation::KeyFrame::Node& node = keyFrame.nodes.at(nodeIndex);
//
//		// ���ꂼ��̒l�ɑΉ�����s��̍쐬
//		XMMATRIX S = XMMatrixScaling(node.scaling.x, node.scaling.y, node.scaling.z);
//		XMMATRIX R = XMMatrixRotationQuaternion(XMLoadFloat4(&node.rotation));
//		XMMATRIX T = XMMatrixTranslation(node.translation.x, node.translation.y, node.translation.z);
//
//		// �e�s�񂪂���ΐe�̍s���������
//		int64_t parentIndex = sceneView.nodes.at(nodeIndex).parentIndex;
//		XMMATRIX P = parentIndex < 0 ? XMMatrixIdentity() : XMLoadFloat4x4(&keyFrame.nodes.at(parentIndex).globalTransform);
//		
//		// globalTransform �ɒl��ݒ�
//		XMStoreFloat4x4(&node.globalTransform, S * R * T * P);
//
//		// �X�P���g���̃{�[�����X�V
//		int uniqueId = node.uniqueId;
//	}
//}
//
//// �A�j���[�V�����̒ǉ�
//bool SkinnedMesh::AppendAnimations(const char* animationFileName, float samplingRate)
//{
//	FbxManager* fbxManager = FbxManager::Create();
//	FbxScene* fbxScene = FbxScene::Create(fbxManager, "");
//
//	FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
//	bool importStatus = false;
//	importStatus = fbxImporter->Initialize(animationFileName);
//	_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());
//	importStatus = fbxImporter->Import(fbxScene);
//	_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());
//
//	FetchAnimations(animationFileName, fbxScene, animationClips, samplingRate);
//
//	fbxManager->Destroy();
//
//	return true;
//}
//
//// �A�j���[�V�����u�����h(2�̃L�[�t���[���̏����܂��Ă�C���[�W)
//// keyFrames �̂Q�̏����܂��� keyFrame �ɂ���Ă�
//void SkinnedMesh::BlendAnimations(const Animation::KeyFrame* keyFrames[2], float factor, Animation::KeyFrame& keyFrame)
//{
//	// �o�͑Ώۂ� keyFrame �� node ���������� keyFrame �� node �̃T�C�Y�ɂ���
//	size_t nodeCount = keyFrames[0]->nodes.size();
//	keyFrame.nodes.resize(nodeCount);
//	// �S�Ă� node ����������
//	for (size_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
//	{
//		// scale �̍���
//		XMVECTOR S[2] = 
//		{
//			XMLoadFloat3(&keyFrames[0]->nodes.at(nodeIndex).scaling),
//			XMLoadFloat3(&keyFrames[1]->nodes.at(nodeIndex).scaling)
//		};
//		XMStoreFloat3(&keyFrame.nodes.at(nodeIndex).scaling, XMVectorLerp(S[0], S[1], factor));
//
//		// rotation �̍���
//		XMVECTOR R[2] =
//		{
//			XMLoadFloat4(&keyFrames[0]->nodes.at(nodeIndex).rotation),
//			XMLoadFloat4(&keyFrames[1]->nodes.at(nodeIndex).rotation)
//		};
//		XMStoreFloat4(&keyFrame.nodes.at(nodeIndex).rotation, XMQuaternionSlerp(R[0], R[1], factor));
//
//		// translation �̍���
//		XMVECTOR T[2] =
//		{
//			XMLoadFloat3(&keyFrames[0]->nodes.at(nodeIndex).translation),
//			XMLoadFloat3(&keyFrames[1]->nodes.at(nodeIndex).translation)
//		};
//		XMStoreFloat3(&keyFrame.nodes.at(nodeIndex).translation, XMVectorLerp(T[0], T[1], factor));
//	}
//}
//
//
//
//void SkinnedMesh::CreateComObjects(const char* fbxFilename)
//{
//	// --- Graphics �擾 ----
//	Graphics* gfx = &Graphics::Instance();
//	ID3D11Device* device =  gfx->GetDevice();
//
//
//	//---  vertexBuffer �� indexBuffer �̍쐬 ---
//	for (Mesh& mesh : meshes)
//	{
//		HRESULT hr{ S_OK };
//		D3D11_BUFFER_DESC bufferDesc{};
//		D3D11_SUBRESOURCE_DATA subresourceData{};
//		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * mesh.vertices.size());
//		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
//		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//		bufferDesc.CPUAccessFlags = 0;
//		bufferDesc.MiscFlags = 0;
//		bufferDesc.StructureByteStride = 0;
//		subresourceData.pSysMem = mesh.vertices.data();
//		subresourceData.SysMemPitch = 0;
//		subresourceData.SysMemSlicePitch = 0;
//		hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.vertexBuffer.ReleaseAndGetAddressOf());
//		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
//
//		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh.indices.size());
//		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
//		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//		subresourceData.pSysMem = mesh.indices.data();
//		hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.indexBuffer.ReleaseAndGetAddressOf());
//		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
//
//#if 1
//		// �g�p���� ���_���ƃC���f�b�N�X�����N���A����
//		//mesh.vertices.clear();
//		//mesh.indices.clear();
//#endif
//	}
//
//	//--- inputLayout �� vertexShader �̍쐬 ---
//	HRESULT hr = S_OK;
//	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
//	{
//		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
//		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
//		{"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
//		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
//		{"WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
//		{"BONES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
//	};
//	CreateVsFromCso("Data/Shader/SkinnedMesh_VS.cso", vertexShader.ReleaseAndGetAddressOf(), inputLayout.ReleaseAndGetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
//	//--- pixelShader �̍쐬 ---
//	CreatePsFromCso("Data/Shader/SkinnedMesh_PS.cso", pixelShader.ReleaseAndGetAddressOf());
//
//	//--- constantBuffer �̍쐬 ---
//	D3D11_BUFFER_DESC bufferDesc{};
//	bufferDesc.ByteWidth = sizeof(Constants);
//	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.ReleaseAndGetAddressOf());
//	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
//
//	//--- isBloomConstantBuffer �̍쐬 ---
//	D3D11_BUFFER_DESC isBloombBufferDesc{};
//	isBloombBufferDesc.ByteWidth = sizeof(IsBloomConstants);
//	isBloombBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	isBloombBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	hr = device->CreateBuffer(&isBloombBufferDesc, nullptr, isBloomConstantBuffer.ReleaseAndGetAddressOf());
//	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
//
//	// material �̖��O�ɑΉ����� �e�N�X�`������shaderResourceView�̐���
//	for (std::unordered_map<uint64_t, Material>::iterator iterator = materials.begin(); iterator != materials.end(); ++iterator)
//	{
//		for (size_t textureIndex = 0; textureIndex < 4; textureIndex++)
//		{
//			if (iterator->second.textureFilenames[textureIndex].size() > 0)
//			{
//				std::filesystem::path path(fbxFilename);
//				path.replace_filename(iterator->second.textureFilenames[textureIndex]);
//				D3D11_TEXTURE2D_DESC texture2dDesc{};
//				LoadTextureFromFile(path.c_str(), iterator->second.shaderResourceViews[textureIndex].GetAddressOf(), &texture2dDesc);
//			}
//			else
//			{
//				MakeDummyTexture(iterator->second.shaderResourceViews[textureIndex].GetAddressOf(), textureIndex == 1 ? 0xFFFF7F7F : 0xFFFFFFFF, 16);
//			}
//		}
//	}
//}
//
//// �_�~�[�e�N�X�`���̐���
//HRESULT SkinnedMesh::MakeDummyTexture(ID3D11ShaderResourceView** shaderResourceView, DWORD value, UINT dimension)
//{
//	// --- Graphics �擾 ---
//	Graphics* gfx = &Graphics::Instance();
//	ID3D11Device* device = gfx->GetDevice();
//
//	//--- �_�~�[�e�N�X�`���̐��� ----
//	HRESULT hr = S_OK;
//
//	D3D11_TEXTURE2D_DESC texture2dDesc{};
//	texture2dDesc.Width = dimension;
//	texture2dDesc.Height = dimension;
//	texture2dDesc.MipLevels = 1;
//	texture2dDesc.ArraySize = 1;
//	texture2dDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//	texture2dDesc.SampleDesc.Count = 1;
//	texture2dDesc.SampleDesc.Quality = 0;
//	texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
//	texture2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
//
//	size_t texels = dimension * dimension;
//	unique_ptr<DWORD[]> sysmem{ make_unique<DWORD[]>(texels) };
//	for (size_t i = 0; i < texels; ++i) sysmem[i] = value;
//
//	D3D11_SUBRESOURCE_DATA subresourceData{};
//	subresourceData.pSysMem = sysmem.get();
//	subresourceData.SysMemPitch = sizeof(DWORD) * dimension;
//
//	ComPtr<ID3D11Texture2D> texture2d;
//	hr = device->CreateTexture2D(&texture2dDesc, &subresourceData, &texture2d);
//	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
//
//	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
//	shaderResourceViewDesc.Format = texture2dDesc.Format;
//	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
//	shaderResourceViewDesc.Texture2D.MipLevels = 1;
//	hr = device->CreateShaderResourceView(texture2d.Get(), &shaderResourceViewDesc, shaderResourceView);
//	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
//
//	return hr;
//}
//
//// �_�~�[�}�e���A���̐���
//void SkinnedMesh::MakeDummyMaterial()
//{
//	Material material;
//	material.name = "dummyMaterial";
//	material.uniqueId = 0;
//	material.Kd.x = 1.0f;
//	material.Kd.y = 1.0f;
//	material.Kd.z = 1.0f;
//	material.Kd.w = 1.0f;
//
//	materials.emplace(material.uniqueId, std::move(material));
//}
//
//// CoordinateSystemTransform ���ݒ�
//void SkinnedMesh::SetCoordinateSystemTransform(CoordinateSystemTransform coordinateSystemTransform)
//{
//	this->coordinateSystemIndex = static_cast<int>(coordinateSystemTransform);
//
//	this->fbxUnit = static_cast<int>(fbxUnit);
//
//	// model�̏o�̓t�@�C���쐬
//	std::filesystem::path path(fbxPath);
//	std::string modelFilePath = parentPath + "/" + path.stem().string() + ".model";
//
//	// �o��
//		// �o��
//#if 0
//	std::ofstream ofs(modelFilePath.c_str(), std::ios::binary);
//	cereal::BinaryOutputArchive serialization(ofs);
//#else
//	std::ofstream ofs(modelFilePath.c_str(), std::ios::out);
//	cereal::JSONOutputArchive  serialization(ofs);
//#endif
//	serialization(sceneView, this->coordinateSystemIndex, this->fbxUnit, skeletonSpheres);
//}
//
//// fbx �̒P�ʐݒ�
//void SkinnedMesh::SetFbxUnit(FbxUnit fbxUnit)
//{
//	this->fbxUnit = static_cast<int>(fbxUnit);
//
//	// model�̏o�̓t�@�C���쐬
//	std::filesystem::path path(fbxPath);
//	std::string modelFilePath = parentPath + "/" + path.stem().string() + ".model";
//
//	// �o��
//		// �o��
//#if 0
//	std::ofstream ofs(modelFilePath.c_str(), std::ios::binary);
//	cereal::BinaryOutputArchive serialization(ofs);
//#else
//	std::ofstream ofs(modelFilePath.c_str(), std::ios::out);
//	cereal::JSONOutputArchive  serialization(ofs);
//#endif
//	serialization(sceneView, this->coordinateSystemIndex, this->fbxUnit, skeletonSpheres);
//}

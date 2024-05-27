#include <functional>
#include "LoadFbx.h"
#include "../../ErrorLogger.h"
#include "../../ImGui/ConsoleData.h"
#include "../../Graphics/Graphics.h"
#include "../../RegisterNum.h"

using namespace DirectX;

// ===== 補助関数 =============================================
#pragma region 補助関数

//--------------------------------------------------------------
// Fbx の行列から DirectX の行列に変換する関数
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
//	Fbx を読み込んで ModelResource に変換する関数
//------------------------------------------------------------
// fbxFilename	: 読み込む fbx のパス
// triangulate	: 三角化するかどうか
std::shared_ptr<ModelResource> LoadFbx::Load(const char* fbxFilename, bool triangulate)
{
	// ノードデータをクリア(同じシーンで２回目以降のロードの対応のため)
	fbxNodeGroup.fbxDataNodes.clear();

	std::shared_ptr<ModelResource> modelResource = std::make_unique<ModelResource>();

	// fbx ファイルパスの保存
	modelResource->SetFilePath(fbxFilename);

	// 軸変換
	modelResource->SetCoordinateSystemTransformType(CoordinateSystemTransform::LHS_Y_UP);
	modelResource->SetCoordinateSystemTransform({ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 });
	
	// FbxManager の作成
	FbxManager* fbxManager = FbxManager::Create();
	// FbxScene の作成
	FbxScene* fbxScene = FbxScene::Create(fbxManager, "");
	// FbxImporter の作成
	FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");

	// ファイルの初期化
	bool importStatus = false;
	importStatus = fbxImporter->Initialize(fbxFilename);
	_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

	// ファイルのインポート
	importStatus = fbxImporter->Import(fbxScene);
	_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

	// 3Dモデルの不要なポリゴンの除去
	FbxGeometryConverter fbxConverter(fbxManager);
	if (triangulate)
	{
		// ポリゴンを３角形に変換する(重い処理)
		fbxConverter.Triangulate(fbxScene, true/*replace*/, false/*legacy*/);
		fbxConverter.RemoveBadPolygonsFromMeshes(fbxScene);
	}

	std::function<void(FbxNode*)> traverse
	{
		[&](FbxNode* fbxNode)
		{
			// node を emplace_back()
			FbxNodeGroup::FbxDataNode& node = fbxNodeGroup.fbxDataNodes.emplace_back();
			// node の属性取得
			node.attribute = fbxNode->GetNodeAttribute() ? fbxNode->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eBoundary;
			// node の名前取得
			node.name = fbxNode->GetName();
			// node の id 取得
			node.uniqueId = fbxNode->GetUniqueID();
			// 親 node のインデックス取得
			node.parentIndex = fbxNodeGroup.GetIndex(fbxNode->GetParent() ? fbxNode->GetParent()->GetUniqueID() : 0);

			ModelResource::Node& nodeData = modelResource->GetSceneView().nodes.emplace_back();
			nodeData.name = node.name;
			nodeData.uniqueId = node.uniqueId;
			nodeData.parentIndex = node.parentIndex;


			// 子 node を traverse()
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

	// メッシュの取得
	FetchMesh(fbxScene, modelResource.get());
	// マテリアルの取得
	FetchMaterial(fbxScene, modelResource.get());
	// アニメーションの取得
	FetchAnimations(fbxScene, modelResource.get(), 30);


	// 破棄
	fbxImporter->Destroy();
	fbxScene->Destroy();
	fbxManager->Destroy();

	return modelResource;
}

//------------------------------------------------------------
//	fbxScene から mesh データを取得
//------------------------------------------------------------
// fbxScene			: mesh データの取得元の FbxScene
// modelResource	: 取得したメッシュデータの格納先
void LoadFbx::FetchMesh(FbxScene* fbxScene, ModelResource* modelResource)
{
	// すべての FbxDataNode 構造体 をチェック
	for (const FbxNodeGroup::FbxDataNode& node : fbxNodeGroup.fbxDataNodes)
	{
		// node構造体 のタイプが mesh 以外なら continue
		if (node.attribute != FbxNodeAttribute::EType::eMesh)
		{
			continue;
		}
		// fbxNode の取得
		FbxNode* fbxNode = fbxScene->FindNodeByName(node.name.c_str());
		// fbxMesh の取得
		FbxMesh* fbxMesh = fbxNode->GetMesh();

		// mesh構造体を作成し meshes に push
		ModelResource::Mesh& mesh = modelResource->GetMeshes().emplace_back();
		mesh.meshName = fbxMesh->GetNode()->GetName();
		mesh.uniqueId = fbxMesh->GetNode()->GetUniqueID();
		mesh.defaultGlobalTransform = toXmfloat4x4(fbxMesh->GetNode()->EvaluateGlobalTransform());	// モデル空間の原点からみた姿勢
		mesh.nodeIndex = modelResource->GetSceneView().GetIndex(mesh.uniqueId);


		// 頂点に対するボーンの影響度ベクターの作成して情報を取得
		std::vector<BoneInfluencesPerControlPoint> boneInfluences;
		FetchBoneInfluence(fbxMesh, boneInfluences);
		// bone データの取得
		FetchSkeleton(fbxMesh, mesh.skeleton, modelResource);


		// Subset のベクターを mesh の subsets から作成
		std::vector<ModelResource::Subset>& subsets = mesh.subsets;
		// material の数を取得
		const int materialCount = fbxMesh->GetNode()->GetMaterialCount();
		// subsets のサイズをマテリアルの数にする
		subsets.resize(materialCount > 0 ? materialCount : 1);

		for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex)
		{
			// index の material を取得
			const FbxSurfaceMaterial* fbxMaterial = fbxMesh->GetNode()->GetMaterial(materialIndex);
			// subset に name をセット
			subsets.at(materialIndex).materialName = fbxMaterial->GetName();
		}
		if (materialCount > 0)
		{
			// mesh のポリゴンの数を取得
			const int polygonCount = fbxMesh->GetPolygonCount();
			for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
			{
				// ポリゴンインデックスに対応する material の index を取得
				// ポリゴンのindexを渡すとmatrialのindexを返してくれる
				// ポリゴンx3だけindexCountを加算する
				const int materialIndex = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex);
				// 対応する materialIndex の indexCount を頂点の数(3)加算する
				subsets.at(materialIndex).indexCount += 3;
			}
			// subsets 内の startIndex の調整
			uint32_t offset{ 0 };
			for (ModelResource::Subset& subset : subsets)
			{
				subset.startIndex = offset;
				offset += subset.indexCount;
				subset.indexCount = 0;
			}
		}
		else
		// ここでやってるの違う気がする(FetchMaterialに入れたほうが綺麗なのでは)
		{
			// マテリアルがなければダミーマテリアルを作成
			MakeDummyMaterial(modelResource);
		}

		// ポリゴン数を取得
		const int polygonCount = fbxMesh->GetPolygonCount();
		mesh.vertices.resize(polygonCount * 3LL);	// vertices のサイズをポリゴンの３倍にリサイズ
		mesh.indices.resize(polygonCount * 3LL);	// indices のサイズをポリゴンの３倍にリサイズ

		// UVSet の名前を取得
		FbxStringList uvNames;
		fbxMesh->GetUVSetNames(uvNames);

		// コントロールポイント(頂点座標)を取得
		const FbxVector4* controlPoints = fbxMesh->GetControlPoints();

		// ポリゴンの数だけ繰り返す
		for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
		{
			// ポリゴンインデックスに対応する material の index を取得
			const int materialIndex = materialCount > 0 ? fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex) : 0;
			// 対応する materialIndex の subset を取得
			ModelResource::Subset& subset = subsets.at(materialIndex);
			const uint32_t offset = subset.startIndex + subset.indexCount;

			// 頂点の数(3)だけ繰り返す
			for (int positionInPolygon = 0; positionInPolygon < 3; ++positionInPolygon)
			{
				// 現在の頂点の index を計算
				const int vertexIndex = polygonIndex * 3 + positionInPolygon;

				// 頂点情報(vertex)の作成
				ModelResource::Vertex vertex;
				const int polygonVertex = fbxMesh->GetPolygonVertex(polygonIndex, positionInPolygon);	// polygonIndex のポリゴンの positionInPolygon の頂点の index を取得
				vertex.position.x = static_cast<float>(controlPoints[polygonVertex][0]);	// index に対応した頂点座標をセット
				vertex.position.y = static_cast<float>(controlPoints[polygonVertex][1]);
				vertex.position.z = static_cast<float>(controlPoints[polygonVertex][2]);

				// 頂点に対応する ボーンのインデックスとウェイトをセットする
				const BoneInfluencesPerControlPoint& influencesPerControlPoint = boneInfluences.at(polygonVertex);
				for (size_t influenceIndex = 0; influenceIndex < influencesPerControlPoint.size(); ++influenceIndex)
				{
					if (influenceIndex < ModelResource::MAX_BONE_INFLUENCES)
					{
						// vertex の weight と index に influencesPerControlPoint の値をセットする
						vertex.boneWeights[influenceIndex] = influencesPerControlPoint.at(influenceIndex).boneWeight;
						vertex.boneIndices[influenceIndex] = influencesPerControlPoint.at(influenceIndex).boneIndex;
					}
				}

				// mesh に法線情報があるなら vertex にセットする
				if (fbxMesh->GetElementNormalCount() > 0)
				{
					FbxVector4 normal;
					fbxMesh->GetPolygonVertexNormal(polygonIndex, positionInPolygon, normal);
					vertex.normal.x = static_cast<float>(normal[0]);
					vertex.normal.y = static_cast<float>(normal[1]);
					vertex.normal.z = static_cast<float>(normal[2]);
				}
				// mesh にUV(テクスチャ座標)情報があるなら vertex にセットする
				if (fbxMesh->GetElementUVCount() > 0)
				{
					FbxVector2 uv;
					bool unmappedUv;
					fbxMesh->GetPolygonVertexUV(polygonIndex, positionInPolygon, uvNames[0], uv, unmappedUv);
					vertex.texcoord.x = static_cast<float>(uv[0]);
					vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
				}
				// 法線ベクトルの値のセット(法線ベクトルを持たない場合は GenerateTangentsData で生成)
				if (fbxMesh->GenerateTangentsData(0, false))
				{
					const FbxGeometryElementTangent* tangent = fbxMesh->GetElementTangent(0);
					vertex.tangent.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[0]);
					vertex.tangent.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[1]);
					vertex.tangent.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[2]);
					vertex.tangent.w = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[3]);
				}

				// mesh の 頂点情報ベクター(vertices)に作成した頂点情報(vertex)をセットする
				mesh.vertices.at(vertexIndex) = std::move(vertex);
				// mesh の インデックス情報ベクター(indices)に現在の index をセットする
				mesh.indices.at(static_cast<size_t>(offset) + positionInPolygon) = vertexIndex;
				subset.indexCount++;
			}
		}
	}
}

//------------------------------------------------------------
//	fbxScene から material データを取得
//------------------------------------------------------------
// fbxScene			: mesh データの取得元の FbxScene
// modelResource	: 取得したマテリアルデータの格納先
void LoadFbx::FetchMaterial(FbxScene* fbxScene, ModelResource* modelResource)
{
	// すべての FbxDataNode 構造体 をチェック
	for (const FbxNodeGroup::FbxDataNode& node : fbxNodeGroup.fbxDataNodes)
	{
		if (node.attribute != FbxNodeAttribute::EType::eMesh) continue;

		// fbxNode の取得
		const FbxNode* fbxNode = fbxScene->FindNodeByName(node.name.c_str());

		// 取得した node が持つ material の数を取得
		const int materialCount = fbxNode->GetMaterialCount();
		for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex)
		{
			// マテリアルはメッシュが持っているため mesh 以外調べなくてよい
			const FbxSurfaceMaterial* fbxMaterial = fbxNode->GetMaterial(materialIndex);

			// マテリアルの作成
			ModelResource::Material material;
			material.name = fbxMaterial->GetName();

			// Diffuse プロパティを取得
			FbxProperty fbxProperty;
			fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
			if (fbxProperty.IsValid())
			{
				const FbxDouble3 color = fbxProperty.Get<FbxDouble3>();
				material.Kd.x = static_cast<float>(color[0]);
				material.Kd.y = static_cast<float>(color[1]);
				material.Kd.z = static_cast<float>(color[2]);
				material.Kd.w = 1.0f;

				// fbxProperty (Diffuse) に関連づけられたテクスチャの取得
				const FbxFileTexture* fbxTexture = fbxProperty.GetSrcObject<FbxFileTexture>();
				material.textureFilenames[_deffuseTexture] = fbxTexture ? fbxTexture->GetRelativeFileName() : "";
			}
			// Specular プロパティを取得
			fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
			if (fbxProperty.IsValid())
			{
				const FbxDouble3 color = fbxProperty.Get<FbxDouble3>();
				material.Ks.x = static_cast<float>(color[0]);
				material.Ks.y = static_cast<float>(color[1]);
				material.Ks.z = static_cast<float>(color[2]);
				material.Ks.w = 1.0f;

				// fbxProperty に関連づけられたテクスチャの取得
				const FbxFileTexture* fbxTexture = fbxProperty.GetSrcObject<FbxFileTexture>();
				material.textureFilenames[_specularTexture] = fbxTexture ? fbxTexture->GetRelativeFileName() : "";
			}
			// Ambient プロパティを取得
			fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sAmbient);
			if (fbxProperty.IsValid())
			{
				const FbxDouble3 color = fbxProperty.Get<FbxDouble3>();
				material.Ka.x = static_cast<float>(color[0]);
				material.Ka.y = static_cast<float>(color[1]);
				material.Ka.z = static_cast<float>(color[2]);
				material.Ka.w = 1.0f;

				// fbxProperty に関連づけられたテクスチャの取得
				//const FbxFileTexture* fbxTexture = fbxProperty.GetSrcObject<FbxFileTexture>();
				//material.textureFilenames[] = fbxTexture ? fbxTexture->GetRelativeFileName() : "";
			}

			modelResource->GetMaterials().emplace(material.name, std::move(material));
		}
	}
}


//------------------------------------------------------------
//	fbxMesh から 各頂点に対する bone の影響力を取得
//------------------------------------------------------------
// fbxMesh			: 頂点データ bone データの取得元の FbxMesh
// boneInfluences	: 取得した影響力データの格納先
void LoadFbx::FetchBoneInfluence(FbxMesh* fbxMesh, std::vector<BoneInfluencesPerControlPoint>& boneInfluences)
{
	// --- ボーン影響度構造体のサイズをmeshの持つ頂点の数に変更 ---
	const int controlPointsCount = fbxMesh->GetControlPointsCount();
	boneInfluences.resize(controlPointsCount);

	// すべての deformer (FbxSkin) を処理 
	const int skinCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int skinIndex = 0; skinIndex < skinCount; ++skinIndex)
	{
		// 処理する deformer の取得	
		const FbxSkin* fbxSkin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(skinIndex, FbxDeformer::eSkin));

		// この deformer がもつ全ての cluster を処理する --- (すべてのボーンの処理)
		const int clusterCount = fbxSkin->GetClusterCount();
		for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)
		{
			// 現在の index の cluster を取得 --- (処理するボーンの取得)
			const FbxCluster* fbxCluster = fbxSkin->GetCluster(clusterIndex);

			// cluster が影響を与える頂点の数を取得(1つの cluster が３つの頂点に影響を与える場合３を返す) --- (ボーンが影響を与える全ての頂点を処理)
			const int controlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();

			for (int controlPointIndicesIndex = 0; controlPointIndicesIndex < controlPointIndicesCount; ++controlPointIndicesIndex)
			{
				// index と weight を push する
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
//	fbxMesh から bone データを取得
//------------------------------------------------------------
// fbxMesh		: bone データの取得元の FbxMesh
// skeleton		: 取得したボーンデータの格納先
void LoadFbx::FetchSkeleton(FbxMesh* fbxMesh, ModelResource::Skeleton& skeleton, ModelResource* modelResource)
{
	// すべての deformer (FbxSkin) を処理 
	const int deformerCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int deformerIndex = 0; deformerIndex < deformerCount; ++deformerIndex)
	{
		// 処理する deformer の取得	
		FbxSkin* skin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
	
		// この deformer がもつ全ての cluster を処理する --- (すべてのボーンの処理)
		const int clusterCount = skin->GetClusterCount();
	
		// 処理しているメッシュが持つSkeletonのBoneの大きさをボーンの数にする
		skeleton.bones.resize(clusterCount);
	
		for (int clusterIndex = 0; clusterIndex < clusterCount; clusterIndex++)
		{
			// 現在の index の cluster を取得 --- (処理するボーンの取得)
			FbxCluster* cluster = skin->GetCluster(clusterIndex);
	
			// ボーンの情報を取得してセット
			ModelResource::Bone& bone = skeleton.bones.at(clusterIndex);
			bone.name = cluster->GetLink()->GetName();
			bone.uniqueId = cluster->GetLink()->GetUniqueID();
			bone.parentIndex = skeleton.GetIndex(cluster->GetLink()->GetParent()->GetUniqueID());
			bone.nodeIndex = modelResource->GetSceneView().GetIndex (bone.uniqueId);
	
			// メッシュの初期グローバル行列を取得
			FbxAMatrix referenceGlobalInitPosition;
			cluster->GetTransformMatrix(referenceGlobalInitPosition);
	
			// ボーンの初期グローバル行列を取得
			FbxAMatrix clusterGlobalInitPosition;
			cluster->GetTransformLinkMatrix(clusterGlobalInitPosition);
	
			// メッシュ空間の頂点座標をグローバル空間の頂点座標にする行列と
			// グローバル空間の頂点座標をボーン空間の頂点座標にする行列を掛け算することで
			// メッシュ空間からボーン空間へ変換するオフセット行列を作成する
			bone.offsetTransform = toXmfloat4x4(clusterGlobalInitPosition.Inverse() * referenceGlobalInitPosition);
	
			// スキニング後の頂点座標 = メッシュ空間の頂点座標 * オフセット行列
		}
	}
}

//------------------------------------------------------------
//	fbxScene から Animation データを取得
//------------------------------------------------------------
// fbxMesh			: bone データの取得元の FbxMesh
// modelResource	: 取得したボーンデータの格納先
// samplingRate		: アニメーションのサンプリングを (1 / samplingRate) 秒で行う
void LoadFbx::FetchAnimations(FbxScene* fbxScene, ModelResource* modelResource, float samplingRate)
{
	// 全てのアニメーションスタックの名前の取得
	FbxArray<FbxString*> animationStackNames;
	fbxScene->FillAnimStackNameArray(animationStackNames);
	
	
	// 全てのアニメーションスタックを処理する
	const int animationStackCount = animationStackNames.GetCount();
	
	for (int animationStackIndex = 0; animationStackIndex < animationStackCount; animationStackIndex++)
	{
		// 全てのアニメーションスタックの名前の取得
		FbxArray<FbxString*> animationStackNames;
		fbxScene->FillAnimStackNameArray(animationStackNames);

		// 全てのアニメーションスタックを処理する
		const int animationStackCount = animationStackNames.GetCount();

		for (int animationStackIndex = 0; animationStackIndex < animationStackCount; animationStackIndex++)
		{
			// --- ここから各アニメーションスタックに対する処理 ---

			// 出力先の animationClips に同じ名前があればスキップ
			bool isContinue = false;
			for (int i = 0; i < modelResource->GetAnimationClips().size(); i++)
			{
				std::string animationStackName = animationStackNames[animationStackIndex]->Buffer();
				std::string animationName = modelResource->GetAnimationClips().at(i).name;
				if (animationStackName == animationName) isContinue = true;
			}
			if (isContinue) continue;

			// animeClipの名前に アニメーションスタック名をいれる
			ModelResource::Animation& animationClip = modelResource->GetAnimationClips().emplace_back();
			animationClip.name = animationStackNames[animationStackIndex]->Buffer();

			// index のアニメーションスタックを取得
			FbxAnimStack* animationStack = fbxScene->FindMember<FbxAnimStack>(animationClip.name.c_str());
			fbxScene->SetCurrentAnimationStack(animationStack);

			// --- サンプリングレートとサンプリング間隔の設定 ---

			// タイムモードの取得（時間の表現方法の指定）
			const FbxTime::EMode timeMode = fbxScene->GetGlobalSettings().GetTimeMode();
			// タイムモードに対応する１秒の表現をするための変数を用意
			FbxTime oneSecond;
			oneSecond.SetTime(0, 0, 1, 0, 0, timeMode);
			// サンプリングレートの指定
			animationClip.samplingRate = samplingRate > 0 ? samplingRate : static_cast<float>(oneSecond.GetFrameRate(timeMode));
			// サンプリング間隔の計算(この秒数ごとにサンプリングを行う)
			const FbxTime samplingInterval = static_cast<FbxLongLong>(oneSecond.Get() / animationClip.samplingRate);
			// アニメーションの開始時間と終了時間の取得
			const FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(animationClip.name.c_str());
			const FbxTime startTime = takeInfo->mLocalTimeSpan.GetStart();
			const FbxTime stopTime = takeInfo->mLocalTimeSpan.GetStop();

			// アニメーション終了時間の保存
			double doubleSeconds = stopTime.GetSecondDouble() - startTime.GetSecondDouble();
			//double doubleSeconds = fbxScene->GetTakeInfo(animationClip.name.c_str())->mLocalTimeSpan.GetStop().GetSecondDouble();
			animationClip.secondsLength = static_cast<float>(doubleSeconds);

			// --- 各キーフレームに対する処理 ---
			// アニメーションの開始時間から終了時間まで、サンプリング間隔ごとにキーフレームを生成している
			for (FbxTime time = startTime; time < stopTime; time += samplingInterval)
			{
				// キーフレームを生成してシークエンスにemplace_backしている
				ModelResource::KeyFrame& keyFrame = animationClip.sequence.emplace_back();

				// キーフレームの node の数を このモデルがもつnodeの数にする
				const size_t nodeCount = modelResource->GetSceneView().nodes.size();
				keyFrame.nodes.resize(nodeCount);

				// すべての node の処理
				for (size_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
				{
					FbxNode* fbxNode = fbxScene->FindNodeByName(modelResource->GetSceneView().nodes.at(nodeIndex).name.c_str());
					if (fbxNode)
					{
						ModelResource::KeyFrame::Node& node = keyFrame.nodes.at(nodeIndex);

						//	uniqueId と名前のセット
						node.uniqueId = fbxNode->GetUniqueID();
						node.name = fbxNode->GetName();

						// node の グローバル変換行列にいれる
						node.globalTransform = toXmfloat4x4(fbxNode->EvaluateGlobalTransform(time));

						// node ローカル の scaling rotation translation にそれぞれ変換行列をいれる
						const FbxAMatrix& localTransform = fbxNode->EvaluateLocalTransform(time);
						node.scaling = toXmfloat3(localTransform.GetS());
						node.rotation = toXmfloat4(localTransform.GetQ());
						node.translation = toXmfloat3(localTransform.GetT());
					}
				}
			}
		}
	}
	// 全てのアニメーションスタック名の解放
	for (int animationStackIndex = 0; animationStackIndex < animationStackCount; animationStackIndex++)
	{
		delete animationStackNames[animationStackIndex];
	}
}

//------------------------------------------------------------
//	fbxScene から Animation データを取得して追加を行う
//------------------------------------------------------------
// animationFileName	: 取得元の fbx のパス
// modelResource		: 取得したボーンデータの格納先
// samplingRate			: アニメーションのサンプリングを (1 / samplingRate) 秒で行う
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
//	ダミーマテリアルの生成
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
//	ダミーテクスチャの生成
//------------------------------------------------------------
void LoadFbx::MakeDummyTexture(ID3D11ShaderResourceView** shaderResourceView, DWORD value, UINT dimension)
{
	// --- Graphics 取得 ---
	Graphics* gfx = &Graphics::Instance();
	ID3D11Device* device = gfx->GetDevice();

	//--- ダミーテクスチャの生成 ----
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


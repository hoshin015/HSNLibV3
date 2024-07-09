// --- Library ---
#include "../../Library/Timer.h"
// --- Game ---
#include "AnimatedObject.h"
#include "../../../../Library/3D/DebugPrimitive.h"

AnimatedObject::AnimatedObject(const char* filePath)
{
	model = std::make_unique<AnimatedModel>(filePath);
}

void AnimatedObject::UpdateTransform()
{
	// スケール変更
	DirectX::XMMATRIX MS = DirectX::XMMatrixScaling(model->GetModelResource()->GetScale(),
	                                                model->GetModelResource()->GetScale(),
	                                                model->GetModelResource()->GetScale());

	// 軸変換行列
	DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&model->GetModelResource()->GetCoordinateSystemTransform());

	// スケール行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(GetScaleX(), GetScaleY(), GetScaleZ());
	// 回転行列を作成
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(GetAngleX()),
	                                                            DirectX::XMConvertToRadians(GetAngleY()),
	                                                            DirectX::XMConvertToRadians(GetAngleZ()));
	// 位置行列を作成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(GetPosX(), GetPosY(), GetPosZ());

	// ４つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX W = MS * S * R * T * C;
	DirectX::XMStoreFloat4x4(&transform, W);
}

void AnimatedObject::DrawDebugPrimitive()
{
	if (!model->GetModelResource()->GetAnimationClips().empty())
	{
		for (auto& sphere : model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex).animSphereCollisions)
		{
			if (sphere.startFrame <= currentKeyFrame && sphere.endFrame >= currentKeyFrame)
			{
				DirectX::XMFLOAT3 pos = GetBonePosition(sphere.bindBoneName);
				DebugPrimitive::Instance().AddSphere(pos, sphere.radius, sphere.color);
			}
		}
	}

	for (auto& bone : model->GetModelResource()->GetSkeletonSphereCollisions())
	{
		DirectX::XMFLOAT3 pos;
		if (bone.name != "")
		{
			pos = GetBonePosition(bone.name);
		}
		else
		{
			DirectX::XMVECTOR BONE_POS = DirectX::XMLoadFloat3(&bone.position);
			DirectX::XMVECTOR POS = DirectX::XMLoadFloat3(&position);
			DirectX::XMStoreFloat3(&pos, DirectX::XMVectorAdd(BONE_POS, POS));
		}

		DirectX::XMFLOAT4 color = bone.color[static_cast<int>(bone.skeletonType)];
		DebugPrimitive::Instance().AddSphere(pos, bone.radius, color);
	}
}

void AnimatedObject::PlayAnimation(int index, bool loop)
{
	if (GetCurrentAnimationIndex() == index && animationLoopFlag == loop) return;

	currentAnimationIndex   = index;
	currentAnimationSeconds = 0.0f;

	animationLoopFlag = loop;
	animationEndFlag  = false;

	ClearAnimSphereCollisionDamagedFlag();
	ClearSeFlag();
}

// アニメーション更新
void AnimatedObject::UpdateAnimation()
{
	// 再生中でないなら更新しない
	if (!isPlayAnimation) return;

	// アニメーションクリップを持たない場合処理しない
	if (model->GetModelResource()->GetAnimationClips().empty()) return;

	// 最終フレームなら更新しない
	if (animationEndFlag) return;

	// アニメーションデータ取得
	ModelResource::Animation& animation = model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex);

	currentKeyFrame = static_cast<int>(currentAnimationSeconds * animation.samplingRate);

	// 経過時間
	currentAnimationSeconds += Timer::Instance().DeltaTime() * animationTimeScale;
	// 現在のフレームが最大フレームを超えていたら
	if (currentAnimationSeconds >= animation.secondsLength)
	{
		if (animationLoopFlag)
		{
			// 先頭秒数へ
			currentKeyFrame = 0;
			currentAnimationSeconds -= animation.secondsLength;

			ClearAnimSphereCollisionDamagedFlag();
			ClearSeFlag();
		}
		else
		{
			// 最終秒数で停止
			currentKeyFrame         = animation.sequence.size() - 1;
			currentAnimationSeconds = animation.secondsLength;
			animationEndFlag        = true;
		}
	}

	// keyFrame設定
	keyFrame = animation.sequence.at(currentKeyFrame);


	// ダブルアニメーションが有効なら上半身のみ別キーにする
	if (isDoubleAnimations)
	{
		// ダブルアニメーションデータ取得
		ModelResource::Animation& doubleAnimation = model->GetModelResource()->GetAnimationClips().at(
			doubleCurrentAnimationIndex);

		doubleCurrentKeyFrame = static_cast<int>(doubleCurrentAnimationSeconds * animation.samplingRate);

		// 経過時間
		doubleCurrentAnimationSeconds += Timer::Instance().DeltaTime();

		// 現在のフレームが最大フレームを超えていたら
		if (doubleCurrentAnimationSeconds >= doubleAnimation.secondsLength)
		{
			// ダブルアニメーションをOFF
			isDoubleAnimations            = false;
			doubleCurrentKeyFrame         = 0;
			doubleCurrentAnimationSeconds = 0.0f;

			return;
		}

		doubleKeyFrame = doubleAnimation.sequence.at(doubleCurrentKeyFrame);

		// 上半身アニメーションを使用するノードの index を格納するベクター (親行列が上半身アニメーション)
		std::vector<uint32_t> doubleAnimationNodeIndex;

		//node の数だけ繰り返す
		size_t nodeCount = doubleKeyFrame.nodes.size();
		for (size_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
		{
			// 現在の index の node を取得
			ModelResource::KeyFrame::Node& keyData = doubleKeyFrame.nodes.at(nodeIndex);

			// それぞれの値に対応する行列の作成
			DirectX::XMMATRIX S = DirectX::XMMatrixScaling(keyData.scaling.x, keyData.scaling.y, keyData.scaling.z);
			DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(XMLoadFloat4(&keyData.rotation));
			DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(keyData.translation.x, keyData.translation.y,
			                                                   keyData.translation.z);

			// 親行列があれば親の行列をかける
			uint32_t uniqueIndex = model->GetModelResource()->GetSceneView().GetIndex(keyData.uniqueId);
			// このノードのuniqueId 取得
			int64_t parentIndex = model->GetModelResource()->GetSceneView().nodes.at(uniqueIndex).parentIndex;
			// このuniqueId の parent の親の index を取得

			// 乗算すべき親行列がどちらのアニメーションかを調べる
			bool doubleAnimationParent = false;
			for (auto checkIndex : doubleAnimationNodeIndex)
			{
				if (parentIndex == checkIndex)
				{
					doubleAnimationParent = true;                    // 親はダブルアニメーションの行列を使用
					doubleAnimationNodeIndex.push_back(uniqueIndex); // このノードの子のために追加
					break;
				}
			}

			DirectX::XMMATRIX P;

			// ここの名前は変えれるようにしたほうがよさそう
			if (keyData.name == "mixamorig:Spine")
			{
				doubleAnimationNodeIndex.push_back(uniqueIndex);
				doubleAnimationParent = true; // 親はダブルアニメーションの行列を使用(こっちは場合によってはやめたほうがいいかも)
			}

			if (doubleAnimationParent)
			{
				P = parentIndex < 0
					    ? DirectX::XMMatrixIdentity()
					    : DirectX::XMLoadFloat4x4(&keyFrame.nodes.at(parentIndex).globalTransform);
				XMStoreFloat4x4(&keyData.globalTransform, S * R * T * P);
				keyFrame.nodes.at(nodeIndex).globalTransform = keyData.globalTransform;
			}
		}
	}


	// アニメーションブレンド(次のアニメーションへの自然な遷移)
	// 現在のアニメーションがループアニメーションなら今から遷移
	// 現在のアニメーションが非ループアニメーションなら指定した時間から指定した時間を掛けて遷移
	if (blendAnimationIndex != -1)
	{
		float blendRate = 1.0f;
		if (animationBlendTransitionDurations > animationBlendTimer)
		{
			animationBlendTimer += Timer::Instance().DeltaTime();
			if (animationBlendTimer > animationBlendTransitionDurations)
			{
				animationBlendTimer = animationBlendTransitionDurations;
			}
			blendRate = animationBlendTimer / animationBlendTransitionDurations;
			blendRate *= blendRate;

			ModelResource::Animation& blendAnimation = model->GetModelResource()->GetAnimationClips().at(
				blendAnimationIndex);
			int blendAnimationCurrentKeyFrame = static_cast<int>(animationBlendTimer * blendAnimation.samplingRate);

			ModelResource::KeyFrame key = keyFrame;

			const ModelResource::KeyFrame* keyFrames[2] =
			{
				&blendAnimation.sequence.at(blendAnimationCurrentKeyFrame), // 次のアニメーションのキーフレーム
				&keyFrame,                                                  // 現在再生中のキーフレーム
			};
			BlendAnimation(keyFrames, blendRate, keyFrame);


			//node の数だけ繰り返す
			size_t nodeCount = keyFrame.nodes.size();
			for (size_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
			{
				// 現在の index の node を取得
				ModelResource::KeyFrame::Node& keyData = keyFrame.nodes.at(nodeIndex);

				// それぞれの値に対応する行列の作成
				DirectX::XMMATRIX S = DirectX::XMMatrixScaling(keyData.scaling.x, keyData.scaling.y, keyData.scaling.z);
				DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(XMLoadFloat4(&keyData.rotation));
				DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(keyData.translation.x, keyData.translation.y,
				                                                   keyData.translation.z);

				// 親行列があれば親の行列をかける
				uint32_t uniqueIndex = model->GetModelResource()->GetSceneView().GetIndex(keyData.uniqueId);
				// このノードのuniqueId 取得
				int64_t parentIndex = model->GetModelResource()->GetSceneView().nodes.at(uniqueIndex).parentIndex;
				// このuniqueId の parent の親の index を取得

				DirectX::XMMATRIX P = parentIndex < 0
					                      ? DirectX::XMMatrixIdentity()
					                      : DirectX::XMLoadFloat4x4(&keyFrame.nodes.at(parentIndex).globalTransform);
				XMStoreFloat4x4(&keyData.globalTransform, S * R * T * P);
			}
		}
		else
		{
			blendAnimationIndex = -1;
		}
	}
}

void AnimatedObject::UpdateBlendAnimation()
{
	// 再生中でないなら更新しない
	if (!isPlayAnimation) return;

	// アニメーションクリップを持たない場合処理しない
	if (model->GetModelResource()->GetAnimationClips().empty()) return;

	// 最終フレームなら更新しない
	if (animationEndFlag) return;

	// アニメーションデータ取得
	ModelResource::Animation& animation = model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex);

	currentKeyFrame = static_cast<int>(currentAnimationSeconds * animation.samplingRate);

	// 経過時間
	currentAnimationSeconds += Timer::Instance().DeltaTime() * animationTimeScale;
	// 現在のフレームが最大フレームを超えていたら
	if (currentAnimationSeconds >= animation.secondsLength)
	{
		if (animationLoopFlag)
		{
			// 先頭秒数へ
			currentKeyFrame = 0;
			//currentAnimationSeconds -= animation.secondsLength;
			currentAnimationSeconds = 0;
		}
		else
		{
			// 最終秒数で停止
			currentKeyFrame = animation.sequence.size() - 1;
			currentAnimationSeconds = animation.secondsLength;
			animationEndFlag = true;
		}
	}
}

// ダメージフラグのクリア
void AnimatedObject::ClearAnimSphereCollisionDamagedFlag()
{
	for (auto& playerAnimSphereCollision : model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex).animSphereCollisions)
	{
		playerAnimSphereCollision.isDamaged = false;
	}
}

// 効果音更新
void AnimatedObject::UpdateSe()
{
	int seCount = model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex).animSes.size();

	for (int i = 0; i < seCount; i++)
	{
		// 既に再生されているなら再生しない
		if (model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex).animSes.at(i).isPlay) continue;

		int startFrame = model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex).animSes.at(i).startFrame;
		int endFrame = model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex).animSes.at(i).endFrame;

		//if (currentFrameInt < startFrame || currentFrameInt > endFrame) continue;
		if (currentKeyFrame != startFrame) continue;

		model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex).animSes.at(i).isPlay = true;

		MUSIC_LABEL type = model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex).animSes.at(i).musicType;

		AudioManager::Instance().PlayMusic(static_cast<int>(type));
	}
}

// 効果音フラグのクリア
void AnimatedObject::ClearSeFlag()
{
	int seCount = model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex).animSes.size();

	for (int i = 0; i < seCount; i++)
	{
		model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex).animSes.at(i).isPlay = false;
	}
}


DirectX::XMFLOAT3 AnimatedObject::GetBonePosition(std::string boneName)
{
	for(auto& mesh : model->GetModelResource()->GetMeshes())
	{
		for(auto& bone : mesh.skeleton.bones)
		{
			if(bone.name == boneName)
			{
				ModelResource::Animation& animationClip = model->GetModelResource()->GetAnimationClips().
					at(currentAnimationIndex);

				ModelResource::KeyFrame& keyframe = animationClip.sequence.at(currentKeyFrame);

				int nodeCount = keyframe.nodes.size();
				for (int nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
				{
					ModelResource::KeyFrame::Node& node = keyframe.nodes.at(nodeIndex);

					if (bone.name == node.name)
					{
						DirectX::XMMATRIX T = DirectX::XMLoadFloat4x4(&transform);
						DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&node.globalTransform);

						DirectX::XMMATRIX W = M * T;
						DirectX::XMFLOAT4X4 w;
						DirectX::XMStoreFloat4x4(&w, W);

						return { w._41, w._42, w._43 };
					}
				}
			}
		}
	}

	// 一致するボーンがない場合 0,0,0 を返す
	return { 0,0,0 };
}

// アニメーションブレンド
void AnimatedObject::BlendAnimation(const ModelResource::KeyFrame* keyFrames[2], float factor,
                                    ModelResource::KeyFrame&       keyFrame)
{
	// 出力対象の keyFrame の node を合成する keyFrame の node のサイズにする
	size_t nodeCount = keyFrames[0]->nodes.size();
	keyFrame.nodes.resize(nodeCount);
	// 全ての node を処理する
	for (size_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
	{
		// scale の合成
		DirectX::XMVECTOR S[2] =
		{
			XMLoadFloat3(&keyFrames[0]->nodes.at(nodeIndex).scaling),
			XMLoadFloat3(&keyFrames[1]->nodes.at(nodeIndex).scaling)
		};
		XMStoreFloat3(&keyFrame.nodes.at(nodeIndex).scaling, DirectX::XMVectorLerp(S[0], S[1], factor));

		// rotation の合成
		DirectX::XMVECTOR R[2] =
		{
			XMLoadFloat4(&keyFrames[0]->nodes.at(nodeIndex).rotation),
			XMLoadFloat4(&keyFrames[1]->nodes.at(nodeIndex).rotation)
		};
		XMStoreFloat4(&keyFrame.nodes.at(nodeIndex).rotation, DirectX::XMQuaternionSlerp(R[0], R[1], factor));

		// translation の合成
		DirectX::XMVECTOR T[2] =
		{
			XMLoadFloat3(&keyFrames[0]->nodes.at(nodeIndex).translation),
			XMLoadFloat3(&keyFrames[1]->nodes.at(nodeIndex).translation)
		};
		XMStoreFloat3(&keyFrame.nodes.at(nodeIndex).translation, DirectX::XMVectorLerp(T[0], T[1], factor));
	}
}

// --- Library ---
#include "../../Library/Timer.h"
// --- Game ---
#include "AnimatedObject.h"

AnimatedObject::AnimatedObject(const char* filePath)
{
	model = std::make_unique<AnimatedModel>(filePath);
}

void AnimatedObject::UpdateTransform()
{
	// スケール変更
	DirectX::XMMATRIX MS = DirectX::XMMatrixScaling(model->GetModelResource()->GetScale(), model->GetModelResource()->GetScale(), model->GetModelResource()->GetScale());

	// 軸変換行列
	DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&model->GetModelResource()->GetCoordinateSystemTransform());

	// スケール行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(GetScaleX(), GetScaleY(), GetScaleZ());
	// 回転行列を作成
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(GetAngleX()), DirectX::XMConvertToRadians(GetAngleY()), DirectX::XMConvertToRadians(GetAngleZ()));
	// 位置行列を作成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(GetPosX(), GetPosY(), GetPosZ());

	// ４つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX W = MS * S * R * T * C;
	DirectX::XMStoreFloat4x4(&transform, W);
}

void AnimatedObject::PlayAnimation(int index, bool loop)
{
	currentAnimationIndex = index;
	currentAnimationSeconds = 0.0f;

	animationLoopFlag = loop;
	animationEndFlag = false;
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
	currentAnimationSeconds += Timer::Instance().DeltaTime();
	// 現在のフレームが最大フレームを超えていたら
	if (currentAnimationSeconds >= animation.secondsLength)
	{
		if (animationLoopFlag)
		{
			// 先頭秒数へ
			currentKeyFrame = 0;
			currentAnimationSeconds -= animation.secondsLength;
		}
		else
		{
			// 最終秒数で停止
			currentKeyFrame = animation.sequence.size() - 1;
			currentAnimationSeconds = animation.secondsLength;
			animationEndFlag = true;
		}
	}

	// keyFrame設定
	keyFrame = animation.sequence.at(currentKeyFrame);


	// ダブルアニメーションが有効なら上半身のみ別キーにする
	if(isDoubleAnimations)
	{
		// ダブルアニメーションデータ取得
		ModelResource::Animation& doubleAnimation = model->GetModelResource()->GetAnimationClips().at(doubleCurrentAnimationIndex);

		doubleCurrentKeyFrame = static_cast<int>(doubleCurrentAnimationSeconds * animation.samplingRate);

		// 経過時間
		doubleCurrentAnimationSeconds += Timer::Instance().DeltaTime();

		// 現在のフレームが最大フレームを超えていたら
		if (doubleCurrentAnimationSeconds >= doubleAnimation.secondsLength)
		{
			// ダブルアニメーションをOFF
			isDoubleAnimations = false;
			doubleCurrentKeyFrame = 0;
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
			DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(keyData.translation.x, keyData.translation.y, keyData.translation.z);

			// 親行列があれば親の行列をかける
			uint32_t uniqueIndex = model->GetModelResource()->GetSceneView().GetIndex(keyData.uniqueId);			// このノードのuniqueId 取得
			int64_t parentIndex = model->GetModelResource()->GetSceneView().nodes.at(uniqueIndex).parentIndex;		// このuniqueId の parent の親の index を取得

			// 乗算すべき親行列がどちらのアニメーションかを調べる
			bool doubleAnimationParent = false;
			for (auto checkIndex : doubleAnimationNodeIndex)
			{
				if(parentIndex == checkIndex)
				{
					doubleAnimationParent = true;						// 親はダブルアニメーションの行列を使用
					doubleAnimationNodeIndex.push_back(uniqueIndex);	// このノードの子のために追加
					break;
				}
			}

			DirectX::XMMATRIX P;

			// ここの名前は変えれるようにしたほうがよさそう
			if (keyData.name == "mixamorig:Spine")
			{
				doubleAnimationNodeIndex.push_back(uniqueIndex);
				doubleAnimationParent = true;						// 親はダブルアニメーションの行列を使用(こっちは場合によってはやめたほうがいいかも)
			}

			if(doubleAnimationParent)
			{
				P = parentIndex < 0 ? DirectX::XMMatrixIdentity() : DirectX::XMLoadFloat4x4(&keyFrame.nodes.at(parentIndex).globalTransform);
				XMStoreFloat4x4(&keyData.globalTransform, S * R * T * P);
				keyFrame.nodes.at(nodeIndex).globalTransform = keyData.globalTransform;
			}
		}
	}
}

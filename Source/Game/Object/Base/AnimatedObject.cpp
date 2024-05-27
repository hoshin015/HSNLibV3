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


	//int keyCount = animation.sequence.size();
	//for (int keyIndex = 0; keyIndex < keyCount - 1; keyIndex++)
	//{
	//	const ModelResource::KeyFrame& keyFrame0 = animation.sequence.at(keyIndex);
	//	const ModelResource::KeyFrame& keyFrame1 = animation.sequence.at(keyIndex+1);
	//	if(currentAnimationSeconds >= keyFrame0.)
	//}

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

	//node の数だけ繰り返す
	//size_t nodeCount = keyFrame.animKeyData.size();
	//for (size_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
	//{
	//	// 現在の index の node を取得
	//	ModelResource::AnimKeyData& keyData = keyFrame.animKeyData.at(nodeIndex);
	//
	//	// それぞれの値に対応する行列の作成
	//	XMMATRIX S = XMMatrixScaling(keyData.scaling.x, keyData.scaling.y, keyData.scaling.z);
	//	XMMATRIX R = XMMatrixRotationQuaternion(XMLoadFloat4(&keyData.rotation));
	//	XMMATRIX T = XMMatrixTranslation(keyData.translation.x, keyData.translation.y, keyData.translation.z);
	//
	//	// 親行列があれば親の行列をかける
	//	int64_t parentIndex = modelResource->GetNodeGroup().nodeData.at(nodeIndex).parentIndex;
	//	XMMATRIX P = parentIndex < 0 ? XMMatrixIdentity() : XMLoadFloat4x4(&keyFrame.animKeyData.at(parentIndex).globalTransform);
	//
	//	// globalTransform に値を設定
	//	XMStoreFloat4x4(&keyData.globalTransform, S * R * T * P);
	//}
}

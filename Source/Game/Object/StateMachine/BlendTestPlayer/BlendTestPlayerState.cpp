#include "BlendTestPlayerState.h"
#include "BlendTestPlayer.h"
#include "../../../../../Library/Math/OperatorXMFloat3.h"
#include "../../../../../Library/Input/InputManager.h"

///////////////////////////
///
///		１層目
///
///////////////////////////


void BlendTestPlayerNormalState::Enter()
{
	SetSubState(static_cast<int>(BlendTestPlayer::Normal::Idle));
}


void BlendTestPlayerNormalState::Execute()
{
	currentSubState->Execute();
}


void BlendTestPlayerNormalState::Exit()
{
}


///////////////////////////
///
///		２層目
///
///////////////////////////


// ========================================================
// 	BlendTestPlayerTestState
// ========================================================

void BlendTestPlayerTestState::Enter()
{
	owner->SetVelocity({ 0,0,0 });

	owner->SetBeforeAnimationIndex(owner->GetCurrentAnimationIndex());
	owner->SetAnimationBlendTimer(0.0f);
	owner->SetAnimationBlendTransitionDurations(0.2f);
	owner->PlayAnimation(static_cast<int>(BlendTestPlayerAnimNum::Walk), true);

	owner->SetAngleY(180);
}

void BlendTestPlayerTestState::Execute()
{
	// 走り移動量計算
	owner->CalcVelocity();

	if (std::fabs(owner->GetVelocity().z) <= owner->GetWalkThreshold())
	{

		float blendRate = std::fabs(owner->GetVelocity().z) / owner->GetWalkThreshold();

		ModelResource::Animation& animIdle = owner->GetModel()->GetModelResource()->GetAnimationClips().at(static_cast<int>(BlendTestPlayerAnimNum::Idle));
		ModelResource::Animation& animWalk = owner->GetModel()->GetModelResource()->GetAnimationClips().at(static_cast<int>(BlendTestPlayerAnimNum::Walk));

		float normalTime = owner->GetCurrentAnimationSeconds() / animWalk.secondsLength;

		int idleAnimIndex = static_cast<int>(normalTime * animIdle.sequence.size());
		int walkAnimIndex = static_cast<int>(normalTime * animWalk.sequence.size());

		ModelResource::KeyFrame key = animIdle.sequence.at(walkAnimIndex);
		const ModelResource::KeyFrame* keyFrames[2] =
		{
			&animIdle.sequence.at(idleAnimIndex),	// idleのアニメーションのキーフレーム
			&animWalk.sequence.at(walkAnimIndex),  // walkのアニメーションのキーフレーム
		};
		owner->BlendAnimation(keyFrames, blendRate, key);

		size_t nodeCount = key.nodes.size();
		for (size_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
		{
			// 現在の index の node を取得
			ModelResource::KeyFrame::Node& keyData = key.nodes.at(nodeIndex);

			// それぞれの値に対応する行列の作成
			DirectX::XMMATRIX S = DirectX::XMMatrixScaling(keyData.scaling.x, keyData.scaling.y, keyData.scaling.z);
			DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(XMLoadFloat4(&keyData.rotation));
			DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(keyData.translation.x, keyData.translation.y,
				keyData.translation.z);

			// 親行列があれば親の行列をかける
			uint32_t uniqueIndex = owner->GetModel()->GetModelResource()->GetSceneView().GetIndex(keyData.uniqueId);
			// このノードのuniqueId 取得
			int64_t parentIndex = owner->GetModel()->GetModelResource()->GetSceneView().nodes.at(uniqueIndex).parentIndex;
			// このuniqueId の parent の親の index を取得

			DirectX::XMMATRIX P = parentIndex < 0
				? DirectX::XMMatrixIdentity()
				: DirectX::XMLoadFloat4x4(&key.nodes.at(parentIndex).globalTransform);
			XMStoreFloat4x4(&keyData.globalTransform, S * R * T * P);
		}

		owner->SetKeyFrame(key);
	}
	else if (std::fabs(owner->GetVelocity().z) <= owner->GetRunThreshold())
	{

		float blendRate = (std::fabs(owner->GetVelocity().z) - owner->GetWalkThreshold()) / (owner->GetRunThreshold() - owner->GetWalkThreshold());

		ModelResource::Animation& animWalk = owner->GetModel()->GetModelResource()->GetAnimationClips().at(static_cast<int>(BlendTestPlayerAnimNum::Walk));
		ModelResource::Animation& animRun = owner->GetModel()->GetModelResource()->GetAnimationClips().at(static_cast<int>(BlendTestPlayerAnimNum::Run));

		float normalTime = owner->GetCurrentAnimationSeconds() / animWalk.secondsLength;

		int walkAnimIndex = static_cast<int>(normalTime * animWalk.sequence.size());
		int runAnimIndex = static_cast<int>(normalTime * animRun.sequence.size());

		ModelResource::KeyFrame key = animWalk.sequence.at(walkAnimIndex);
		const ModelResource::KeyFrame* keyFrames[2] =
		{
			&animWalk.sequence.at(walkAnimIndex),  // walkのアニメーションのキーフレーム
			&animRun.sequence.at(runAnimIndex),	// runのアニメーションのキーフレーム
		};
		owner->BlendAnimation(keyFrames, blendRate, key);

		size_t nodeCount = key.nodes.size();
		for (size_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
		{
			// 現在の index の node を取得
			ModelResource::KeyFrame::Node& keyData = key.nodes.at(nodeIndex);

			// それぞれの値に対応する行列の作成
			DirectX::XMMATRIX S = DirectX::XMMatrixScaling(keyData.scaling.x, keyData.scaling.y, keyData.scaling.z);
			DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(XMLoadFloat4(&keyData.rotation));
			DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(keyData.translation.x, keyData.translation.y,
				keyData.translation.z);

			// 親行列があれば親の行列をかける
			uint32_t uniqueIndex = owner->GetModel()->GetModelResource()->GetSceneView().GetIndex(keyData.uniqueId);
			// このノードのuniqueId 取得
			int64_t parentIndex = owner->GetModel()->GetModelResource()->GetSceneView().nodes.at(uniqueIndex).parentIndex;
			// このuniqueId の parent の親の index を取得

			DirectX::XMMATRIX P = parentIndex < 0
				? DirectX::XMMatrixIdentity()
				: DirectX::XMLoadFloat4x4(&key.nodes.at(parentIndex).globalTransform);
			XMStoreFloat4x4(&keyData.globalTransform, S * R * T * P);
		}

		owner->SetKeyFrame(key);
	}


	// 回転
	//owner->Turn();

	// 移動
	owner->Move();
}

void BlendTestPlayerTestState::Exit()
{
}
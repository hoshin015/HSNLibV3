#include "Enemy.h"

#include "../../External/ImGui/imgui.h"

#include "../../Library/3D/DebugPrimitive.h"
#include "../../Library/3D/CameraManager.h"

#include "../../Library/Timer.h"
#include "../../Library/Math/Math.h"
#include "../../Library/Math/Collision.h"

#include "../../Library/Input/InputManager.h"

#include "../../Effect/Rock/RockEffect.h"
#include "../../Effect/Breath/BreathEffect.h"
#include "../../Library/Particle/EmitterManager.h"

#include "EnemyAwakeBehavior.h"

#include "../../../../Camera/CameraDerived.h"

#include "../Player/Player.h"

#include "../../../../UserInterface/UiClearAfter.h"


Enemy::~Enemy()
{
}


const Vector3 Enemy::GetFrontVec()
{
	Matrix R;
	R.MakeRotationFromQuaternion(quaternion_);
	return R.v_[2].xyz();
}


void Enemy::Initialize()
{
	// --- ビヘイビアツリーの初期化   子ノードの追加やアクションノードの設定など ---
	InitializeBehaviorTree();

	position = { 0, 0, 0 };
	velocity = { 0.0f, 0.0f, 0.0f };
	moveVec = { 0.0f, 0.0f, 0.0f };

	foundPlayer = false;

	float theta = DirectX::XMConvertToRadians(rand() % 360);
	moveTargetPosition_ = { cosf(theta) * wanderRange, 0.0f, sinf(theta) * wanderRange };
	targetVec = Vector3::Zero_;
	turnAngle = 0.0f;


	// --- ステータスの設定 ---
	maxHP = 1000.0f;
	hp = maxHP;
	flinchValue = 100.0f;

	alive = true;
	awake = false;
	endRushingBite = false;
	wasAttacked = false;

	actionCount = 0;
	roarNeededActionCount = 25;
	hissatuCount = 0;

	// Animator
	if (animator.GetCurrentState()) return;
	enum class Motion : int {
		ASIDON,
		BURESU,
		DOWN,
		DOWN_TYU,
		HISSATU_1,
		HISSATU_2,
		HISSATU_3,
		HOERU,
		HOERU_BIG,
		IDLE,
		KAITEN,
		KAMITUKI_1,
		KAMITUKI_2,
		OKIRU,
		SINU,
		SUKUIAGE,
		TAKKURU,
		TAKKURU_RIGHT,
		TOKOTOKO_LEFT,
		TOKOTOKO_RIGHT,
		TOSSIN,
		WALK_LEFT,
		WALK_MAE,
		WALK_RIGHT,
		WALK_USIRO,
		SIZE
	};

	constexpr auto IntCast = [](auto v) ->int {return static_cast<int>(v); };

	std::vector<ModelResource::Animation>& animations = GetModel()->GetModelResource()->GetAnimationClips();

	std::vector<Animator::Motion> motions;
	int i = 0;
	for (auto&& animation: animations) {
		Animator::Motion motion;
		motion.motion = &animation;
		motion.animationIndex = i;
		motion.loop = false;
		motions.emplace_back(motion);

		i++;
	}

	motions[IntCast(Motion::IDLE)].loop = true;
	motions[IntCast(Motion::WALK_MAE)].loop = true;
	motions[IntCast(Motion::WALK_USIRO)].loop = true;
	motions[IntCast(Motion::WALK_LEFT)].loop = true;
	motions[IntCast(Motion::WALK_RIGHT)].loop = true;
	motions[IntCast(Motion::DOWN_TYU)].loop = true;
	motions[IntCast(Motion::HISSATU_2)].loop = true;
	motions[IntCast(Motion::TOKOTOKO_LEFT)].loop = true;
	motions[IntCast(Motion::TOKOTOKO_RIGHT)].loop = true;
	motions[IntCast(Motion::HISSATU_2)].loop = true;

	motions[IntCast(Motion::IDLE)].threshold = {0,0};
	motions[IntCast(Motion::WALK_MAE)].threshold = {0,1};
	motions[IntCast(Motion::WALK_USIRO)].threshold = {0,-1};
	motions[IntCast(Motion::WALK_LEFT)].threshold = {-1,0};
	motions[IntCast(Motion::WALK_RIGHT)].threshold = {1,0};

	Animator::BlendTree walkTree;
	walkTree.motions.emplace_back(motions[IntCast(Motion::IDLE)]);
	walkTree.motions.emplace_back(motions[IntCast(Motion::WALK_MAE)]);
	walkTree.motions.emplace_back(motions[IntCast(Motion::WALK_USIRO)]);
	walkTree.motions.emplace_back(motions[IntCast(Motion::WALK_LEFT)]);
	walkTree.motions.emplace_back(motions[IntCast(Motion::WALK_RIGHT)]);
	walkTree.parameters[0] = "moveX";
	walkTree.parameters[1] = "moveY";
	walkTree.maxSeconds = 1.833333f;

	Animator::State walkState;
	walkState.object = Animator::MakeObjPointer(walkTree);
	walkState.type = Animator::State::BLEND_TREE;
	//walkState.transitions;

	std::vector<Animator::State> states;
	states.resize(IntCast(Motion::SIZE));
	i = 0;
	for (auto&& state: states) {
		state.object = Animator::MakeObjPointer(motions[i]);
		state.type = Animator::State::MOTION;
		state.transitionTime = 0.4f;
		i++;
	}

	animator.AddState("asidon", states[IntCast(Motion::ASIDON)]);
	animator.AddState("buresu", states[IntCast(Motion::BURESU)]);
	animator.AddState("down", states[IntCast(Motion::DOWN)]);
	animator.AddState("down_tyu", states[IntCast(Motion::DOWN_TYU)]);
	animator.AddState("hissatu_1", states[IntCast(Motion::HISSATU_1)]);
	animator.AddState("hissatu_2", states[IntCast(Motion::HISSATU_2)]);
	animator.AddState("hissatu_3", states[IntCast(Motion::HISSATU_3)]);
	animator.AddState("hoeru", states[IntCast(Motion::HOERU)]);
	animator.AddState("hoeru_big", states[IntCast(Motion::HOERU_BIG)]);
	animator.AddState("idle", states[IntCast(Motion::IDLE)]);
	animator.AddState("kaiten", states[IntCast(Motion::KAITEN)]);
	animator.AddState("kamituki_1", states[IntCast(Motion::KAMITUKI_1)]);
	animator.AddState("kamituki_2", states[IntCast(Motion::KAMITUKI_2)]);
	animator.AddState("okiru", states[IntCast(Motion::OKIRU)]);
	animator.AddState("sinu", states[IntCast(Motion::SINU)]);
	animator.AddState("sukuiage", states[IntCast(Motion::SUKUIAGE)]);
	animator.AddState("takkuru_left", states[IntCast(Motion::TAKKURU)]);
	animator.AddState("takkuru_right", states[IntCast(Motion::TAKKURU_RIGHT)]);
	animator.AddState("tokotoko_left", states[IntCast(Motion::TOKOTOKO_LEFT)]);
	animator.AddState("tokotoko_right", states[IntCast(Motion::TOKOTOKO_RIGHT)]);
	animator.AddState("tossin", states[IntCast(Motion::TOSSIN)]);
	animator.AddState("walk_left", states[IntCast(Motion::WALK_LEFT)]);
	animator.AddState("walk_mae", states[IntCast(Motion::WALK_MAE)]);
	animator.AddState("walk_right", states[IntCast(Motion::WALK_RIGHT)]);
	animator.AddState("walk_usiro", states[IntCast(Motion::WALK_USIRO)]);
	//animator.AddState("walk", walkState);
	animator.SetEntryState("idle");
	animator.SetModelSceneView(&GetModel()->GetModelResource()->GetSceneView());
	animator.EnableRootMotion("joint1");

	//PlayAnimation(static_cast<int>(MonsterAnimation::WALK_FOWARD), true);
}

void Enemy::Finalize()
{
	FinalizeBehaviorTree();
}


void Enemy::Update()
{
	float elapsedTime = Timer::Instance().DeltaTime();

	// --- ビヘイビアツリーの更新 ---
	UpdateBehaviorTree(elapsedTime);

	// --- 移動処理 ---
	UpdateMove(elapsedTime);

	// アニメーション更新
	//UpdateAnimation();

	animatorKeyFrame = animator.PlayAnimation(elapsedTime);
	currentKeyFrame = animator.GetKeyFrameIndex();
	currentAnimationIndex = animator.GetMotionIndex();

	// --- 位置の制限 ---
	ClampPosition(75.0f);

	// 生きてるときにHPが０を下回ったら
	if (alive && hp < 0.0f)
	{
		OnDead();
	}


	// --- 姿勢行列作成 ---
	Transform();
}


void Enemy::Render(bool isShadow)
{
	model->Render(transform, &animatorKeyFrame, isShadow);
}


void Enemy::ShowNode(NodeBase<Enemy>* node, std::string nodeName)
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode(nodeName.c_str()))
	{
		for (auto& childNode : node->children)
		{
			std::string name = childNode->GetName();
			if (activeNode_)
			{
				if (name == activeNode_->GetName())
					name += u8" : 実行中";
			}

			ShowNode(childNode, name);
		}

		ImGui::TreePop();
	}
}


// --- 死亡したときに呼ばれる ---
void Enemy::OnDead()
{
	CameraManager::Instance().SetCurrentCamera("EnemyDeadCamera");

	CameraManager::Instance().clearTimer = 40.0f;
	//CameraManager::Instance().clear = true;
	UiClearAfter::Instance().OnClear();

	Player::Instance().SetCamera(CameraManager::Instance().GetCamera("PlayerCamera").get());

	alive = false;
}

// --- 攻撃されたとき ---
void Enemy::OnAttacked(const float attackPower)
{
	hp -= attackPower;

	//if (hp < 0.0f)
	//{
	//	hp = 0.0f;
	//}
}


// --- デバッグGui描画 ---
void Enemy::DrawDebugGui()
{
	ImGui::Begin(u8"ビヘイビアツリー");

	// --- 実行中のノード ---
	ImGui::Text(activeNode_ ? activeNode_->GetName().c_str() : u8"ノードなし");


	// --- ビヘイビアツリーのデバッグ ---
	auto* root = aiTree_->GetRoot();
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode(root->GetName().c_str()))
	{
		for (auto& node : root->children)
		{
			ShowNode(node, node->GetName());
		}

		ImGui::TreePop();
	}

	ImGui::End();


	ImGui::Begin(u8"敵");

	ImGui::DragFloat(u8"タイマー", &runTimer_);
	if (ImGui::TreeNode(u8"移動関連"))
	{
		ImGui::DragFloat3(u8"速度", &velocity.x);
		ImGui::DragFloat3(u8"移動ベクトル", &moveVec.x);
		ImGui::DragFloat(u8"加速度", &acceleration);
		ImGui::DragFloat(u8"摩擦", &friction);
		ImGui::DragFloat(u8"最大速度", &maxSpeed);
		ImGui::Checkbox(u8"発見", &foundPlayer);
		ImGui::DragFloat(u8"索敵距離", &searchRange_);
		ImGui::DragFloat(u8"歩く速度", &walkSpeed_, 0.01f);
		ImGui::DragFloat(u8"走る速度", &runSpeed_, 0.01f);
		ImGui::DragFloat3(u8"徘徊目標", &moveTargetPosition_.x);
		ImGui::DragFloat(u8"突進スピード", &rushSpeed, 0.01f);
		ImGui::DragFloat(u8"突進終わりスピード", &rushEndSpeed, 0.01f);
		ImGui::DragFloat(u8"突進溜め時間", &rushChargeTimer, 0.01f);
		ImGui::DragFloat(u8"突進中の時間", &whileRushTimer, 0.01f);
		ImGui::TreePop();
	}

	ImGui::Checkbox(u8"攻撃されたか", &wasAttacked);
	ImGui::Text(u8"行動回数 : %d", actionCount);
	ImGui::Text(u8"咆哮に必要な行動回数 : %d", roarNeededActionCount);
	ImGui::Text(u8"攻撃回数 : %d", attackCount);
	ImGui::DragFloat(u8"HP", &hp, 1.0f);
	ImGui::DragFloat(u8"怯み値", &flinchValue, 1.0f);
	Vector3 playerPosition = Player::Instance().GetPos();
	float playerLength = playerPosition.Length();
	ImGui::Text(u8"プレイヤーの真ん中からの距離 : %f", playerLength);
	ImGui::Separator();

	ImGui::DragFloat3(u8"スケール", &scale.x);


	ImGui::Separator();
	ImGui::DragFloat(u8"遠距離範囲", &longRange, 0.01f);

	Matrix R;
	R.MakeRotationFromQuaternion(quaternion_);
	Vector3 front = R.v_[2].xyz();
	float atan = atan2(front.z, front.x);
	float theta = DirectX::XMConvertToDegrees(atan);
	ImGui::Text(u8"角度 :%f", theta);

	Vector3 vec = playerPosition - position;
	vec.Normalize();

	float cross = (front.z * vec.x) - (front.x * vec.z);
	ImGui::Text(u8"左右判定 : %f", cross);

	float dot = front.Dot(vec);
	ImGui::Text(u8"内積 : %f", dot);

	if (ImGui::Button(u8"死亡カメラ切り替え", { 200.0f, 30.0f }))
	{
		CameraManager::Instance().SetCurrentCamera("EnemyDeadCamera");
	}

	if (ImGui::Button(u8"プレイヤー死亡カメラ切り替え", { 200.0f, 30.0f }))
	{
		CameraManager::Instance().SetCurrentCamera("PlayerDeadCamera");
	}


	ImGui::End();

	animator.AnimationEditor();
}


void Enemy::DrawDebug()
{
//	DebugPrimitive::Instance().AddCylinder(position, 3.0f, 1.0f, { 1.0f, 0.0f, 0.0f, 1.0f });
	DebugPrimitive::Instance().AddCylinder(position, searchRange_, 1.0f, { 1.0f, 0.0f, 1.0f, 1.0f });
//	DebugPrimitive::Instance().AddCylinder({}, wanderRange, 5.0f, { 0.3f, 0.3f, 1.0f, 1.0f });
//	DebugPrimitive::Instance().AddCylinder(position, shortRange, 2.0f, { 1.0f, 0.0f, 0.0f, 1.0f });
//	DebugPrimitive::Instance().AddCylinder(position, middleRange, 2.0f, { 0.0f, 1.0f, 0.0f, 1.0f });
//	DebugPrimitive::Instance().AddCylinder(position, longRange, 5.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
//	DebugPrimitive::Instance().AddSphere(moveTargetPosition_.vec_, 1.0f, { 0.3f, 0.3f, 1.0f, 1.0f });
}


void Enemy::Transform()
{
	// スケール変更
	Matrix MS;
	Vector3 modelScale = Vector3::Unit_ * model->GetModelResource()->GetScale();
	MS.MakeScaling(modelScale);

	// 軸変換行列
	Matrix C = model->GetModelResource()->GetCoordinateSystemTransform();

	// スケール行列を作成
	Matrix S;
	S.MakeScaling(Vector3(GetScale()));
	// 回転行列を作成
	Matrix R;
	R.MakeRotationFromQuaternion(quaternion_);
	// 位置行列を作成
	Matrix T;
	T.MakeTranslation(Vector3(GetPos()));

	// ４つの行列を組み合わせ、ワールド行列を作成
	Matrix W =C * MS * S * R * T;
	transform = W.mat_;
}


void Enemy::CollisionVSPlayer()
{
	// --- 敵の攻撃の球 ---
	for (auto& enemySphere : model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex).animSphereCollisions)
	{
		// --- アニメーションが範囲外だったら次へ ---
		if (currentKeyFrame < enemySphere.startFrame || currentKeyFrame > enemySphere.endFrame) continue;

		if (enemySphere.isDamaged)
			break;


		// --- プレイヤーの球 ---
		for (auto& playerSphere : Player::Instance().GetModel()->GetModelResource()->GetSkeletonSphereCollisions())
		{
			// --- 敵の球の座標を取得 ---
			Vector3 enemyBonePosition = enemySphere.position;
			enemyBonePosition = (enemySphere.bindBoneName == "") ? enemyBonePosition + position : GetBonePosition(enemySphere.bindBoneName);

			// --- プレイヤーの球の座標を取得 ---
			Vector3 playerBonePosition = Player::Instance().GetPos();
			playerBonePosition = (playerSphere.name == "") ? playerBonePosition + playerSphere.position : Player::Instance().GetBonePosition(playerSphere.name);


			Vector3 dummy;
			if (Collision::IntersectSphereVsSphere(enemyBonePosition.vec_, enemySphere.radius, playerBonePosition.vec_, playerSphere.radius, dummy.vec_))
			{
				EnableAnimSphereCollisionDamagedFlag();

				// --- ここに当たった時の処理を書く ---
				CameraManager::Instance().shakeTimer = 1.0f;
				CameraManager::Instance().shakePower = 100.0f;
				attackCount++;

				// ヒットストップ処理
				Player::Instance().hitStopTimer = 0.05f;
				Timer::Instance().SetTimeScale(0.0f);

				// TODO::Playerダメージ関係
				Player& player = Player::Instance();
				//float currentHP = player.AStatus().hp;
				bool frying = false;

				Vector3 pPos = player.GetPos();
				Vector3 ePos = position;
				Vector3 fryVec = pPos - ePos;
				fryVec.y = 0;
				fryVec.Normalize();
				fryVec *= 10;
				const Animator::State* current = animator.GetCurrentState();

				if (current == &animator.GetState("asidon")) frying = true;

				player.HitDamaged(attackPower,false,frying,fryVec);

				// --- この攻撃でプレイヤーが死亡したとき ---
				// if (player.AStatus().hp <= 0.0f && currentHP > 0.0f)
				// {
				// 	CameraManager::Instance().SetCurrentCamera("PlayerDeadCamera");
				// }

				break;
			}
		}
	}
}


// --- 移動処理 ---
void Enemy::UpdateMove(float elapsedTime)
{
	float elapsedFrame = (60.0f / (1.0f / elapsedTime));

	// --- 水平速力処理 ---
	UpdateHorizontalVelocity(elapsedTime, elapsedFrame);

	// --- 水平移動処理 ---
	UpdateHorizontalMove(elapsedTime, elapsedFrame);
}


// --- 水平速力処理 ---
void Enemy::UpdateHorizontalVelocity(float elapsedTime, float elapsedFrame)
{
	Vector2 velocity = this->velocity.xz();
	float length = velocity.Length();

	// --- 速度があれば減速 ---
	if (length > 0.0f)
	{
		// --- 抵抗力 ---
		float friction = this->friction * elapsedFrame;
		if (length > friction)
		{
			velocity.Normalize();

			// --- 減速処理 ---
			this->velocity.x -= velocity.x * friction;
			this->velocity.z -= velocity.y * friction;
		}

		else
		{
			this->velocity.x = 0.0f;
			this->velocity.z = 0.0f;
		}
	}


	// --- 最大速度を越えていないなら加速 ---
	if (length <= maxSpeed)
	{
		Vector2 moveVec = this->moveVec.xz();
		float moveVecLength = moveVec.Length();
		if (moveVecLength > 0.0f)
		{
			// --- 加速度 ---
			float acceleration = this->acceleration * elapsedFrame;

			// --- 移動ベクトル方向に加速 ---
			this->velocity.x += moveVec.x * acceleration;
			this->velocity.z += moveVec.y * acceleration;
		}


		// --- 最大速度制限 ---
		Vector2 updatedVelocity = this->velocity.xz();
		float updatedLength = updatedVelocity.Length();

		// --- 最大速度を越えていたら制限 ---
		if (updatedLength > maxSpeed)
		{
			this->velocity.x = (this->velocity.x / updatedLength) * maxSpeed;
			this->velocity.z = (this->velocity.z / updatedLength) * maxSpeed;
		}
	}


	moveVec.x = moveVec.z = 0.0f;
}


// --- 水平移動処理 ---
void Enemy::UpdateHorizontalMove(float elapsedTime, float elapsedFrame)
{
	Vector2 vec = velocity.xz();
	float length = vec.Length();

	// --- 移動量があれば ---
	if (length > 0.0f)
	{
		Vector3 updatedPosition;
		Vector3 position = this->position;
		updatedPosition = position + velocity * elapsedTime;

		this->position = updatedPosition.vec_;
	}
}


// --- 移動 ---
void Enemy::Move(const Vector3& vec, const float speed)
{
	moveVec = vec;
	maxSpeed = speed;
}


// --- プレイヤーを見つけたか ---
bool Enemy::SearchPlayer()
{
	// --- プレイヤーとの距離の計算 ---
	Vector3 playerPosition = Player::Instance().GetPos();
	Vector3 toPlayerVec = playerPosition - position;
	float length = toPlayerVec.Length();

	// --- 中央から離れていたら ---
	float playerLength = playerPosition.Length();
	if (playerLength > 81.0f)
		return false;

	// --- 距離が探知範囲内なら ---
	if (length < searchRange_)
	{
		// --- 正面ベクトルを取得 ---
		Matrix R;
		R.MakeRotationFromQuaternion(quaternion_);
		Vector3 front = R.v_[2].xyz();
		Vector3 vec = Vector3::Normalize(toPlayerVec);
		float dot = front.Dot(vec);

		if (dot > 0.0f)
		{
			return true;
		}
	}

	// --- 見つからなかった ---
	return false;
}


// --- 目標のベクトルへ回転 ---
void Enemy::RotateToTargetVec(const DirectX::XMFLOAT3& targetVec, float t, const Vector3* tempFront)
{
	Matrix R;
	R.MakeRotationFromQuaternion(quaternion_);
	Vector3 front = R.v_[2].xyz();	// 正面を取得

	if (tempFront)
		front = *tempFront;

	Vector3 cross = front.Cross(targetVec);

	float dot = front.Dot(targetVec);
	if (dot != 1.0f)
	{
		float angle = acosf(dot);

		Quaternion rot;
		rot.SetRotation(cross, angle);
		if (angle > 1e-8f)
		{
			Quaternion dst = rot * quaternion_;
			quaternion_ = quaternion_.Lerp(quaternion_, dst, t);
		}

		else
		{
			Quaternion dst = rot * quaternion_;
			quaternion_ = quaternion_.Lerp(quaternion_, dst, 1.0f);
		}
	}
}


// --- 位置の制限 ---
void Enemy::ClampPosition(float range)
{
	Vector3 pos = position;
	float length = pos.Length();
	if (length > range)
	{
		pos.Normalize();
		pos *= range;
		position = pos.vec_;
	}
}

// --- プレイヤーが死んだときに呼ぶ ---
void Enemy::OnPlayerDead()
{
	foundPlayer = false;
	wasAttacked = false;
}


// --- 岩エフェクト ---
void Enemy::PlayRockEffect()
{
	for (int i = 0; i < 10; i++)
	{
		DirectX::XMFLOAT3 rPos = { (rand() % 10) - 5.0f, 0, rand() % 10 - 5.0f };
		DirectX::XMFLOAT3 rVec = { (rand() % 10) - 5.0f, 5, rand() % 10 - 5.0f };
		Vector3 position = GetPos();
		position += rPos;
		RockEffect::Instance().Emit(position.vec_, rVec, { 0.1, 1 });
	}
}


// --- 炎ブレス ---
void Enemy::PlayFireBress(float angle)
{
	BreathEffect::Instance().SetPosition(Enemy::Instance().GetBonePosition("sitaago"));
	BreathEffect::Instance().SetAngle(angle);
	BreathEffect::Instance().Emit();
}



enum
{
	FIND,
	DEAD,
	FLINCH,
	AWAKE,
	END_RUSHING_BITE,
	BIG_ROAR,
	BATTLE,
	SCOUT,
};



// --- ビヘイビアツリーの初期化 ---
void Enemy::InitializeBehaviorTree()
{
	behaviorData_ = new BehaviorData<Enemy>();
	aiTree_ = new BehaviorTree<Enemy>();


	// --- 大元のノード ---
	aiTree_->AddNode("", "Root", 0, BT_SelectRule::Priority, nullptr, nullptr);


	// --- 最初の発見時 ---
	aiTree_->AddNode("Root", "Find", FIND, BT_SelectRule::Sequence, new EnemyBattleInitJudgment(this), nullptr);
	{
		aiTree_->AddNode("Find", "AxisAlignment", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
		aiTree_->AddNode("Find", "BigRoar", 0, BT_SelectRule::Non, nullptr, new EnemyBigRoarAction(this));
	}

	// --- 死亡 ---
	aiTree_->AddNode("Root", "Dead", DEAD, BT_SelectRule::Non, new EnemyDeadJudgment(this), new EnemyDeadAction(this));

	// --- 怯み ---
	aiTree_->AddNode("Root", "Flinch", FLINCH, BT_SelectRule::Non, new EnemyFlinchJudgment(this), new EnemyFlinchAction(this));


	// --- 覚醒移行 ---
	aiTree_->AddNode("Root", "Awake", AWAKE, BT_SelectRule::Sequence, new EnemyAwakeJudgment(this), nullptr);
	{
		aiTree_->AddNode("Awake", "BigRoar", 0, BT_SelectRule::Non, nullptr, new EnemyBigRoarAction(this));
		aiTree_->AddNode("Awake", "Move", 0, BT_SelectRule::Non, nullptr, new EnemyMoveCenterAction(this));
		aiTree_->AddNode("Awake", "Turn", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
		aiTree_->AddNode("Awake", "Awake", 0, BT_SelectRule::Non, nullptr, new EnemyDeathBlowAction(this));
	}


	// --- 踏み込み噛みつき終わり行動 ---
	aiTree_->AddNode("Root", "EndRushingBite", END_RUSHING_BITE, BT_SelectRule::Priority, new EnemyEndRushingBiteJudgment(this), nullptr);
	{
		// --- 足元にいたら踏みつけ ---
		aiTree_->AddNode("EndRushingBite", "Stamp", 0, BT_SelectRule::Non, new EnemyFootJudgment(this), new EnemyStampAction(this));

		// --- 横にいたらタックル ---
		aiTree_->AddNode("EndRushingBite", "Tackle", 1, BT_SelectRule::Non, new EnemyRightJudgment(this), new EnemyTackleAction(this));

		// --- 後ろにいたら ---
		aiTree_->AddNode("EndRushingBite", "TailAttack", 2, BT_SelectRule::Non, new EnemyBehindJudgment(this), new EnemyTailAttack(this));
	}

	// --- 大咆哮 ---
	aiTree_->AddNode("Root", "BigRoar", BIG_ROAR, BT_SelectRule::Non, new EnemyBigRoarJudgment(this), new EnemyBigRoarAction(this));

	// --- 戦闘ノード ---
	aiTree_->AddNode("Root", "Battle", BATTLE, BT_SelectRule::Priority, new EnemyBattleJudgment(this), nullptr);
	{
		// --- 攻撃ノード ---
		aiTree_->AddNode("Battle", "Attack", 0, BT_SelectRule::Priority, nullptr, nullptr);
		{
			// --- 足元攻撃 ---
			aiTree_->AddNode("Attack", "FootAttack", 0, BT_SelectRule::Random, new EnemyFootJudgment(this), nullptr);
			{
				// --- 踏みつけ ---
				aiTree_->AddNode("FootAttack", "Stamp", 0, BT_SelectRule::Non, nullptr, new EnemyStampAction(this));

				// --- 尻尾回転 ---
				aiTree_->AddNode("FootAttack", "TailAttack", 0, BT_SelectRule::Non, nullptr, new EnemyTailAttack(this));
			}

			// --- それ以外 ---
			aiTree_->AddNode("Attack", "NormalAttack", 1, BT_SelectRule::Random, nullptr, nullptr);
			{
				// --- 遠距離 ---
				aiTree_->AddNode("NormalAttack", "LongRange", 0, BT_SelectRule::Random, new EnemyLongRangeJudgment(this)/*nullptr*/, nullptr);
				{
					// --- ブレス → 威嚇 ---
					aiTree_->AddNode("LongRange", "L_Bless_Threat", 0, BT_SelectRule::Sequence, new EnemyAwakedJudgment(this)/*nullptr*/, nullptr);
					{
						aiTree_->AddNode("L_Bless_Threat", "L_AxisAlignment", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
						aiTree_->AddNode("L_Bless_Threat", "L_Bless", 0, BT_SelectRule::Non, nullptr, new EnemyBlessAction(this));
						aiTree_->AddNode("L_Bless_Threat", "L_Threat", 0, BT_SelectRule::Non, nullptr, new EnemyThreatAction(this));
					}

					// --- 雷咆哮 ---
					aiTree_->AddNode("LongRange", "L_Turn_LightningRoar", 0, BT_SelectRule::Sequence, new EnemyAwakedJudgment(this), nullptr);
					{
						aiTree_->AddNode("L_Turn_LightningRoar", "L_Turn", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
						aiTree_->AddNode("L_Turn_LightningRoar", "L_LightningRoar", 0, BT_SelectRule::Non, nullptr, new EnemyLightningRoarAction(this));
					}

					// --- 雷大咆哮 ---
					aiTree_->AddNode("LongRange", "L_Turn_LightningBigRoar", 0, BT_SelectRule::Sequence, new EnemyAwakedJudgment(this), nullptr);
					{
						aiTree_->AddNode("L_Turn_LightningRoar", "L_Turn", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
						aiTree_->AddNode("L_Turn_LightningRoar", "L_LightningBigRoar", 0, BT_SelectRule::Non, nullptr, new EnemyLightningBigRoarAction(this));
					}


					// --- 軸合わせ → 掬い上げ ---
					aiTree_->AddNode("LongRange", "L_Turn_ScoopUp", 0, BT_SelectRule::Sequence, nullptr, nullptr);
					{
						aiTree_->AddNode("L_Turn_ScoopUp", "L_Turn", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
						aiTree_->AddNode("L_Turn_ScoopUp", "L_ScoopUp", 0, BT_SelectRule::Non, nullptr, new EnemyScoopUpAction(this));
					}


					// --- 突進 → 威嚇 ---
					aiTree_->AddNode("LongRange", "L_Rush_Threat", 0, BT_SelectRule::Sequence, nullptr, nullptr);
					{
						aiTree_->AddNode("L_Rush_Threat", "L_AxisAlignment", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
						aiTree_->AddNode("L_Rush_Threat", "L_Rush", 0, BT_SelectRule::Non, nullptr, new EnemyRushAction(this));
						aiTree_->AddNode("L_Rush_Threat", "L_Threat", 0, BT_SelectRule::Non, nullptr, new EnemyThreatAction(this));
					}

					// --- 普通に接近 ---
					aiTree_->AddNode("LongRange", "L_Turn_Pursuit", 0, BT_SelectRule::Sequence, nullptr, nullptr);
					{
						aiTree_->AddNode("L_Turn_Pursuit", "L_Turn", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
						aiTree_->AddNode("L_Turn_Pursuit", "L_Pursuit", 0, BT_SelectRule::Non, nullptr, new EnemyPursuitAction(this));
					}
				}


				// --- 中距離 ---
				aiTree_->AddNode("NormalAttack", "MiddleRange", 0, BT_SelectRule::Random, new EnemyMiddleRangeJudgment(this), nullptr);
				{
					// --- 軸合わせ → 踏み込み噛みつき ---
					aiTree_->AddNode("MiddleRange", "M_Turn_RushingBite", 0, BT_SelectRule::Sequence, nullptr, nullptr);
					{
						aiTree_->AddNode("M_Turn_RushingBite", "M_Turn", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
						aiTree_->AddNode("M_Turn_RushingBite", "M_RushingBite", 0, BT_SelectRule::Non, nullptr, new EnemyRushingBiteAction(this));
					}

					// --- 軸合わせ → 掬い上げ ---
					aiTree_->AddNode("MiddleRange", "M_Turn_ScoopUp", 0, BT_SelectRule::Sequence, nullptr, nullptr);
					{
						aiTree_->AddNode("M_Turn_ScoopUp", "M_Turn", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
						aiTree_->AddNode("M_Turn_ScoopUp", "M_ScoopUp", 0, BT_SelectRule::Non, nullptr, new EnemyScoopUpAction(this));
					}

					// --- 突進 → 威嚇 ---
					aiTree_->AddNode("MiddleRange", "M_Rush_Threat", 0, BT_SelectRule::Sequence, nullptr, nullptr);
					{
						aiTree_->AddNode("M_Rush_Threat", "M_AxisAlignment", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
						aiTree_->AddNode("M_Rush_Threat", "M_Rush", 0, BT_SelectRule::Non, nullptr, new EnemyRushAction(this));
						aiTree_->AddNode("M_Rush_Threat", "M_Threat", 0, BT_SelectRule::Non, nullptr, new EnemyThreatAction(this));
					}

					// --- タックル ---
					aiTree_->AddNode("MiddleRange", "M_Tackle", 0, BT_SelectRule::Non, new EnemyRightJudgment(this), new EnemyTackleAction(this));

					// --- 尻尾回転 ---
					aiTree_->AddNode("MiddleRange", "M_TailAttack", 0, BT_SelectRule::Non, nullptr, new EnemyTailAttack(this));
				

					// --- 雷咆哮 ---
					aiTree_->AddNode("MiddleRange", "M_Turn_LightningRoar", 0, BT_SelectRule::Sequence, new EnemyAwakedJudgment(this), nullptr);
					{
						aiTree_->AddNode("M_Turn_LightningRoar", "M_Turn", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
						aiTree_->AddNode("M_Turn_LightningRoar", "M_LightningRoar", 0, BT_SelectRule::Non, nullptr, new EnemyLightningRoarAction(this));
					}

					// --- 雷大咆哮 ---
					aiTree_->AddNode("LongRange", "M_Turn_LightningBigRoar", 0, BT_SelectRule::Sequence, new EnemyAwakedJudgment(this), nullptr);
					{
						aiTree_->AddNode("M_Turn_LightningRoar", "M_Turn", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
						aiTree_->AddNode("M_Turn_LightningRoar", "M_LightningBigRoar", 0, BT_SelectRule::Non, nullptr, new EnemyLightningBigRoarAction(this));
					}
				}


				// --- 近距離 ---
				aiTree_->AddNode("NormalAttack", "ShortRange", 0, BT_SelectRule::Random, new EnemyShortRangeJudgment(this), nullptr);
				{
					// --- 踏みつけ → 威嚇 ---
					aiTree_->AddNode("ShortRange", "S_Stamp_Threat", 0, BT_SelectRule::Sequence, nullptr, nullptr);
					{
						aiTree_->AddNode("S_Stamp_Threat", "S_Stamp", 0, BT_SelectRule::Non, nullptr, new EnemyStampAction(this));
						aiTree_->AddNode("S_Stamp_Threat", "S_Threat", 0, BT_SelectRule::Non, nullptr, new EnemyThreatAction(this));
					}

					// --- 軸合わせ → 噛みつき ---
					aiTree_->AddNode("ShortRange", "S_AxisAlignment_Bite", 0, BT_SelectRule::Sequence, nullptr, nullptr);
					{
						aiTree_->AddNode("S_AxisAlignment_Bite", "S_AxisAlignment", 0, BT_SelectRule::Non, nullptr, new EnemyAxisAlignmentAction(this));
						aiTree_->AddNode("S_AxisAlignment_Bite", "S_Bite", 0, BT_SelectRule::Non, nullptr, new EnemyBiteAction(this));
					}

					// --- 尻尾回転 ---
					aiTree_->AddNode("ShortRange", "S_TailAttack", 0, BT_SelectRule::Non, nullptr, new EnemyTailAttack(this));

					// --- タックル ---
					aiTree_->AddNode("ShortRange", "S_Tackle", 0, BT_SelectRule::Non, new EnemyRightJudgment(this), new EnemyTackleAction(this));
				}
			}
		}


		// --- 追跡ノード ---
		aiTree_->AddNode("Battle", "Pursuit", 1, BT_SelectRule::Non, nullptr, new EnemyPursuitAction(this));
	}


	// --- 未発見時ノード ---
	aiTree_->AddNode("Root", "Scout", SCOUT, BT_SelectRule::Sequence, new EnemyScoutJudgment(this), nullptr);
	{
		// --- 徘徊 ---
		aiTree_->AddNode("Scout", "Wander", 0, BT_SelectRule::Non, new EnemyWanderJudgment(this), new EnemyWanderAction(this));

		// --- 待機 ---
		aiTree_->AddNode("Scout", "Idle", 0, BT_SelectRule::Non, nullptr, new EnemyIdleAction(this));
	}
}

// --- 終了処理 ---
void Enemy::FinalizeBehaviorTree()
{
	if(behaviorData_)
	{
		delete behaviorData_;
		behaviorData_ = nullptr;
	}

	if(aiTree_)
	{
		delete aiTree_;
		aiTree_ = nullptr;
	}

	activeNode_ = nullptr;
}

void Enemy::UpdateBehaviorTree(float elapsedTime)
{
	// --- 実行中のノードがなければ ---
	if (!activeNode_)
	{
		// --- 次の実行ノードを推論 ---
		activeNode_ = aiTree_->ActiveNodeInference(behaviorData_);
	}

	// --- 実行ノードがあれば ---
	if (activeNode_)
	{
		activeNode_ = aiTree_->Run(activeNode_, behaviorData_, elapsedTime);
	}
}

void Enemy::DrawDebugImGui(int number)
{
	ImGui::Begin("Enemy");
	{
		ImGui::DragFloat("emissive", &GetModel()->data.emissivePower, 0.01f);
		ImGui::SliderFloat("roughness", &GetModel()->data.roughnessPower, -1.0f, 1.0f);
		ImGui::SliderFloat("metalness", &GetModel()->data.metalnessPower, -1.0f, 1.0f);
	}
	ImGui::End();
}
#include "Player.h"

#include "PlayerState.h"
#include "../../Library/Input/InputManager.h"
#include "../../Library/3D/Camera.h"
#include "../../Library/Timer.h"
#include "../../Library/Math/Math.h"
#include "../../Library/Math/Collision.h"
#include "../../Source/Game/Object/Stage/StageManager.h"
#include "../../ImGui/imgui.h"
#include "../Enemy/Enemy.h"
#include "../../Library/ImGui/ConsoleData.h"
#include "../../../../../Library/Particle/EmitterManager.h"
#include "../../../../../Library/Resource/Model/Animator.h"
#include "../../../../UserInterface/DamageTextManager.h"

Player::Player(const char* filePath) : AnimatedObject(filePath)
{
	stateMachine = std::make_unique<StateMachine<Player>>();
	// １層目ステート登録
	stateMachine->RegisterState(new PlayerNormalState(this));
	// ２層目ステート登録
	stateMachine->RegisterSubState(static_cast<int>(State::Normal), new PlayerIdleState(this));
	stateMachine->RegisterSubState(static_cast<int>(State::Normal), new PlayerWalkState(this));
	stateMachine->RegisterSubState(static_cast<int>(State::Normal), new PlayerRunState(this));
	stateMachine->RegisterSubState(static_cast<int>(State::Normal), new PlayerAttackState(this));
	stateMachine->RegisterSubState(static_cast<int>(State::Normal), new PlayerDodgeState(this));

	auto& animation = model->GetModelResource()->GetAnimationClips();

	Animator::Motion idle;
	idle.motion = &animation[1];
	idle.animationSpeed = 0.15f;
	idle.threshold = { 0,0 };

	Animator::Motion walkMae;
	walkMae.motion = &animation[12];
	walkMae.animationSpeed = 0.2f;
	walkMae.threshold = { 0,1 };

	Animator::Motion walkUsiro;
	walkUsiro.motion = &animation[14];
	walkUsiro.animationSpeed = 0.2f;
	walkUsiro.threshold = { 0,-1 };

	Animator::Motion walkLeft;
	walkLeft.motion = &animation[11];
	walkLeft.animationSpeed = 0.2f;
	walkLeft.threshold = { -1,0 };

	Animator::Motion walkRight;
	walkRight.motion = &animation[13];
	walkRight.animationSpeed = 0.2f;
	walkRight.threshold = { 1,0 };

	Animator::Motion runMotion;
	runMotion.motion = &animation[6];
	runMotion.animationSpeed = 0.2f;
	runMotion.threshold = { 0,1 };

	Animator::Motion attack1;
	attack1.motion = &animation[2];
	attack1.animationSpeed = 0.2f;
	attack1.threshold = { 1,0 };
	attack1.loop = false;

	Animator::Motion attack2;
	attack2.motion = &animation[3];
	attack2.animationSpeed = 0.2f;
	attack2.threshold = { 1,0 };
	attack2.loop = false;

	Animator::Motion attack3;
	attack3.motion = &animation[4];
	attack3.animationSpeed = 0.2f;
	attack3.threshold = { 1,0 };
	attack3.loop = false;

	Animator::Motion attack4;
	attack4.motion = &animation[5];
	attack4.animationSpeed = 0.2f;
	attack4.threshold = { 1,0 };
	attack4.loop = false;

	Animator::BlendTree walkTree;
	walkTree.motions.emplace_back(idle);
	walkTree.motions.emplace_back(walkMae);
	walkTree.motions.emplace_back(walkUsiro);
	walkTree.motions.emplace_back(walkLeft);
	walkTree.motions.emplace_back(walkRight);
	walkTree.maxSeconds = 1.033333f;
	walkTree.parameters[0] = "moveX";
	walkTree.parameters[1] = "moveY";

	Animator::BlendTree runTree;
	runTree.motions.emplace_back(idle);
	runTree.motions.emplace_back(runMotion);
	runTree.maxSeconds = 1.033333f;
	runTree.parameters[0] = "moveX";
	runTree.parameters[1] = "moveY";

	Animator::State walkState;
	walkState.object = Animator::MakeObjPointer(walkTree);
	walkState.type = Animator::State::BLEND_TREE;
	walkState.transitions =
		STATE_FUNC(animator) {
		if (animator.GetParameter<bool>("run"))
			return &animator.GetState("run");

		if (animator.GetParameter<bool>("attack"))
			return &animator.GetState("attack1");

		return nullptr;
		};

	Animator::State runState;
	runState.object = Animator::MakeObjPointer(runTree);
	runState.type = Animator::State::BLEND_TREE;
	runState.transitions =
		STATE_FUNC(animator) {
		if (!animator.GetParameter<bool>("run"))
			return &animator.GetState("walk");

		if (animator.GetParameter<bool>("attack"))
			return &animator.GetState("attack1");

		return nullptr;
		};

	Animator::State attack1State;
	attack1State.object = Animator::MakeObjPointer(attack1);
	attack1State.type = Animator::State::MOTION;
	attack1State.transitions = STATE_FUNC(animator) {
		if (animator.GetParameter<bool>("endAttack"))
			return &animator.GetState("walk");

		if (animator.GetParameter<bool>("attack"))
			return &animator.GetState("attack2");

		return nullptr;
	};

	Animator::State attack2State;
	attack2State.object = Animator::MakeObjPointer(attack2);
	attack2State.type = Animator::State::MOTION;
	attack2State.transitions = STATE_FUNC(animator) {
		if (animator.GetParameter<bool>("endAttack"))
			return &animator.GetState("walk");

		if (animator.GetParameter<bool>("attack"))
			return &animator.GetState("attack3");

		return nullptr;
	};

	Animator::State attack3State;
	attack3State.object = Animator::MakeObjPointer(attack3);
	attack3State.type = Animator::State::MOTION;
	attack3State.transitions = STATE_FUNC(animator) {
		if (animator.GetParameter<bool>("endAttack"))
			return &animator.GetState("walk");

		if (animator.GetParameter<bool>("attack"))
			return &animator.GetState("attack4");
		return nullptr;
	};

	Animator::State attack4State;
	attack4State.object = Animator::MakeObjPointer(attack4);
	attack4State.type = Animator::State::MOTION;
	attack4State.transitions = STATE_FUNC(animator) {
		if (animator.GetParameter<bool>("endAttack"))
			return &animator.GetState("walk");
		return nullptr;
	};

	animator.AddState("walk", walkState);
	animator.AddState("run", runState);
	animator.AddState("attack1", attack1State);
	animator.AddState("attack2", attack2State);
	animator.AddState("attack3", attack3State);
	animator.AddState("attack4", attack4State);
	animator.EnableRootMotion("root");
	animator.SetModelSceneView(&model->GetModelResource()->GetSceneView());
	animator.SetEntryState("walk");
}

void Player::Initialize()
{
	// ステートセット
	stateMachine->SetState(static_cast<int>(State::Normal));

	// idle アニメーション再生
	PlayAnimation(static_cast<int>(PlayerAnimNum::Idle), true);
}

void Player::Update()
{
	// 入力データ取得
	Input();


	// ステートマシン更新
	stateMachine->Update();

	// 敵との当たり判定
	CollisionVsEnemy();

	// アニメーション更新
	UpdateAnimationParam();

	// 姿勢行列更新
	UpdateTransform();
}

void Player::Render(bool isShadow)
{
	// Animatorを使ったモーション
	ModelResource::KeyFrame keyFrame = animator.PlayAnimation(Timer::Instance().DeltaTime());
	model->Render(transform, &keyFrame, isShadow);
}

void Player::DrawDebugImGui(int number) {

	auto DrawVector = [](ImDrawList* drawList, ImVec2 startPos, ImVec2 endPos, ImU32 color, float thickness) {
		drawList->AddLine(startPos, endPos, color, thickness);

		// 矢印の先端を描画
		float arrowSize = 10.0f;
		ImVec2 direction = ImVec2(endPos.x - startPos.x, endPos.y - startPos.y);
		float length = sqrt(direction.x * direction.x + direction.y * direction.y);
		if (length > 0.0f) {
			direction.x /= length;
			direction.y /= length;
		}
		ImVec2 left = ImVec2(-direction.y, direction.x);
		ImVec2 right = ImVec2(direction.y, -direction.x);
		ImVec2 arrowLeft = ImVec2(endPos.x - direction.x * arrowSize + left.x * arrowSize, endPos.y - direction.y * arrowSize + left.y * arrowSize);
		ImVec2 arrowRight = ImVec2(endPos.x - direction.x * arrowSize + right.x * arrowSize, endPos.y - direction.y * arrowSize + right.y * arrowSize);
		drawList->AddTriangleFilled(endPos, arrowLeft, arrowRight, color);
	};

	ImGui::Begin("Player");
	{
		ImGui::DragFloat("emissive", &GetModel()->data.emissivePower, 0.01f);
		ImGui::SliderFloat("roughness", &GetModel()->data.roughnessPower, -1.0f, 1.0f);
		ImGui::SliderFloat("metalness", &GetModel()->data.metalnessPower, -1.0f, 1.0f);

		//ImGui::ShowDemoWindow();
		if(ImGui::CollapsingHeader(u8"ステータス")) {
			if(ImGui::TreeNode(u8"変動値")) {
				ImGui::Text(u8"速度:%.5f", Vector3(velocity).Length());
				ImGui::DragFloat3(u8"回転", &angle.x, 0.1f);
				ImGui::TreePop();
			}

			if(ImGui::TreeNode(u8"能力値")) {
				ImGui::DragFloat(u8"体力", &ability.hp, 1);
				ImGui::DragFloat(u8"攻撃力", &ability.strength, 0.1f);
				ImGui::DragFloat(u8"移動速度", &ability.moveSpeed, 0.1f);

				ImGui::TreePop();
			}

			if(ImGui::TreeNode(u8"定数値")) {
				ImGui::DragFloat(u8"最大体力", &constant.maxHp);
				ImGui::DragFloat(u8"ダッシュ移行時間", &constant.shiftDashTimer,0.01f);
				ImGui::DragFloat(u8"ダッシュ倍率", &constant.dashSpeed,0.01f);

				static float rate = constant.dashDeadZone * 100;
				if(ImGui::SliderFloat(u8"ダッシュデットゾーン", &rate,0,100,"%.0f%%"))
					constant.dashDeadZone = rate / 100;

				ImGui::TreePop();
			}

		}

#ifdef _DEBUG
		if(ImGui::CollapsingHeader(u8"デバッグ")) {
			for (auto& [name, data] : debug) {
				if(const float* val = std::get_if<float>(&data)) ImGui::Text("%s: %f", name.c_str(),*val);
				if(const bool* val = std::get_if<bool>(&data)) ImGui::Text("%s: %s", name.c_str(), *val ? "true" : "false");
				if(const int* val = std::get_if<int>(&data)) ImGui::Text("%s: %d", name.c_str(),*val);
				if (const XMFLOAT2* val = std::get_if<XMFLOAT2>(&data)) {
					ImGui::Text("%s: %f,%f", name.c_str(), val->x, val->y);

					ImVec2 window_pos = ImGui::GetWindowPos();
					ImVec2 window_size = ImGui::GetWindowSize();
					ImVec2 center = ImVec2(window_pos.x + window_size.x / 2, window_pos.y + window_size.y / 2);
					DrawVector(ImGui::GetWindowDrawList(), center, ImVec2(center.x + val->x * 100,center.y + -val->y * 100), IM_COL32(0,255,0,255), 2.f);
				}
			}
		}
#endif

		if (ImGui::CollapsingHeader(u8"入力")) {
			for (auto& [name, data] : inputMap) {
				if (const float* val = std::get_if<float>(&data)) ImGui::Text("%s: %f", name.c_str(), *val);
				if (const bool* val = std::get_if<bool>(&data)) ImGui::Text("%s: %s", name.c_str(), *val ? "true" : "false");
				if (const int* val = std::get_if<int>(&data)) ImGui::Text("%s: %d", name.c_str(), *val);
				if (const XMFLOAT2* val = std::get_if<XMFLOAT2>(&data)) ImGui::Text("%s: %f,%f", name.c_str(), val->x, val->y);
			}
		}
	}
	ImGui::End();

	animator.AnimationEditor();
}

void Player::UpdateAnimationParam() {
	Vector2 move = GetInputMap<XMFLOAT2>("Move");
	//velocity.
	debug[u8"方向"] = move.vec_;
	XMStoreFloat2(&move.vec_,XMVector2TransformCoord(XMLoadFloat2(&move.vec_),XMMatrixRotationZ(XMConvertToRadians(this->angle.y))));

	//float ang = atan2f(move.y , move.x);
	// float len = move.Length();
	// move = { cosf(angle.y) * move.x,sinf(angle.y)x * move.y };

	debug[u8"角度"] = angle.y;

	if (false) {
		animator.SetParameter("moveX", -move.y);
		animator.SetParameter("moveY", move.x);
	}
	else {
		float len = move.Length();
		if (len > 1) len = 1;
		animator.SetParameter("moveX", 0.f);
		animator.SetParameter("moveY", len);
	}
}

// 入力データ取得
void Player::Input()
{
	InputManager& input = InputManager::Instance();
	float dt = Timer::Instance().DeltaTime();
	float frameDt = dt * 10;

	// --- 移動 ---
	Vector2 inputMoveData;
	inputMoveData.x = input.GetKeyPress(Keyboard::D) - input.GetKeyPress(Keyboard::A);
	inputMoveData.y = input.GetKeyPress(Keyboard::W) - input.GetKeyPress(Keyboard::S);

	// コントローラー対応
	if (input.IsGamePadConnected() && inputMoveData.x == 0 && inputMoveData.y == 0)
	{
		inputMoveData.x = input.GetThumSticksLeftX();
		inputMoveData.y = input.GetThumSticksLeftY();
	}

	if (inputMoveData.LengthSq() > 1)inputMoveData.Normalize();
	if (XMFLOAT2* move = std::get_if<XMFLOAT2>(&inputMap["Move"])) {
		Vector2 v1 = *move;
		inputMoveData =
			inputMoveData * Math::Lerp(v1.Length(), inputMoveData.Length(), frameDt);
	}

	inputMap["Move"] = inputMoveData.vec_;

	// --- 走り ---
	static bool  inputRunData = false;
	static float runTimer        = 0;

	if (inputMoveData.Length() > constant.dashDeadZone) runTimer += dt;
	else runTimer = 0;

	inputRunData = runTimer >= constant.shiftDashTimer;

	ability.moveSpeed = Math::Lerp(ability.moveSpeed, inputRunData ? constant.dashSpeed : constant.walkSpeed, frameDt);

	inputMap["Run"] = inputRunData;
	animator.SetParameter("run", inputRunData);

	// --- 攻撃 ---
	bool inputAttackData = input.GetKeyPressed(DirectX::Keyboard::Enter);

	// コントローラー対応
	if (input.IsGamePadConnected() && !inputAttackData)
	{
		inputAttackData = input.GetGamePadButtonPressed(GAMEPADBUTTON_STATE::x);
	}

	inputMap["Attack"] = inputAttackData;
	animator.SetParameter("attack", inputAttackData);
	if(inputAttackData)ability.attackTimer = 0.2f;


	// 攻撃の入力した時点で次の入力をする
	// static bool inputAttack = false;
	// if (bool* endAttack = std::get_if<bool>(&inputMap["EndAttack"])) {
	// 	bool attack = *endAttack || animator.GetEndMotion() ? inputAttackData: false;
	// 	if (attack) {
	// 		attackTimer = constant.inputReceptionTime;
	// 		if (ability.attackCount >= constant.maxAttackCombo) {
	// 			attack      = false;
	// 			attackTimer = 0;
	// 		}
	// 	}
	// 	if (*endAttack) ability.attackCount = 0;
	// 	if (ability.attackCount == 0) {
	// 		inputMap["Attack"] = attack;
	// 		animator.SetParameter("attack", attack);
	// 		if(attack) ability.attackCount++;
	// 	}
	// 	else {
	// 		if(attack)inputAttack = true;
	// 		inputMap["Attack"] = inputAttack && animator.GetEndMotion();
	// 		animator.SetParameter("attack", inputAttack && animator.GetEndMotion());
	// 		if (inputAttack && animator.GetEndMotion()) {
	// 			ability.attackCount++;
	// 			inputAttack = false;
	// 		}
	// 	}
	// }
	//else inputMap["Attack"] = false;
	//
	// if(animator.GetEndMotion()) attackTimer -= dt;
	// inputMap["EndAttack"] = attackTimer <= 0;
	// animator.SetParameter("endAttack", attackTimer <= 0);
	// debug[u8"攻撃時間"] = attackTimer;
	// animator.GetVelocity()

	// --- 回避 ---
	bool dodge = input.GetKeyPressed(DirectX::Keyboard::Space);

	if (input.IsGamePadConnected() && !dodge)
		dodge = input.GetGamePadButtonPress(GAMEPADBUTTON_STATE::a);

	inputMap["Dodge"] = dodge;
	//if (dodge) ability.dodgeTimer = constant.dodgeTime;

	// // --- ドリンク ---
	// bool inputDrinkData = InputManager::Instance().GetKeyPressed(DirectX::Keyboard::E);
	//
	// // コントローラー対応
	// if (input.IsGamePadConnected() && !inputDrinkData)
	// {
	// 	inputDrinkData = input.GetGamePadButtonPressed(GAMEPADBUTTON_STATE::x);
	// }
	// inputMap["Drink"] = inputDrinkData;
}

void Player::InputAttack() {
	InputManager& input = InputManager::Instance();
	float dt = Timer::Instance().DeltaTime();
	float frameDt = dt * 10;

	// --- 攻撃 ---
	bool inputAttackData = input.GetKeyPressed(DirectX::Keyboard::Enter);

	// コントローラー対応
	if (input.IsGamePadConnected() && !inputAttackData) {
		inputAttackData = input.GetGamePadButtonPressed(GAMEPADBUTTON_STATE::x);
	}
	inputMap["Attack"] = inputAttackData;

	if(inputAttackData) {
		ability.attackCount++;
		if (ability.attackCount >= constant.maxAttackCombo) ability.attackTimer = 0;
	}

	if (animator.GetEndMotion()) ability.attackTimer -= dt;
	inputMap["EndAttack"] = ability.attackTimer <= 0 && animator.GetEndMotion();
	animator.SetParameter("endAttack", ability.attackTimer <= 0 && animator.GetEndMotion());
	debug[u8"攻撃時間"] = ability.attackTimer;

	if(ability.attackTimer <= 0 && animator.GetEndMotion()) {
		ability.attackTimer = 0;
		ability.attackCount = 0;
	}

}

// 移動量計算
void Player::CalcWalkVelocity()
{
	// カメラの方向に対応する移動
	float deltaTime = Timer::Instance().DeltaTime();

#if 0
	velocity.x += Camera::Instance().GetFront().x * GetInputMap<DirectX::XMFLOAT2>("Move").y * ability.moveSpeed * deltaTime;
	velocity.z += Camera::Instance().GetFront().z * GetInputMap<DirectX::XMFLOAT2>("Move").y * ability.moveSpeed * deltaTime;

	velocity.x += Camera::Instance().GetRight().x * GetInputMap<DirectX::XMFLOAT2>("Move").x * ability.moveSpeed * deltaTime;
	velocity.z += Camera::Instance().GetRight().z * GetInputMap<DirectX::XMFLOAT2>("Move").x * ability.moveSpeed * deltaTime;
#else
	Vector2 move = GetInputMap<XMFLOAT2>("Move");

	Vector2 front = camera->GetFrontVec().xz();
	Vector2 right = camera->GetRightVec().xz();

	Vector2 vel = { 0,0 };
	vel += front * move.y;
	vel += right * move.x;

	vel *= ability.moveSpeed * deltaTime;

	velocity.x += vel.x;
	velocity.z += vel.y;

	// velocity.x +=  camera->GetFrontVec().x * move.y * ability.moveSpeed * deltaTime;
	// velocity.z +=  camera->GetFrontVec().z * move.y * ability.moveSpeed * deltaTime;
	// velocity.x +=  camera->GetRightVec().x * move.x * ability.moveSpeed * deltaTime;
	// velocity.z +=  camera->GetRightVec().z * move.x * ability.moveSpeed * deltaTime;
#endif
}

// 走り移動量計算
void Player::CalcRunVelocity()
{
	//float runPower = constant.dashPower;

	// カメラの方向に対応する移動
	float deltaTime = Timer::Instance().DeltaTime();
#if 0
	velocity.x += Camera::Instance().GetFront().x * GetInputMap<DirectX::XMFLOAT2>("Move").y * ability.moveSpeed * runPower *
		deltaTime;
	velocity.z += Camera::Instance().GetFront().z * GetInputMap<DirectX::XMFLOAT2>("Move").y * ability.moveSpeed * runPower *
		deltaTime;

	velocity.x += Camera::Instance().GetRight().x * GetInputMap<DirectX::XMFLOAT2>("Move").x * ability.moveSpeed * runPower *
		deltaTime;
	velocity.z += Camera::Instance().GetRight().z * GetInputMap<DirectX::XMFLOAT2>("Move").x * ability.moveSpeed * runPower *
		deltaTime;

#else
	Vector2 move = GetInputMap<XMFLOAT2>("Move");

	Vector2 front = camera->GetFrontVec().xz();
	Vector2 right = camera->GetRightVec().xz();

	Vector2 vel = {0,0};
	vel += front * move.y;
	vel += right * move.x;

	vel *= ability.moveSpeed * deltaTime;

	velocity.x += vel.x;
	velocity.z += vel.y;

	// velocity.x += camera->GetFrontVec().x * move.y * ability.moveSpeed * runPower * deltaTime;
	// velocity.z += camera->GetFrontVec().z * move.y * ability.moveSpeed * runPower * deltaTime;
	//
	// velocity.x += camera->GetRightVec().x * move.x * ability.moveSpeed * runPower * deltaTime;
	// velocity.z += camera->GetRightVec().z * move.x * ability.moveSpeed * runPower * deltaTime;
#endif
}

void Player::CalcDodgeVelocity() {
#if 1 // TODO::RootMotionを使うなら消す
	float dt = Timer::Instance().DeltaTime();
	Vector2 move = { velocity.x,velocity.z };
	move.Normalize();

	if(move.LengthSq() == 0) {
		move = -camera->GetFrontVec().xz();
	}

	move *= constant.dodgePower * dt;

	velocity.x += move.x;
	velocity.z += move.y;

	ability.dodgeTimer -= dt;
#else
	velocity += animator.GetVelocity();

#endif
}

void Player::CalcRootAnimationVelocity() {
	Vector3 animVel = animator.GetVelocity();
	XMMATRIX ST = XMLoadFloat4x4(&model->GetModelResource()->GetCoordinateSystemTransform());
	XMMATRIX S = XMMatrixScaling(10,10,10);
	XMMATRIX R = XMMatrixRotationY(XMConvertToRadians(angle.y));
	XMStoreFloat3(&animVel.vec_,XMVector3TransformCoord(animVel, ST*S*R));
	velocity += animVel.vec_;
}

// 移動している方向に向く
void Player::Turn()
{
	// 移動していなければ return 
	if (Float3Length(velocity) < 0.0001f) return;

	// プレイヤーの前方向ベクトルを取得(sinf, cosf を使用しているため勝手に単位ベクトルになっている)
	DirectX::XMFLOAT3 front =
	{
		sinf(DirectX::XMConvertToRadians(angle.y)),
		0,
		cosf(DirectX::XMConvertToRadians(angle.y))
	};
	// 移動方向の単位ベクトル取得
	DirectX::XMVECTOR N_VELOCITY = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&velocity));
	DirectX::XMFLOAT3 normalizeVelocity;
	DirectX::XMStoreFloat3(&normalizeVelocity, N_VELOCITY);

	// 内積を計算して回転角度を求める
	float dot      = (front.x * normalizeVelocity.x) + (front.z * normalizeVelocity.z);
	float rotPower = (1.0f - dot) * rot;
	if (rotPower > rot) rotPower = rot;

	// 外積計算
	float cross = (front.x * normalizeVelocity.z) - (front.z * normalizeVelocity.x);

	// 回転
	float deltaTime = Timer::Instance().DeltaTime();
	angle.y += (cross > 0) ? (-rotPower * deltaTime) : (rotPower * deltaTime);
}

// 移動
void Player::Move()
{
	position.x += velocity.x;
	position.z += velocity.z;
	velocity *= 0.98f * Timer::Instance().DeltaTime();
}

// 敵との衝突処理
void Player::CollisionVsEnemy()
{
	for (auto& eBoneSphere : Enemy::Instance().GetModel()->GetModelResource()->GetSkeletonSphereCollisions())
	{
		// ===== 体同士の当たり判定 =====
		for (auto& boneSphere : model->GetModelResource()->GetSkeletonSphereCollisions())
		{
			// 処理する２つのボーンの座標計算
			DirectX::XMFLOAT3 bonePos;
			DirectX::XMFLOAT3 eBonePos;
			if (boneSphere.name != "")
			{
				bonePos = GetBonePosition(boneSphere.name);
			}
			else
			{
				DirectX::XMVECTOR BONE_POS = DirectX::XMLoadFloat3(&boneSphere.position);
				DirectX::XMVECTOR POS      = DirectX::XMLoadFloat3(&position);
				DirectX::XMStoreFloat3(&bonePos, DirectX::XMVectorAdd(BONE_POS, POS));
			}
			if (eBoneSphere.name != "")
			{
				eBonePos = Enemy::Instance().GetBonePosition(eBoneSphere.name);
			}
			else
			{
				DirectX::XMFLOAT3 ePos     = Enemy::Instance().GetPos();
				DirectX::XMVECTOR BONE_POS = DirectX::XMLoadFloat3(&eBoneSphere.position);
				DirectX::XMVECTOR POS      = DirectX::XMLoadFloat3(&ePos);
				DirectX::XMStoreFloat3(&eBonePos, DirectX::XMVectorAdd(BONE_POS, POS));
			}

			// 衝突処理
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsSphere(
					eBonePos,
					eBoneSphere.radius,
					bonePos,
					boneSphere.radius,
					outPosition)
			)
			{
				// 押し出し後の位置設定
				outPosition.y = 0;
				SetPos(outPosition);
			}
		}

		// ===== 自分の攻撃と敵の体の当たり判定 =====
		for(auto& playerAnimSphereCollision : model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex).animSphereCollisions)
		{
			// 既にダメージを与えているかチェック
			if (playerAnimSphereCollision.isDamaged) continue;
			// フレーム範囲内かチェック
			if (currentKeyFrame < playerAnimSphereCollision.startFrame || currentKeyFrame > playerAnimSphereCollision.endFrame) continue;

			// 処理する２つのボーンの座標計算
			DirectX::XMFLOAT3 bonePos;
			DirectX::XMFLOAT3 eBonePos;
			if (playerAnimSphereCollision.bindBoneName != "")
			{
				bonePos = GetBonePosition(playerAnimSphereCollision.bindBoneName);
			}
			else
			{
				DirectX::XMVECTOR BONE_POS = DirectX::XMLoadFloat3(&playerAnimSphereCollision.position);
				DirectX::XMVECTOR POS = DirectX::XMLoadFloat3(&position);
				DirectX::XMStoreFloat3(&bonePos, DirectX::XMVectorAdd(BONE_POS, POS));
			}
			if (eBoneSphere.name != "")
			{
				eBonePos = Enemy::Instance().GetBonePosition(eBoneSphere.name);
			}
			else
			{
				DirectX::XMFLOAT3 ePos = Enemy::Instance().GetPos();
				DirectX::XMVECTOR BONE_POS = DirectX::XMLoadFloat3(&eBoneSphere.position);
				DirectX::XMVECTOR POS = DirectX::XMLoadFloat3(&ePos);
				DirectX::XMStoreFloat3(&eBonePos, DirectX::XMVectorAdd(BONE_POS, POS));
			}

			// 衝突処理
			DirectX::XMFLOAT3 collisionPoint;
			if (Collision::SphereVsSphereCollisionPoint(
				eBonePos,
				eBoneSphere.radius,
				bonePos,
				playerAnimSphereCollision.radius,
				collisionPoint)
				)
			{
				playerAnimSphereCollision.isDamaged = true;

				ConsoleData::Instance().logs.push_back("Damage!");

				Emitter* emitter = new Emitter();
				emitter->position = collisionPoint;
				emitter->emitterData.duration = 2.0;
				emitter->emitterData.looping = false;
				emitter->emitterData.burstsTime = 0.0;
				emitter->emitterData.burstsCount = 256;
				emitter->emitterData.particleKind = pk_PlayerAttackSpark;
				emitter->emitterData.particleLifeTimeMin = 0.4f;
				emitter->emitterData.particleLifeTimeMax = 0.6f;
				emitter->emitterData.particleSpeedMin = 15.0f;
				emitter->emitterData.particleSpeedMax = 30.0f;
				emitter->emitterData.particleSizeMin = { 0.25f, 0.005f };
				emitter->emitterData.particleSizeMax = { 2.0f, 0.1f };
				emitter->emitterData.particleColorMin = { 3.5, 3.5, 0.8, 1 };
				emitter->emitterData.particleColorMax = { 6.0, 6.0, 0.8, 1 };
				emitter->emitterData.particleGravity = 20;
				emitter->emitterData.particleBillboardType = 1;
				emitter->emitterData.particleTextureType = 0;
				emitter->emitterData.burstsOneShot = true;
				EmitterManager::Instance().Register(emitter);

				Emitter* emitter1 = new Emitter();
				emitter1->position = collisionPoint;
				emitter1->emitterData.duration = 2.0;
				emitter1->emitterData.looping = false;
				emitter1->emitterData.burstsTime = 0.03;
				emitter1->emitterData.burstsCount = 1;
				emitter1->emitterData.particleKind = pk_PlayerAttackHit;
				emitter1->emitterData.particleLifeTimeMin = 0.1f;
				emitter1->emitterData.particleLifeTimeMax = 0.1f;
				emitter1->emitterData.particleSpeedMin = 0.0f;
				emitter1->emitterData.particleSpeedMax = 0.0f;
				emitter1->emitterData.particleSizeMin = { 50.0f, 5.0f };
				emitter1->emitterData.particleSizeMax = { 50.0f, 5.0f };
				emitter1->emitterData.particleColorMin = { 1.9, 1.9, 8.8, 1 };
				emitter1->emitterData.particleColorMax = { 1.9, 1.9, 10.8, 1 };
				emitter1->emitterData.particleAngleMin = 0;
				emitter1->emitterData.particleAngleMax = 359;
				emitter1->emitterData.particleGravity = 20;
				emitter1->emitterData.particleBillboardType = 2;
				emitter1->emitterData.particleTextureType = 1;
				emitter1->emitterData.burstsOneShot = 2;

				emitter1->emitRateTimer = emitter1->emitterData.burstsTime;	// 生成時に発生するようにする
				EmitterManager::Instance().Register(emitter1);

				Emitter* emitter2 = new Emitter();
				emitter2->position = collisionPoint;
				emitter2->emitterData.duration = 2.0;
				emitter2->emitterData.looping = false;
				emitter2->emitterData.burstsTime = 0.0;
				emitter2->emitterData.burstsCount = 1;
				emitter2->emitterData.particleKind = 3;
				emitter2->emitterData.particleLifeTimeMin = 0.05f;
				emitter2->emitterData.particleLifeTimeMax = 0.05f;
				emitter2->emitterData.particleSpeedMin = 0.0f;
				emitter2->emitterData.particleSpeedMax = 0.0f;
				emitter2->emitterData.particleSizeMin = { 5.0f, 5.0f };
				emitter2->emitterData.particleSizeMax = { 5.0f, 5.0f };
				emitter2->emitterData.particleColorMin = { 8.5, 3.0, 0.8, 1 };
				emitter2->emitterData.particleColorMax = { 8.5, 3.0, 0.8, 1 };
				emitter2->emitterData.particleAngleMin = 0;
				emitter2->emitterData.particleAngleMax = 359;
				emitter2->emitterData.particleGravity = 20;
				emitter2->emitterData.particleBillboardType = 2;
				emitter2->emitterData.particleTextureType = 2;
				emitter2->emitterData.burstsOneShot = 1;
				EmitterManager::Instance().Register(emitter2);

				int dmg = rand() % 20 + 1;
				std::string dmgText = std::to_string(dmg);
				DamageTextManager::Instance().Register({ dmgText, collisionPoint });
			}
		}
	}
}

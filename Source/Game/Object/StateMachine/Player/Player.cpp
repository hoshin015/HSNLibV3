#include "Player.h"

#include "PlayerState.h"
#include "../../../../../Library/3D/CameraManager.h"
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
#include "../../../../../Library/Resource/Model/Animator.h"
#include "../../../../UserInterface/DamageTextManager.h"

#include "../../Source/Camera/CameraDerived.h"
#include "../../../../../Library/3D/DebugPrimitive.h"
#include "../../../../../Library/Graphics/ColorFilter.h"
#include "../../../../UserInterface/UiClearAfter.h"
#include "../../Stage/Gate.h"

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
	stateMachine->RegisterSubState(static_cast<int>(State::Normal), new PlayerDamageState(this));

	auto& animation = model->GetModelResource()->GetAnimationClips();

	Animator::Motion idle;
	idle.motion = &animation[3];
	idle.animationSpeed = 0.75f;
	idle.threshold = { 0,0 };

	Animator::Motion walkMae;
	walkMae.motion = &animation[16];
	walkMae.animationSpeed = 1.2f;
	walkMae.threshold = { 0,1 };

	Animator::Motion walkUsiro;
	walkUsiro.motion = &animation[18];
	walkUsiro.animationSpeed = 1.2f;
	walkUsiro.threshold = { 0,-1 };

	Animator::Motion walkLeft;
	walkLeft.motion = &animation[15];
	walkLeft.animationSpeed = 1.2f;
	walkLeft.threshold = { 1,0 };

	Animator::Motion walkRight;
	walkRight.motion = &animation[17];
	walkRight.animationSpeed = 1.2f;
	walkRight.threshold = { -1,0 };

	Animator::Motion runMotion;
	runMotion.motion = &animation[9];
	runMotion.animationSpeed = 1.25f;
	runMotion.threshold = { 0,1 };

	Animator::Motion attack1;
	attack1.motion = &animation[4];
	attack1.animationSpeed = 1.f;
	attack1.threshold = { 1,0 };
	attack1.loop = false;
	attack1.animationIndex = 4;

	Animator::Motion attack2;
	attack2.motion = &animation[5];
	attack2.animationSpeed = 1.f;
	attack2.threshold = { 1,0 };
	attack2.loop = false;
	attack2.animationIndex = 5;

	Animator::Motion attack3;
	attack3.motion = &animation[6];
	attack3.animationSpeed = 1.f;
	attack3.threshold = { 1,0 };
	attack3.loop = false;
	attack3.animationIndex = 6;

	Animator::Motion attack4;
	attack4.motion = &animation[7];
	attack4.animationSpeed = 1.f;
	attack4.threshold = { 1,0 };
	attack4.loop = false;
	attack4.animationIndex = 7;

	Animator::Motion dodgeMae;
	dodgeMae.motion = &animation[12];
	dodgeMae.animationSpeed = 1.f;
	dodgeMae.threshold = { 0,1 };
	dodgeMae.loop = false;

	Animator::Motion dodgeUsiro;
	dodgeUsiro.motion = &animation[14];
	dodgeUsiro.animationSpeed = 1.f;
	dodgeUsiro.threshold = { 0,-1 };
	dodgeUsiro.loop = false;

	Animator::Motion dodgeLeft;
	dodgeLeft.motion = &animation[11];
	dodgeLeft.animationSpeed = 1.f;
	dodgeLeft.threshold = { 1,0 };
	dodgeLeft.loop = false;

	Animator::Motion dodgeRight;
	dodgeRight.motion = &animation[13];
	dodgeRight.animationSpeed = 1.f;
	dodgeRight.threshold = { -1,0 };
	dodgeRight.loop = false;

	Animator::Motion hit;
	hit.motion = &animation[0];
	hit.animationSpeed = 1.f;
	hit.threshold = { -1,0 };
	hit.loop = false;

	Animator::Motion down;
	down.motion = &animation[1];
	down.animationSpeed = 1.f;
	down.threshold = { -1,0 };
	down.loop = false;

	Animator::Motion wakeUp;
	wakeUp.motion = &animation[8];
	wakeUp.animationSpeed = 1.f;
	wakeUp.threshold = { -1,0 };
	wakeUp.loop = false;

	Animator::Motion rise;
	rise.motion = &animation[8];
	rise.animationSpeed = 1.f;
	rise.threshold = { -1,0 };
	rise.loop = false;

	Animator::Motion death;
	death .motion = &animation[10];
	death .animationSpeed = 1.f;
	death .threshold = { -1,0 };
	death .loop = false;

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

	Animator::BlendTree dodgeTree;
	dodgeTree.motions.emplace_back(idle);
	dodgeTree.motions.emplace_back(dodgeMae);
	dodgeTree.motions.emplace_back(dodgeUsiro);
	dodgeTree.motions.emplace_back(dodgeLeft);
	dodgeTree.motions.emplace_back(dodgeRight);
	dodgeTree.maxSeconds = 1.033333f;
	dodgeTree.parameters[0] = "dodgeX";
	dodgeTree.parameters[1] = "dodgeY";

	Animator::State walkState;
	walkState.object = Animator::MakeObjPointer(walkTree);
	walkState.type = Animator::State::BLEND_TREE;
	walkState.transitions =
		STATE_FUNC(animator) {
		if (animator.GetParameter<bool>("run"))
			return &animator.GetState("run");

		// if (animator.GetParameter<bool>("attack"))
		// 	return &animator.GetState("attack1");

		if (animator.GetParameter<bool>("dodge"))
			return &animator.GetState("dodge");

		return nullptr;
		};

	Animator::State runState;
	runState.object = Animator::MakeObjPointer(runTree);
	runState.type = Animator::State::BLEND_TREE;
	runState.transitions =
		STATE_FUNC(animator) {
		if (!animator.GetParameter<bool>("run"))
			return &animator.GetState("walk");

		// if (animator.GetParameter<bool>("attack"))
		// 	return &animator.GetState("attack1");

		// if (animator.GetParameter<bool>("dodge"))
		// 	return &animator.GetState("dodge");

		return nullptr;
		};

	Animator::State attack1State;
	attack1State.object = Animator::MakeObjPointer(attack1);
	attack1State.type = Animator::State::MOTION;
	attack1State.transitions = STATE_FUNC(animator) {
		if (animator.GetParameter<bool>("endAttack"))
			return !animator.GetParameter<bool>("run") ?
			&animator.GetState("walk") :
			&animator.GetState("run");

		if (animator.GetParameter<bool>("attack"))
			return &animator.GetState("attack2");

		// if (animator.GetParameter<bool>("dodge"))
		// 	return &animator.GetState("dodge");

		return nullptr;
	};

	Animator::State attack2State;
	attack2State.object = Animator::MakeObjPointer(attack2);
	attack2State.type = Animator::State::MOTION;
	attack2State.transitions = STATE_FUNC(animator) {
		if (animator.GetParameter<bool>("endAttack"))
			return !animator.GetParameter<bool>("run") ?
			&animator.GetState("walk") :
			&animator.GetState("run");

		if (animator.GetParameter<bool>("attack"))
			return &animator.GetState("attack3");

		// if (animator.GetParameter<bool>("dodge"))
		// 	return &animator.GetState("dodge");

		return nullptr;
	};

	Animator::State attack3State;
	attack3State.object = Animator::MakeObjPointer(attack3);
	attack3State.type = Animator::State::MOTION;
	attack3State.transitions = STATE_FUNC(animator) {
		if (animator.GetParameter<bool>("endAttack"))
			return !animator.GetParameter<bool>("run") ?
			&animator.GetState("walk") :
			&animator.GetState("run");

		if (animator.GetParameter<bool>("attack"))
			return &animator.GetState("attack4");

		// if (animator.GetParameter<bool>("dodge"))
		// 	return &animator.GetState("dodge");

		return nullptr;
	};

	Animator::State attack4State;
	attack4State.object = Animator::MakeObjPointer(attack4);
	attack4State.type = Animator::State::MOTION;
	attack4State.transitions = STATE_FUNC(animator) {
		if (animator.GetParameter<bool>("endAttack")) {
			return !animator.GetParameter<bool>("run") ?
				&animator.GetState("walk") :
				&animator.GetState("run");
		}

		if (animator.GetParameter<bool>("attack"))
			return &animator.GetState("attack1");

		// if (animator.GetParameter<bool>("dodge"))
		// 	return &animator.GetState("dodge");

		return nullptr;
	};

	Animator::State dodgeState;
	dodgeState.object = Animator::MakeObjPointer(dodgeTree);
	dodgeState.type = Animator::State::BLEND_TREE;
	dodgeState.transitions =
		STATE_FUNC(animator) {
		if (animator.GetParameter<bool>("endDodge")) {
			// if (animator.GetParameter<bool>("attack"))
			// 	return &animator.GetState("attack1");

			if (!animator.GetParameter<bool>("run"))
				 return &animator.GetState("walk");
			else return &animator.GetState("run");
		}

		return nullptr;
	};

	Animator::State hitState;
	hitState.object = Animator::MakeObjPointer(hit);
	hitState.type = Animator::State::MOTION;

	Animator::State downState;
	downState.object = Animator::MakeObjPointer(down);
	downState.type = Animator::State::MOTION;

	Animator::State wakeUpState;
	wakeUpState.object = Animator::MakeObjPointer(wakeUp);
	wakeUpState.type = Animator::State::MOTION;

	Animator::State riseState;
	riseState.object = Animator::MakeObjPointer(rise);
	riseState.type = Animator::State::MOTION;

	Animator::State deathState;
	deathState.object = Animator::MakeObjPointer(death);
	deathState.type = Animator::State::MOTION;

	animator.AddState("walk", walkState);
	animator.AddState("run", runState);
	animator.AddState("attack1", attack1State);
	animator.AddState("attack2", attack2State);
	animator.AddState("attack3", attack3State);
	animator.AddState("attack4", attack4State);
	animator.AddState("dodge", dodgeState);
	animator.AddState("hit", hitState);
	animator.AddState("down", downState);
	animator.AddState("wakeUp", wakeUpState);
	animator.AddState("rise", riseState);
	animator.AddState("death", deathState);
	animator.EnableRootMotion("root");
	animator.SetModelSceneView(&model->GetModelResource()->GetSceneView());
	animator.SetEntryState("walk");


	// 左壁手前
	for (int i = 0; i < 11; i++)
	{
		wallSpheres.emplace_back(Vector3{ static_cast<float>(i) + 5.0f, 0.0f, 106.5f });
	}
	wallSpheres.emplace_back(Vector3(4.3f, 0.0f, 106.5f));

	// 左壁側面
	for (int i = 0; i < 28; i++)
	{
		wallSpheres.emplace_back(Vector3(4.3f, 0.0f, 106.5f + -static_cast<float>(i)));
	}

	// 右壁手前
	wallSpheres.emplace_back(Vector3(-4.3f, 0.0f, 106.5f));
	for (int i = 0; i < 11; i++)
	{
		wallSpheres.emplace_back(Vector3{ -static_cast<float>(i) + -5.0f, 0.0f, 106.5f });
	}

	// 右壁側面
	for (int i = 0; i < 28; i++)
	{
		wallSpheres.emplace_back(Vector3(-4.3f, 0.0f, 106.5f + -static_cast<float>(i)));
	}

	// swordTrail
	swordTrail = std::make_unique<SwordTrail>();

	// 樽の当たり判定
	wallSpheres.emplace_back(Vector3(24.2f, 0.0f, 141.8f));
	wallSpheres.emplace_back(Vector3(26.0f, 0.0f, 137.8f));
	wallSpheres.emplace_back(Vector3(22.0f, 0.0f, 146.5f));
	wallSpheres.emplace_back(Vector3(20.5f, 0.0f, 148.7f));

	// ベット
	// 足元側
	for(int i = 0; i < 4; i++)
	{
		wallSpheres.emplace_back(Vector3(21.0f, 0.0f, 130.7f + static_cast<float>(i)));
	}

	// 左側面
	for (int i = 0; i < 7; i++)
	{
		wallSpheres.emplace_back(Vector3(22.0f + static_cast<float>(i), 0.0f, 133.7f));
	}

	// 右側面
	for (int i = 0; i < 7; i++)
	{
		wallSpheres.emplace_back(Vector3(22.0f + static_cast<float>(i), 0.0f, 130.7f));
	}
}

void Player::Initialize()
{
	// ステートセット
	stateMachine->SetState(static_cast<int>(State::Normal));

	// idle アニメーション再生
	PlayAnimation(static_cast<int>(PlayerAnimNum::Idle), true);

	Respawn();

	hitStopTimer = 0.0f;
}

void Player::Update()
{
	// 入力データ取得
	// --- カメラ関連 ---
	static bool cameraFlag = true;
	InputManager& input = InputManager::Instance();
	bool lockOn = input.GetMousePressed(MOUSEBUTTON_STATE::rightButton);
	if(input.IsGamePadConnected()&&!lockOn) {
		lockOn = input.GetGamePadButtonPressed(GAMEPADBUTTON_STATE::rightStick);
	}

	if (lockOn) {
		// --- プレイヤーカメラをセット ---
		if (!cameraFlag) {
			auto camera = CameraManager::Instance().GetCamera();
			Vector3 p = camera->GetCurrentPosition();
			Vector3 target = camera->GetTarget();
			CameraManager::Instance().SetCurrentCamera("PlayerCamera");
			auto ptr = std::dynamic_pointer_cast<PlayerCamera>(CameraManager::Instance().GetCamera());
			ptr->SetCurrentPosition(p);
			ptr->SetTarget(target);
			ptr->currentTarget = target;
			Player::Instance().SetCamera(CameraManager::Instance().GetCamera().get());

			cameraFlag = !cameraFlag;
		}

		// --- ロックオンカメラをセット ---
		else {
			if (!Enemy::Instance().IsDead() && enterStage) {
				auto camera = CameraManager::Instance().GetCamera();
				Vector3 position = camera->GetPosition();
				Vector3 target = camera->GetTarget();
				CameraManager::Instance().SetCurrentCamera("LockOnCamera");
				camera = CameraManager::Instance().GetCamera();
				camera->Initialize();
				//camera = &lockOnCamera;
				//camera->Initialize();

				camera->SetPosition(position);
				camera->SetCurrentPosition(position);
				camera->SetTarget(target);
				Player::Instance().SetCamera(camera.get());

				cameraFlag = !cameraFlag;
			}
		}
	}
	
	// ステートマシン更新
	stateMachine->Update();

	// 敵との当たり判定
	CollisionVsEnemy();

	CalcJustDodge();

	// アニメーション更新
	//SetAnimatorKeyFrame(keyFrame);
	UpdateAnimationParam();
	// Animatorを使ったモーション
	animatorKeyFrame = animator.PlayAnimation(Timer::Instance().DeltaTime());
	currentKeyFrame = animator.GetKeyFrameIndex();
	currentAnimationIndex = animator.GetMotionIndex();


	// --- 位置の制限 ---
	ClampPosition(79.0f);

	UpdateHitStopTimer();


	// 姿勢行列更新
	UpdateTransform();

	swordTrail->Update();
	PowerSwordEffetUpdate();
}

void Player::Render(bool isShadow)
{
	model->Render(transform, &animatorKeyFrame, isShadow);
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

		if (ImGui::Button(u8"再配置", { 200.0f, 30.0f }))
		{
			Respawn();
		}

		if(ImGui::TreeNode(u8"壁の球"))
		{
			int i = 0;
			for (auto& sphere : wallSpheres)
			{
				std::string name = "Sphere" + std::to_string(i);
				if (ImGui::TreeNode(name.c_str()))
				{
					ImGui::DragFloat3(u8"位置", &sphere.x);
					ImGui::TreePop();
				}
				i++;
			}
			ImGui::TreePop();
		}
		if (ImGui::Button(u8"球追加", { 200.0f, 30.0f }))
		{
			wallSpheres.emplace_back();
		}

		ImGui::DragFloat(u8"壁の球の半径", &wallSphereRadius);
		ImGui::DragFloat(u8"プレイヤーの球の半径", &playerRadius);

		ImGui::Separator();
		ImGui::Checkbox(u8"入口に入ったか", &enterEntrance);
		ImGui::DragFloat3(u8"待機場所の中心", &restRoomCenter.x);
		ImGui::DragFloat(u8"待機場所の半径", &restRoomRadius);
		ImGui::DragFloat3(u8"入口の立方体の位置", &entrance.position.x);
		ImGui::DragFloat3(u8"入口の立方体のサイズ", &entrance.size.x);
		ImGui::DragFloat3(u8"入口の左壁の位置", &entranceLWall.position.x);
		ImGui::DragFloat3(u8"入口の左壁のサイズ", &entranceLWall.size.x);
		ImGui::DragFloat3(u8"入口の右壁の位置", &entranceRWall.position.x);
		ImGui::DragFloat3(u8"入口の右壁のサイズ", &entranceRWall.size.x);
		ImGui::Checkbox(u8"ヒット", &isHit);
		ImGui::DragFloat(u8"ヒットストップタイマー", &hitStopTimer);
		ImGui::DragFloat(u8"ヒットストップの時間", &hitStopTime);

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
				ImGui::DragFloat(u8"攻撃時間", &ability.attackTimer,0.1f);
				ImGui::DragFloat(u8"攻撃回数", &ability.attackCount,0.1f);

				ImGui::DragFloat(u8"回避時間", &ability.dodgeTimer, 0.1f);
				ImGui::Checkbox(u8"ジャスト回避", &ability.isJustDodge);

				ImGui::DragFloat(u8"体幹攻撃力", &ability.bodyTrunkStrength, 0.1f);
				ImGui::DragFloat(u8"体幹攻撃力の振れ幅", &ability.bodyTrunkStrengthRange, 0.1f);

				ImGui::TreePop();
			}

			if(ImGui::TreeNode(u8"定数値")) {
				ImGui::DragFloat(u8"最大体力", &constant.maxHp);

				ImGui::DragFloat(u8"歩き速度", &constant.walkSpeed,0.01f);
				ImGui::DragFloat(u8"ダッシュ移行時間", &constant.shiftDashTimer,0.01f);
				ImGui::DragFloat(u8"ダッシュ倍率", &constant.dashSpeed,0.01f);
				static float rate = constant.dashDeadZone * 100;
				if(ImGui::SliderFloat(u8"ダッシュデットゾーン", &rate,0,100,"%.0f%%"))
					constant.dashDeadZone = rate / 100;

				ImGui::DragFloat(u8"回避強さ", &constant.dodgePower,0.01f);
				ImGui::DragFloat(u8"回避時間", &constant.dodgeTime,0.01f);
				ImGui::DragFloat(u8"最小回避時間", &constant.dodgeLowestTime,0.01f);
				ImGui::DragFloat(u8"無敵回避時間", &constant.dodgeInvincibleTime,0.01f);
				ImGui::DragFloat(u8"ジャスト回避時間", &constant.justDodgeTime,0.01f);

				ImGui::DragFloat(u8"最低攻撃力", &constant.leastStrength,0.01f);
				//ImGui::DragFloat(u8"最大攻撃力", &constant.maxStrength, 0.01f);

				ImGui::DragFloat(u8"最低怯み力", &constant.leastBt, 0.01f);
				//ImGui::DragFloat(u8"最大怯み力", &constant.maxBt, 0.01f);

				ImGui::TreePop();
			}

			if (ImGui::Button(u8"ダメージ")) {
				HitDamaged(10);
			}
			if (ImGui::Button(u8"吹っ飛び")) {
				Vector3 pPos = position;
				Vector3 ePos = Enemy::Instance().GetPos();
				Vector3 fryVec = pPos - ePos;
				fryVec.Normalize();
				HitDamaged(10, false,true, fryVec * 30);
			}

			if(ImGui::Button(u8"死亡")) {
				HitDamaged(constant.maxHp);
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
	//Vector2 move = GetInputMap<XMFLOAT2>("Move");
	//velocity.
	//debug[u8"方向"] = move.vec_;
	//XMStoreFloat2(&move.vec_,XMVector2TransformCoord(XMLoadFloat2(&move.vec_),XMMatrixRotationZ(XMConvertToRadians(this->angle.y))));

	//float ang = atan2f(move.y , move.x);
	// float len = move.Length();
	// move = { cosf(angle.y) * move.x,sinf(angle.y)x * move.y };

	//debug[u8"角度"] = angle.y;
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
	// static bool  inputRunData = false;
	// static float runTimer        = 0;

	//if (inputMoveData.Length() > constant.dashDeadZone) runTimer += dt;
	//else runTimer = 0;

	if (inputMoveData.Length() < constant.dashDeadZone) {
		inputMap["Run"] = false;
		animator.SetParameter("run", false);
	}

	//inputRunData = runTimer >= constant.shiftDashTimer;

	//ability.moveSpeed = Math::Lerp(ability.moveSpeed, inputRunData ? constant.dashSpeed : constant.walkSpeed, frameDt);

	// inputMap["Run"] = inputRunData;
	// animator.SetParameter("run", inputRunData);

	// --- 攻撃 ---
	bool inputAttackData = input.GetMousePressed(MOUSEBUTTON_STATE::leftButton);

	// コントローラー対応
	if (input.IsGamePadConnected() && !inputAttackData)
	{
		inputAttackData = input.GetGamePadButtonPressed(GAMEPADBUTTON_STATE::x);
	}

	inputMap["Attack"] = inputAttackData;
	//animator.SetParameter("attack", inputAttackData);

	// --- 回避 ---
	bool dodge = input.GetKeyPressed(DirectX::Keyboard::Space);

	if (input.IsGamePadConnected() && !dodge)
		dodge = input.GetGamePadButtonPressed(GAMEPADBUTTON_STATE::a);

	inputMap["Dodge"] = dodge;
	animator.SetParameter("dodge", dodge);

	bool dodgeHold = input.GetKeyPress(DirectX::Keyboard::Space);
	if (input.IsGamePadConnected() && !dodgeHold)
		dodgeHold = input.GetGamePadButtonPress(GAMEPADBUTTON_STATE::a);

	inputMap["DodgeHold"] = dodgeHold;
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

	if (!CameraManager::Instance().IsCurrentCamera("LockOnCamera")||GetInputMap<bool>("Run")) {
		float len = inputMoveData.Length();
		if (len > 1) len = 1;
		animator.SetParameter("moveX", 0.f);
		animator.SetParameter("moveY", len);
	}
	else if(!dodge) {
		animator.SetParameter("moveX", inputMoveData.x);
		animator.SetParameter("moveY", inputMoveData.y);
	}
}

void Player::InputAttack() {
	InputManager& input = InputManager::Instance();
	float dt = Timer::Instance().DeltaTime();
	float frameDt = dt * 10;

	// --- 攻撃 ---
	inputMap["Attack"] = false;
	animator.SetParameter("attack", false);
	static bool at = false;
	bool inputAttackData = input.GetMousePressed(MOUSEBUTTON_STATE::leftButton);

	// コントローラー対応
	if (input.IsGamePadConnected() && !inputAttackData) {
		inputAttackData = input.GetGamePadButtonPressed(GAMEPADBUTTON_STATE::x);
	}

	if(inputAttackData && !at) {
		at = true;
		ability.attackCount++;
		//if (ability.attackCount >= constant.maxAttackCombo) ability.attackTimer = 0;
	}

	if (animator.GetEndMotion()) {
		inputMap["Attack"] = at;
		animator.SetParameter("attack", at);
		if (at)
		{
			swordTrail->Clear();
			ability.attackTimer = constant.attackReceptionTime;
			ClearAnimSphereCollisionDamagedFlag();
			ClearSeFlag();
		}
		else ability.attackTimer -= dt;
		at = false;

	}

	bool end = ability.attackTimer <= 0 && animator.GetEndMotion() || ability.isHitDamage;
	inputMap["EndAttack"] = end;
	animator.SetParameter("endAttack", end);

	// --- 回避 ---
	bool dodge = input.GetKeyPressed(DirectX::Keyboard::Space);

	if (input.IsGamePadConnected() && !dodge)
		dodge = input.GetGamePadButtonPressed(GAMEPADBUTTON_STATE::a);

	if (ability.notAcceptTimer <= 0)inputMap["Dodge"] = dodge;
	else ability.notAcceptTimer -= dt;

	if(end) {
		ability.attackTimer = 0;
		ability.attackCount = 0;
	}

	// --- 移動 ---
	Vector2 inputMoveData;
	inputMoveData.x = input.GetKeyPress(Keyboard::D) - input.GetKeyPress(Keyboard::A);
	inputMoveData.y = input.GetKeyPress(Keyboard::W) - input.GetKeyPress(Keyboard::S);

	// コントローラー対応
	if (input.IsGamePadConnected() && inputMoveData.x == 0 && inputMoveData.y == 0) {
		inputMoveData.x = input.GetThumSticksLeftX();
		inputMoveData.y = input.GetThumSticksLeftY();
	}

	inputMoveData.Normalize();

	inputMap["AttackMove"] = inputMoveData.vec_;

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

	vel *= ability.moveSpeed * constant.dashSpeed * deltaTime;

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
	const float dt      = Timer::Instance().DeltaTime();
	const float frameDt = dt * 10;
	Vector2     vec     = { velocity.x,velocity.z };
	float       rad     = XMConvertToRadians(angle.y);

	if(vec.LengthSq() == 0) vec = -Vector2(sinf(rad),cosf(rad));
	vec.Normalize();
	XMVECTOR Vec = XMLoadFloat2(&vec.vec_);

	// 回避中に移動
	Vector2 move = GetInputMap<XMFLOAT2>("Move");
	if(move.LengthSq() != 0) {
		move.Normalize();

		Vector2 front = camera->GetFrontVec().xz();
		Vector2 right = camera->GetRightVec().xz();
		Vector2 wMove;

		wMove += front * move.y;
		wMove += right * move.x;

		// debug["wMove"] = wMove.vec_;
		// debug["Vec"] = vec.vec_;

		XMVECTOR lerp = XMVectorLerp(Vec, XMLoadFloat2(&wMove.vec_), frameDt);
		lerp = XMVector2Normalize(lerp);
		XMVECTOR Dot = XMVector2Dot(lerp, Vec);
		float cross = XMVectorGetX(XMVector2Cross(lerp, Vec));
		float deg = XMConvertToDegrees(XMVectorGetX(XMVectorACos(Dot)));
		if (cross < 0) deg *= -1;

		angle.y += deg;

		Vec = lerp;
		XMStoreFloat2(&vec.vec_, lerp);
	}

	Vector2 vel = vec * constant.dodgePower * dt;

	velocity.x += vel.x;
	velocity.z += vel.y;

	ability.dodgeTimer -= dt;

	XMStoreFloat2(&vec.vec_,XMVector2TransformCoord(Vec,XMMatrixRotationZ(rad)));
	animator.SetParameter("dodgeX", vec.x);
	animator.SetParameter("dodgeY", vec.y);

#else
	velocity += animator.GetVelocity();

#endif
}

void Player::CalcAttackVelocity() {
	const float dt = Timer::Instance().DeltaTime();
	const float frameDt = dt * 2;

	Vector2 move = GetInputMap<XMFLOAT2>("AttackMove");
	Vector2 vel = { velocity.x,velocity.z };
	vel.Normalize();

	if (move.LengthSq() <= 0 || vel.LengthSq() <= 0||
		animator.GetEndMotion()) return;

	XMVECTOR Vel = XMLoadFloat2(&vel.vec_);

	Vector2 front = camera->GetFrontVec().xz();
	Vector2 right = camera->GetRightVec().xz();
	Vector2 wMove;

	wMove += front * move.y;
	wMove += right * move.x;
	wMove.Normalize();

	XMVECTOR lerp = XMVectorLerp(Vel, XMLoadFloat2(&wMove.vec_), dt/4);
	//lerp = XMVector2Normalize(lerp);

	XMVECTOR Dot = XMVector2Dot(lerp,Vel);
	float cross = XMVectorGetX(XMVector2Cross(lerp, Vel));
	float deg = XMConvertToDegrees(XMVectorGetX(XMVectorACos(Dot)));
	if (cross < 0) deg *= -1;

	angle.y += deg;
}

void Player::CalcJustDodge() {
	float dt = Timer::Instance().DeltaTime();

	if(ability.isJustDodge) {
		//ability.strength = min(ability.strength+constant.incrementStrength,constant.maxStrength);
		//ability.bodyTrunkStrength = min(ability.bodyTrunkStrength + constant.incrementBt, constant.maxBt);

		//Timer::Instance().SetTimeScale(0.3f);
		//OnHitAttack(false);
		ability.skillGauge += constant.incrementSkill;
		if(ability.skillGauge>=constant.maxSkillGauge) {
			ability.isSkillGaugeMax = true;
		}
		ability.justDodgeSlowTimer = 0;
		ability.isJustDodge = false;
	}
	else {
		if (ability.skillGauge <= 0) { ability.isSkillGaugeMax = false; }

		// スキル関係
		if (ability.isSkillGaugeMax) {
			ability.strength = constant.leastStrength * constant.skillDamageRate;
			ability.bodyTrunkStrength = constant.leastBt * constant.skillDamageRate;
			ability.skillGauge -= dt;
		}
		else {
			ability.strength = constant.leastStrength;
			ability.bodyTrunkStrength = constant.leastBt;
		}

		ability.justDodgeInvincibleTimer -= dt;

		if (GetInputMap<bool>("Attack")) {
			ability.justDodgeSlowTimer = 3;
			Timer::Instance().SetTimeScale(1);
		}
		ability.justDodgeSlowTimer += Timer::Instance().UnscaledDeltaTime();
		if (ability.justDodgeSlowTimer >= 3) ability.justDodgeSlowTimer = 3;
		//(cos(x * pi*2)+1)/2 = (cosf(ability.justDodgeSlowTimer/3 * 6.2831853072)+1)/2
		static float sat = 1;
		sat = Math::Lerp(sat, ability.justDodgeInvincibleTimer <= 0 ? 1 : 0, Timer::Instance().UnscaledDeltaTime() * 15);
		colorFilter->SetSaturation(sat);
		if(ability.justDodgeSlowTimer < 3)Timer::Instance().SetTimeScale(Easing::InQuad(ability.justDodgeSlowTimer, 3.f,1.f,0.f));
	}
}

void Player::HitDamaged(float damage, bool invincibleInvalid, bool flying ,Vector3 vec) {
	ability.hitDamage = damage;
	ability.isHitDamage = ability.hitDamage > 0;
	ability.isInvincibleInvalidDamage = invincibleInvalid;
	ability.isFlying = flying;
	ability.flyVec = vec;
}

void Player::CalcRootAnimationVelocity() {
	Vector3 animVel = animator.GetVelocity();
	XMMATRIX ST = XMLoadFloat4x4(&model->GetModelResource()->GetCoordinateSystemTransform());
	XMMATRIX S = XMMatrixScaling(1,1,1);
	XMMATRIX R = XMMatrixRotationY(XMConvertToRadians(angle.y));
	XMStoreFloat3(&animVel.vec_,XMVector3TransformCoord(animVel, ST*S*R));
	velocity += animVel.vec_;
}

// 移動している方向に向く
void Player::Turn()
{
	Vector3 vel;

	CameraManager& camera = CameraManager::Instance();
	if (camera.IsCurrentCamera("LockOnCamera")&&!GetInputMap<bool>("Run")) {
		vel = Vector3(Enemy::Instance().GetPos()) - Vector3(position);
	}
	else vel = velocity;

	// 移動していなければ return
	if (vel.Length() < 0.0001f) return;
	vel.Normalize();

	// プレイヤーの前方向ベクトルを取得(sinf, cosf を使用しているため勝手に単位ベクトルになっている)
	Vector3 front =
	{
		sinf(DirectX::XMConvertToRadians(angle.y)),
		0,
		cosf(DirectX::XMConvertToRadians(angle.y))
	};
	// 移動方向の単位ベクトル取得

	DirectX::XMFLOAT3 normalizeVelocity = vel.vec_;

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
	// ヒットストップのバグ対策
	if (Timer::Instance().DeltaTime() <= 0.0f)
		return;

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
			if(Enemy::Instance().IsAlive() && ability.hp > 0.0f) // 生きてたら押し出す
			{
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
		}

		// TODO:攻撃
		// ===== 自分の攻撃と敵の体の当たり判定 =====

		auto& anim = model->GetModelResource()->GetAnimationClips().at(currentAnimationIndex).animSphereCollisions;
		for(auto& playerAnimSphereCollision : anim)
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
				for (auto && collision: anim) {
					collision.isDamaged = true;
				}
				Enemy& enemy = Enemy::Instance();
				enemy.OnAttacked(ability.strength);

				//ConsoleData::Instance().logs.push_back("Damage!");

				Emitter* emitter = new Emitter();
				emitter->position = collisionPoint;
				emitter->emitterData.duration = 2.0;
				emitter->emitterData.looping = false;
				emitter->emitterData.burstsTime = 0.0;
				emitter->emitterData.burstsCount = 256;
				emitter->emitterData.particleKind = pk_PlayerAttackSpark;
				emitter->emitterData.particleLifeTimeMin = 0.2f;
				emitter->emitterData.particleLifeTimeMax = 0.6f;
				emitter->emitterData.particleSpeedMin = 3.0f;
				emitter->emitterData.particleSpeedMax = 8.0f;
				emitter->emitterData.particleSizeMin = { 0.25f, 0.005f };
				emitter->emitterData.particleSizeMax = { 8.0f, 0.05f };
				emitter->emitterData.particleColorMin = { 3.5, 3.5, 0.8, 1 };
				emitter->emitterData.particleColorMax = { 6.0, 6.0, 0.8, 1 };
				emitter->emitterData.particleGravity = 8;
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
				emitter1->emitterData.particleSizeMin = { 70.0f, 2.0f };
				emitter1->emitterData.particleSizeMax = { 70.0f, 2.0f };
				emitter1->emitterData.particleColorMin = { 1.9, 1.9, 8.8, 1 };
				emitter1->emitterData.particleColorMax = { 1.9, 1.9, 10.8, 1 };
				emitter1->emitterData.particleAngleMin = 0;
				emitter1->emitterData.particleAngleMax = 359;
				emitter1->emitterData.particleGravity = 20;
				emitter1->emitterData.particleBillboardType = 2;
				emitter1->emitterData.particleTextureType = 1;
				emitter1->emitterData.burstsOneShot = 1;

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

				bool weakness = eBoneSphere.skeletonType == SkeletonSphereCollision::SkeletonType::WeakPoint1;
				OnHitAttack(weakness);
				float rate = weakness ? 1.5f : 1;
				float btStrength = Math::RandomRange(ability.bodyTrunkStrength * rate - ability.bodyTrunkStrengthRange, ability.bodyTrunkStrength * rate + ability.bodyTrunkStrengthRange);
				float strength = Math::RandomRange(ability.strength * rate - ability.strengthRange, ability.strength * rate + ability.strengthRange);
				enemy.SetFlinchValue(enemy.GetFlinchValue() - btStrength);
				Enemy::Instance().OnAttacked(strength);

				std::string dmgText = std::to_string(static_cast<int>(strength));
				DamageTextManager::Instance().Register({ dmgText, collisionPoint });
			}
		}
	}
}

void Player::PowerSwordEffetUpdate()
{
	DirectX::XMFLOAT3 tail = GetBonePosition("sword_05");
	DirectX::XMFLOAT3 head = GetBonePosition("sword_01");

	DirectX::XMVECTOR TAIL = DirectX::XMLoadFloat3(&tail);
	DirectX::XMVECTOR HEAD = DirectX::XMLoadFloat3(&head);
	DirectX::XMVECTOR SUB = DirectX::XMVectorSubtract(TAIL, HEAD);
	DirectX::XMVECTOR N = DirectX::XMVector3Normalize(SUB);
	float length = DirectX::XMVectorGetX(DirectX::XMVector3Length(SUB));
	length += 0.5f;

	static const float swordTime = 0.001f;
	static float swordTimer = 0.0f;

	swordTimer += Timer::Instance().DeltaTime();
	while(swordTimer > swordTime)
	{
		swordTimer -= swordTime;

		// 生成座標の決定
		float lengthPower = Math::RandomRange(0.0f, length);
		DirectX::XMVECTOR GeneratePos = HEAD;
		GeneratePos = DirectX::XMVectorAdd(GeneratePos, DirectX::XMVectorScale(N, lengthPower));
		DirectX::XMFLOAT3 generatePos;
		DirectX::XMStoreFloat3(&generatePos, GeneratePos);

		{
			Emitter* emitter0 = new Emitter();
			emitter0->position = generatePos;
			emitter0->emitterData.duration = 10.0;
			emitter0->emitterData.looping = false;
			emitter0->emitterData.burstsTime = 0.0;
			emitter0->emitterData.burstsCount = 10;
			emitter0->emitterData.particleKind = pk_swordPowerUp;
			emitter0->emitterData.particleLifeTimeMin = 0.05f;
			emitter0->emitterData.particleLifeTimeMax = 0.05f;
			emitter0->emitterData.particleSpeedMin = 1.0f;
			emitter0->emitterData.particleSpeedMax = 1.0f;
			emitter0->emitterData.particleSizeMin = { 0.1f, 1.0f };
			emitter0->emitterData.particleSizeMax = { 0.5f, 2.0f };
			emitter0->emitterData.particleColorMin = { 1.0, 1.0, 4.0, 1 };
			emitter0->emitterData.particleColorMax = { 1.0, 1.0, 4.0, 1 };
			emitter0->emitterData.particleGravity = 0;
			emitter0->emitterData.particleBillboardType = 0;
			emitter0->emitterData.particleTextureType = 0;
			emitter0->emitterData.burstsOneShot = 1;
			EmitterManager::Instance().Register(emitter0);
		}
	}
}

void Player::DrawDebug()
{
	DebugPrimitive::Instance().AddSphere(restRoomCenter.vec_, restRoomRadius, { 0.3f, 0.3f, 1.0f, 1.0f });
	DebugPrimitive::Instance().AddCube(position, {1.5f, 1.5f, 1.5f}, { 0.3f, 0.3f, 1.0f, 1.0f });
	DebugPrimitive::Instance().AddCube(entrance.position.vec_, entrance.size.vec_, { 0.3f, 0.3f, 1.0f, 1.0f });
	DebugPrimitive::Instance().AddCube(entranceLWall.position.vec_, entranceLWall.size.vec_, { 0.3f, 0.3f, 1.0f, 1.0f });
	DebugPrimitive::Instance().AddCube(entranceLWall.position.vec_, entranceLWall.size.vec_, { 0.3f, 0.3f, 1.0f, 1.0f });
	DebugPrimitive::Instance().AddCube(entranceRWall.position.vec_, entranceRWall.size.vec_, { 0.3f, 0.3f, 1.0f, 1.0f });

	for (auto& sphere : wallSpheres)
	{
		DebugPrimitive::Instance().AddSphere(sphere.vec_, wallSphereRadius, { 1.0f, 0.3f, 0.3f, 1.0f });
	}

	DebugPrimitive::Instance().AddSphere(position, playerRadius, { 1.0f, 0.3f, 0.3f, 1.0f });
}

void Player::OnHitAttack(bool hitWeak)
{
	// Enemy::Instance().OnAttacked(ability.strength);
	Enemy::Instance().wasAttacked = true;

	Timer::Instance().SetTimeScale(0.0f);

	// Todo : 頭なら大き目にヒットストップさせる
	CameraManager::Instance().shakeTimer = hitStopTime;
	hitStopTimer = (hitWeak) ? weakHitStopTime : hitStopTime;
}

void Player::UpdateHitStopTimer()
{
	if (hitStopTimer > 0.0f)
	{
		hitStopTimer -= Timer::Instance().UnscaledDeltaTime();

		if (hitStopTimer < 0.0f)
		{
			hitStopTimer = 0.0f;
			Timer::Instance().SetTimeScale(1.0f);
		}
	}
}


static bool IntersectBoxVSBox(const Vector3& positionA, const Vector3& sizeA, const Vector3& positionB, const Vector3& sizeB)
{
	Vector3 rightTopBackA = positionA + sizeA * 0.5f;
	Vector3 leftBottomFrontA = positionA - sizeA * 0.5f;

	Vector3 rightTopBackB = positionB + sizeB * 0.5f;
	Vector3 leftBottomFrontB = positionB - sizeB * 0.5f;

	if (leftBottomFrontA.x < rightTopBackB.x &&
		rightTopBackA.x    > leftBottomFrontB.x &&
		leftBottomFrontA.y < rightTopBackB.y &&
		rightTopBackA.y    > leftBottomFrontB.y &&
		leftBottomFrontA.z < rightTopBackB.z &&
		rightTopBackA.z    > leftBottomFrontB.z)
		return true;

	return false;
}


// --- 位置の制限 ---
void Player::ClampPosition(float range)
{
	// ステージ
	Vector3 pos = position;
	float length = pos.Length();


	// 待機場所
	Vector3 restRoomVec = pos - restRoomCenter;
	float restRoomLength = restRoomVec.Length();




	//auto extrusion = [&](const Vector3& cubePosition, const float elapsedTime)
	//	{
	//		// --- ブロックの面の向きに押し返す ---
	//		Vector3 vec = pos - cubePosition;
	//		vec.Normalize();

	//		// --- Y軸 0固定 ---
	//		vec.y = 0.0f;

	//		const Vector3 directions[4] =
	//		{
	//			Vector3::Front_,
	//			Vector3::Left_,
	//			Vector3::Back_,
	//			Vector3::Right_
	//		};

	//		float dots[4] = {};

	//		for (size_t i = 0; i < 4; i++)
	//			dots[i] = vec.Dot(directions[i]);


	//		size_t indices[2] = { -1, -1 };

	//		indices[0] = (dots[0] > dots[2]) ? 0 : 2;
	//		indices[1] = (dots[1] > dots[3]) ? 1 : 3;

	//		indices[0] = (dots[indices[0]] > dots[indices[1]]) ? indices[0] : indices[1];

	//		position += (directions[indices[0]] * 25.0f * elapsedTime).vec_;
	//	};



	// ステージに入ってなかったら
	if(!enterStage)
	{
		// 入り口の判定
		if (!IntersectBoxVSBox(entrance.position, entrance.size, position, { 1.5f, 1.5f, 1.5f }))
		{
			// 入ってなかったら待機場所に
			if (restRoomLength > restRoomRadius)
			{
				if (!enterStage)
				{
					restRoomVec.Normalize();
					position = (restRoomCenter + restRoomVec * restRoomRadius).vec_;
				}
			}
		}

		else
		{
			enterEntrance = true;
		}


		for (auto& sphere : wallSpheres)
		{
			Vector3 outPosition;
			if (Collision::IntersectSphereVsSphere(sphere.vec_, wallSphereRadius, position, playerRadius, outPosition.vec_))
			{
				position = outPosition.vec_;
			}
		}

		//// 左右の壁
		//if (collision(entranceLWall.position, entranceLWall.size, position, { 1.0f, 1.0f, 1.0f }))
		//{
		//	//Vector3 vec = pos - entranceLWall.position;
		//	//vec.Normalize();
		//	//position += (vec * 0.5f).vec_;

		//	extrusion(entranceLWall.position, Timer::Instance().DeltaTime());
		//}

		//if (collision(entranceRWall.position, entranceRWall.size, position, { 1.0f, 1.0f, 1.0f }))
		//{
		//	//Vector3 vec = pos - entranceRWall.position;
		//	//vec.Normalize();
		//	//position += (vec * 0.5f).vec_;

		//	extrusion(entranceRWall.position, Timer::Instance().DeltaTime());
		//}
	}


	// ステージ内
	if (length > range)
	{
		if(enterStage)
		{
			pos.Normalize();
			pos *= range;
			position = pos.vec_;
		}
	}

	else
	{
		if (!enterStage) enterStage = true;
	}
}

// --- 配置しなおし ---
void Player::Respawn()
{
	position = { 0.0f, 0.0f, 135.0f };	// 初期位置
	angle = { 0.0f, 180.0f, 0.0f };
	enterStage = false;
	enterEntrance = false;
	lockOn = false;

	// 能力値初期設定
	ability = AbilityStatus();
	ability.hp = constant.maxHp;

	CameraManager::Instance().SetCurrentCamera("PlayerCamera");

	Gate::Instance().Initialize();
}

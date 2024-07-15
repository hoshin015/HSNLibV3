#include "CameraDerived.h"

#include "../../External/ImGui/imgui.h"

#include "../../Library/3D/CameraManager.h"

#include "../../Library/Timer.h"

#include "../../Library/Input/InputManager.h"

#include "../../Library/Math/Collision.h"

#include "../Game/Object/StateMachine/Player/Player.h"
#include "../Game/Object/StateMachine/Enemy/Enemy.h"
#include "../Game/Object/Stage/StageManager.h"


// ===== プレイヤーのカメラ ======================================================================================================================================================
void PlayerCamera::Initialize()
{
	verticalAngle = 45.0f;
	horizontalAngle = 0.0f;

	// 勝手に range 変えてます
	//range = 13.0f;
	range = 18.0f;

	height = 6.0f;
	fixedCursor = true;
	
	// --- カメラ位置と目標の設定 ---
	currentPosition = position = Player::Instance().GetPos();
	Vector3 playerVelocity = Vector3::Normalize(Player::Instance().GetVelocity());	// プレイヤーの移動ベクトル
	Vector3 offset = { playerVelocity.x, height, playerVelocity.z };
	target = position + offset;
	currentTarget = target;
}


void PlayerCamera::Update()
{
	switch (state)
	{
	case 0:
		Initialize();
		state++;
		[[fallthrough]];

	case 1:
	{
		float deltaTime = Timer::Instance().DeltaTime();

		Player& player = Player::Instance();
		Vector3 playerPos = player.GetPos();	// プレイヤーの座標
		Vector3 offset = { 0.0f, height, 0.0f };
		Vector3 velocity = player.GetVelocity();
		target = playerPos + offset + Vector3::Normalize(velocity) * 2.0f/* * 500.0f * velocity.Length()*/;


		// --- カーソルを固定するか ---
		if (fixedCursor)
		{
			OnFixedCursor(deltaTime);
		}

		// --- 角度でベクトルを作ってカメラの位置を決める ---
		CalcPositionFromAngle(playerPos);

		// --- 今の位置を本来あるべき位置へ補完し続ける ---
		currentPosition = Vector3::Lerp(currentPosition, position, t);
		currentTarget = Vector3::Lerp(currentTarget, target, 0.1f);



		Vector3 shakeOffset;
		Vector3 shakeIntensity = { 10.0f, 10.0f, 1.0f };
		auto& manager = CameraManager::Instance();
		if (manager.shakeTimer > 0.0f)
		{
			shakeOffset = OnShake(shakeIntensity);
		}



		// --- 床に埋まらないように制限 ---
		if (currentPosition.y < 1.0f)
			currentPosition.y = 1.0f;

		if (position.y < 1.0f)
			position.y = 1.0f;

		if (verticalAngle < 0.0f)
			verticalAngle = 0.0f;


		// --- 仮の壁とのレイキャスト ---
		HitResult result;
		if (StageManager::Instance().RayCast(1, target.vec_, position.vec_, result))
		{
			currentPosition = result.position;
			position = result.position;
		}

		//// --- 待機場所 ---
		//result = {};
		//if (StageManager::Instance().RayCast(3, target.vec_, position.vec_, result))
		//{
		//	currentPosition = result.position;
		//	position = result.position;
		//}


		// --- カメラ情報の更新 ---
		CameraBase::Update(currentPosition/*position*/ + shakeOffset, currentTarget, up, fov, nearZ, farZ, aspect);

		break;
	}

	}
}


void PlayerCamera::UpdateConstants()
{
	CameraBase::UpdateConstants(currentPosition);
}


void PlayerCamera::DrawDebugGui()
{
	ImGui::Begin(u8"プレイヤーカメラ");

	InputManager& input = InputManager::Instance();

	ImGui::BulletText(u8"Mでカーソル固定を切り替え");
	if (input.GetKeyPressed(DirectX::Keyboard::Keys::M))
		fixedCursor = !fixedCursor;

	if (ImGui::Button(u8"シェイク", { 100.0f, 30.0f }))
		CameraManager::Instance().shakeTimer = 0.3f;


	ImGui::BulletText(u8"カーソル");
	ImGui::Text(u8"x : %f / y : %f", input.GetCursorPosXFloat(), input.GetCursorPosYFloat());

	ImGui::BulletText(u8"カメラの角度");
	ImGui::Text(u8"x : %f / y : %f", horizontalAngle, verticalAngle);

	ImGui::DragFloat(u8"感度", &sensitivity, 0.1f, 0.1f);
	ImGui::DragFloat(u8"目標からの距離", &range, 0.1f, 0.1f);
	ImGui::DragFloat(u8"目標の高さ", &height, 0.1f, 0.1f);

	ImGui::DragFloat3(u8"位置", &position.x);
	ImGui::DragFloat3(u8"今の位置", &currentPosition.x);
	ImGui::DragFloat(u8"補完量", &t, 0.01f);

	ImGui::DragFloat3(u8"目標", &target.x);

	float atan = atan2(position.y, position.x);
	atan += DirectX::XM_PIDIV2;
	ImGui::Text("Atan : %f", DirectX::XMConvertToDegrees(atan));

	ImGui::End();
}


// --- カメラを設定したときに呼ばれる ---
void PlayerCamera::OnSetCamera()
{
	// --- ターゲットカメラは角度を使用していないので、どうにかして合わせる ---
	Vector3 enemyPos = Enemy::Instance().GetPos();
	Vector3 playerPos = Player::Instance().GetPos();
	Vector2 vec = enemyPos.xz() - playerPos.xz();
	float atan = atan2(vec.y, vec.x);
	atan = DirectX::XMConvertToDegrees(atan);
	atan += 180.0f; // -180.0 ~ 180.0を0.0 ~ 360.0に

	horizontalAngle = atan;
	verticalAngle = 30.0f;	// 垂直角度は大体30度

	Vector3 offset = { 0.0f, height, 0.0f };
	float horizontalTheta = DirectX::XMConvertToRadians(horizontalAngle);
	float verticalTheta = DirectX::XMConvertToRadians(verticalAngle);
	Vector3 cameraVec = { cosf(horizontalTheta), sinf(verticalTheta), sinf(horizontalTheta) };
	position = playerPos + cameraVec * range;
}


// --- カーソルを固定しているときに呼ばれる ---
void PlayerCamera::OnFixedCursor(float deltaTime)
{	
	InputManager& input = InputManager::Instance();
	Vector2 curCursorPos;

	// --- 中央に固定 ---
		// Todo : 位置がモニターの左上の座標なのでどうにかする
	//SetCursorPos(640, 360);
	input.SetCursorPosition(640, 360);
	
	// --- カーソルの座標を取得して中央との差分をとる ---
	curCursorPos = { input.GetCursorPosXFloat(), input.GetCursorPosYFloat() };
	Vector2 cursorDelta = curCursorPos - Vector2{ 640.0f, 360.0f };

	// コントローラ対応
	if(input.IsGamePadConnected()) {
		cursorDelta = { input.GetThumSticksRightX(),-input.GetThumSticksRightY() };
		cursorDelta *= 200;
	}

	// --- 縦の角度をクランプ ---
	verticalAngle = (std::max)(-90.0f, (std::min)(verticalAngle, 90.0f));

	// --- カーソルの差分だけ角度を加算 ---
	horizontalAngle += -cursorDelta.x * deltaTime * sensitivity;		// 右側がプラスなので符号を反転させる
	verticalAngle += cursorDelta.y * deltaTime * sensitivity * 2.0f;	// 画面比率を考慮して2倍


	// --- 角度を360.0 ~ 0.0の間に制限 ---
	if (horizontalAngle > 360.0f)
		horizontalAngle -= 360.0f;

	else if (horizontalAngle < 0.0f)
		horizontalAngle += 360.0f;
}


Vector3 PlayerCamera::OnShake(const Vector3& intensity)
{
	Vector3 result;
	float shakePower = CameraManager::Instance().shakePower;
	if (static_cast<int>(shakePower) < 1) return { 0,0,0 };
	result = rightVec * (((rand() % static_cast<int>(shakePower)) / shakePower) - 0.5f) * intensity.x;
	result = upVec * (((rand() % static_cast<int>(shakePower)) / shakePower) - 0.5f) * intensity.y;
	result = frontVec * (((rand() % static_cast<int>(shakePower)) / shakePower) - 0.5f) * intensity.z;
	return result;
}


// --- 角度から位置を計算 ---
void PlayerCamera::CalcPositionFromAngle(const Vector3& position)
{
	float horizontalTheta = DirectX::XMConvertToRadians(horizontalAngle);
	float verticalTheta = DirectX::XMConvertToRadians(verticalAngle);
	Vector3 cameraVec = { cosf(horizontalTheta), sinf(verticalTheta), sinf(horizontalTheta) };
	this->position = position + cameraVec * range;
}



// ===== ロックオンカメラ ======================================================================================================================================================
void LockOnCamera::Initialize()
{
	height = 7.0f;
	range = 18.0f;
}

void LockOnCamera::Update()
{
	switch (state)
	{
	case 0:
		Initialize();
		state++;
		[[fallthrough]];

		
	case 1:
	{
		Vector3 playerPos = Player::Instance().GetPos();	// プレイヤーの位置
		Vector3 enemyPos = Enemy::Instance().GetPos();		// 目標の位置
		prevTarget = target;
		//Vector3 enemyPos = Enemy::Instance().GetBonePosition("atama");

		// --- 前回との移動量が小さかったら更新しない ---
		//float l = Vector3(prevTarget - enemyPos).Length();
		//if (l < 0.005f)
		//	break;

		target = enemyPos;	// 目標
		target.y += 3.0f;

		// 敵とプレイヤーとカメラを一直線に並べる
		Vector3 vec = playerPos - enemyPos;	// 敵からプレイヤーへのベクトル
		float length = vec.Length();		// 敵とプレイヤーの距離
		vec.Normalize();

		Vector3 offset = { 0.0f, height, 0.0f };
		position = target + offset + vec * (length + range);


		if (position.y < 1.0f)
			position.y = 1.0f;



		// --- カメラシェイク ---
		Vector3 shakeOffset;
		Vector3 shakeIntensity = { 10.0f, 10.0f, 1.0f };
		auto& manager = CameraManager::Instance();
		if (manager.shakeTimer > 0.0f)
		{
			shakeOffset = OnShake(shakeIntensity);
		}


		// --- 仮の壁とのレイキャスト ---
		HitResult result;
		if (StageManager::Instance().RayCast(1, target.vec_, position.vec_, result))
		{
			currentPosition = result.position;
			position = result.position;
		}


		currentPosition = position;
		CameraBase::Update(currentPosition + shakeOffset, target, up, fov, nearZ, farZ, aspect);

		break;
	}

	}
}

void LockOnCamera::UpdateConstants()
{
	CameraBase::UpdateConstants();
}

void LockOnCamera::DrawDebugGui()
{
	ImGui::Begin(u8"ロックオンカメラ");

	ImGui::DragFloat(u8"高さ", &height, 0.1f);
	ImGui::DragFloat(u8"距離", &range, 0.1f);

	if (ImGui::Button(u8"シェイク", { 100.0f, 30.0f }))
		CameraManager::Instance().shakeTimer = 0.3f;

	ImGui::End();
}

Vector3 LockOnCamera::OnShake(const Vector3& intensity)
{
	Vector3 result;
	float shakePower = CameraManager::Instance().shakePower;
	if (static_cast<int>(shakePower) < 1) return { 0,0,0 };
	result = rightVec * (((rand() % static_cast<int>(shakePower)) / shakePower) - 0.5f) * intensity.x;
	result = upVec * (((rand() % static_cast<int>(shakePower)) / shakePower) - 0.5f) * intensity.y;
	result = frontVec * (((rand() % static_cast<int>(shakePower)) / shakePower) - 0.5f) * intensity.z;
	return result;
}




void EnemyDeadCamera::Initialize()
{
}

void EnemyDeadCamera::Update()
{
	auto lerp = [](const float start, const float end, const float t)
		{
			return start + (end - start) * t;
		};
	static bool enter;
	if (!enter)
	{
		{
			state1Data.time = 5.0f;
			state1Data.range = 25.0f;
			state1Data.height = 3.0f;
			state1Data.beginAngle = 0.0f;
			state1Data.endAngle = 60.0f;
		}

		{
			state2Data.time = 7.0f;
			state2Data.range = 25.0f;
			state2Data.height = 15.0f;
			state2Data.beginAngle = 0.0f;
			state2Data.endAngle = 360.0f;
		}

		{
			state3Data.time = 10.0f;
			state3Data.range = 10.0f;
			state3Data.height = 3.0f;
		}

		enter = true;
	}


	switch(state)
	{
	case 0:

		timer = state1Data.time;

		state++;
		break;


	case 1:
	{
		Vector3 headPos = Enemy::Instance().GetBonePosition("atama");
		target = headPos;	// 目標

		// --- 敵の位置から頭の位置へのベクトルを算出 ---
		Vector3 enemyPosition = Enemy::Instance().GetPos();
		Vector3 vec = headPos - enemyPosition;
		vec.Normalize();

		// --- ベクトルを回転 ---
		Quaternion rot;
		rot.SetRotationDegY(lerp(state1Data.beginAngle, state1Data.endAngle, timer / state1Data.time));
		vec = Quaternion(rot * Quaternion(vec.x, vec.y, vec.z, 0.0f) * Quaternion(-rot.x, -rot.y, -rot.z, rot.w)).xyz();

		// --- ベクトルで位置を決めて、高さ分移動させる ---
		position = enemyPosition + vec * state1Data.range;
		position.y += state1Data.height;


		float elapsedTime = Timer::Instance().DeltaTime();
		if (updateTimer)
			timer -= elapsedTime;
		if (timer < 0.0f)
		{
			timer = state2Data.time;
			state++;
		}
		break;
	}


	case 2:
	{
		Vector3 headPos = Enemy::Instance().GetBonePosition("atama");
		target = headPos;	// 目標

		Vector3 vec = Vector3::Front_;

		// --- ベクトルを回転 ---
		Quaternion rot;
		rot.SetRotationDegY(lerp(state2Data.beginAngle, state2Data.endAngle, timer / state2Data.time));
		vec = Quaternion(rot * Quaternion(vec.x, vec.y, vec.z, 0.0f) * Quaternion(-rot.x, -rot.y, -rot.z, rot.w)).xyz();

		// --- ベクトルで位置を決めて、高さ分移動させる ---
		position = headPos + vec * state2Data.range;
		position.y += state2Data.height;


		float elapsedTime = Timer::Instance().DeltaTime();
		if (updateTimer)
			timer -= elapsedTime;
		if (timer < 0.0f)
		{
			timer = state3Data.time;
			state3Data.range = 10.0f;
			state3Data.height = 3.0f;
			state++;
		}

		break;
	}


	case 3:
	{
		// --- 真ん中へのベクトル ---
		Vector3 vec = Vector3::Zero_ - Enemy::Instance().GetPos();
		vec.Normalize();

		//// --- ベクトルを回転 ---
		//Quaternion rot;
		//rot.SetRotationDegY(lerp(state2Data.beginAngle, state2Data.endAngle, timer / state2Data.time));
		//vec = Quaternion(rot * Quaternion(vec.x, vec.y, vec.z, 0.0f) * Quaternion(-rot.x, -rot.y, -rot.z, rot.w)).xyz();

		// --- ベクトルで位置を決めて、高さ分移動させる ---
		position = Vector3::Zero_ + vec * state3Data.range;
		position.y += state3Data.height;

		float elapsedTime = Timer::Instance().DeltaTime();
		state3Data.range += elapsedTime;
		state3Data.height += elapsedTime * 3.0f;


		if (updateTimer)
			timer -= elapsedTime;
		if (timer < 0.0f)
		{
			CameraManager::Instance().SetCurrentCamera("PlayerCamera");
			state = 0;
		}

		break;
	}

	}

	if (position.y < 1.0f)
		position.y = 1.0f;

	CameraBase::Update();
}

void EnemyDeadCamera::UpdateConstants()
{
	CameraBase::UpdateConstants();
}

void EnemyDeadCamera::DrawDebugGui()
{
	ImGui::Begin(u8"敵の死亡カメラ");

	ImGui::InputInt(u8"ステート", &state);
	ImGui::DragFloat(u8"タイマー", &timer, 0.1f);
	ImGui::Checkbox(u8"タイマー更新", &updateTimer);
	if (ImGui::Button(u8"リセット", { 200.0f, 30.0f }))
	{
		state = 0;
	}
	if (ImGui::TreeNode(u8"ステート1"))
	{
		ImGui::BulletText(u8"近くで顔の周りを回転");
		ImGui::DragFloat(u8"時間", &state1Data.time, 0.1f);
		ImGui::DragFloat(u8"距離", &state1Data.range);
		ImGui::DragFloat(u8"高さ", &state1Data.height);
		ImGui::DragFloat(u8"開始角度", &state1Data.beginAngle);
		ImGui::DragFloat(u8"終了角度", &state1Data.endAngle);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode(u8"ステート2"))
	{
		ImGui::BulletText(u8"ちょっと離れて全体ぐるぐる");
		ImGui::DragFloat(u8"時間", &state2Data.time, 0.1f);
		ImGui::DragFloat(u8"距離", &state2Data.range);
		ImGui::DragFloat(u8"高さ", &state2Data.height);
		ImGui::DragFloat(u8"開始角度", &state2Data.beginAngle);
		ImGui::DragFloat(u8"終了角度", &state2Data.endAngle);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode(u8"ステート3"))
	{
		ImGui::BulletText(u8"画面だんだん引いていく");
		ImGui::DragFloat(u8"時間", &state3Data.time, 0.1f);
		ImGui::DragFloat(u8"距離", &state3Data.range);
		ImGui::DragFloat(u8"高さ", &state3Data.height);
		ImGui::TreePop();
	}


	ImGui::End();
}



// ===== プレイヤーの死亡時のカメラ ======================================================================================================================================================
void PlayerDeadCamera::Initialize()
{
	timer = time;
}

void PlayerDeadCamera::Update()
{
	auto lerp = [](const float start, const float end, const float t)
		{
			return start + (end - start) * t;
		};

	switch (state)
	{
	case 0:
		Initialize();
		state++;
		break;


	case 1:
	{
		Quaternion rot;
		rot.SetRotationDegY(lerp(0.0f, 360.0f, timer / time));

		Matrix R;
		R.MakeRotation(Player::Instance().GetAngle());
		Vector3 vec = R.v_[2].xyz();
		vec = Quaternion(rot * Quaternion(vec.x, vec.y, vec.z, 0.0f) * Quaternion(-rot.x, -rot.y, -rot.z, rot.w)).xyz();

		target = Player::Instance().GetBonePosition("pelvis");
		position = target + vec * range;
		position.y += height;

		float elapsedTime = Timer::Instance().DeltaTime();
		timer -= elapsedTime;
		if (timer < 0.0f)
		{
			state = 0;
			CameraManager::Instance().SetCurrentCamera("PlayerCamera");
		}

		break;
	}

	}


	if (position.y < 1.0f)
		position.y = 1.0f;

	CameraBase::Update();
}

void PlayerDeadCamera::UpdateConstants()
{
	CameraBase::UpdateConstants();
}

void PlayerDeadCamera::DrawDebugGui()
{
	ImGui::DragFloat(u8"距離", &range);
	ImGui::DragFloat(u8"高さ", &height);
}

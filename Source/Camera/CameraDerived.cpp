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
	range = 28.0f;

	height = 6.0f;
	fixedCursor = true;
	
	// --- カメラ位置と目標の設定 ---
	currentPosition = position = Player::Instance().GetPos();
	Vector3 playerVelocity = Vector3::Normalize(Player::Instance().GetVelocity());	// プレイヤーの移動ベクトル
	Vector3 offset = { playerVelocity.x, height, playerVelocity.z };
	target = position + offset;
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

		Vector3 playerPos = Player::Instance().GetPos();	// プレイヤーの座標
		Vector3 offset = { 0.0f, height, 0.0f };
		target = playerPos + offset;


		// --- カーソルを固定するか ---
		if (fixedCursor)
		{
			OnFixedCursor(deltaTime);
		}

		// --- 角度でベクトルを作ってカメラの位置を決める ---
		CalcPositionFromAngle(playerPos);

		// --- 今の位置を本来あるべき位置へ補完し続ける ---
		currentPosition = Vector3::Lerp(currentPosition, position, t);



		Vector3 shakeOffset;
		Vector3 shakeIntensity = { 10.0f, 10.0f, 1.0f };
		if (shakeTimer > 0.0f)
		{
			shakeTimer -= deltaTime;
			shakeOffset = OnShake(shakeIntensity);
		}
		
		else
		{
			shakeTimer = 0.0f;
			shakeOffset = Vector3::Zero_;
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

		// --- 待機場所 ---
		result = {};
		if (StageManager::Instance().RayCast(3, target.vec_, position.vec_, result))
		{
			currentPosition = result.position;
			position = result.position;
		}


		// --- カメラ情報の更新 ---
		CameraBase::Update(currentPosition + shakeOffset, target, up, fov, nearZ, farZ, aspect);

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
		shakeTimer = 0.3f;


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
	const Vector2 cursorDelta = curCursorPos - Vector2{ 640.0f, 360.0f };

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
	range = 30.0f;
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

		// 敵とプレイヤーとカメラを一直線に並べる
		Vector3 vec = playerPos - enemyPos;	// 敵からプレイヤーへのベクトル
		float length = vec.Length();		// 敵とプレイヤーの距離
		vec.Normalize();

		Vector3 offset = { 0.0f, height, 0.0f };
		position = target + offset + vec * (length + range);


		if (position.y < 1.0f)
			position.y = 1.0f;

		CameraBase::Update();

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

	ImGui::End();
}



void EnemyDeadCamera::Initialize()
{
}

void EnemyDeadCamera::Update()
{
	switch(state)
	{
	case 0:

		timer = 3.0f;

		state++;
		break;


	case 1:
	{
		Vector3 headPos = Enemy::Instance().GetBonePosition("atama");
		target = headPos;	// 目標

		Vector3 enemyPosition = Enemy::Instance().GetPos();
		Vector3 vec = headPos - enemyPosition;
		vec.Normalize();

		position = enemyPosition + vec * 25.0f;

		if (position.y < 1.0f)
			position.y = 1.0f;

		CameraBase::Update();


		float elapsedTime = Timer::Instance().DeltaTime();
		timer -= elapsedTime;
		if (timer < 0.0f)
		{
			CameraManager::Instance().SetCurrentCamera("PlayerCamera");
		}
		break;
	}


	}
}

void EnemyDeadCamera::UpdateConstants()
{
	CameraBase::UpdateConstants();
}

void EnemyDeadCamera::DrawDebugGui()
{
}

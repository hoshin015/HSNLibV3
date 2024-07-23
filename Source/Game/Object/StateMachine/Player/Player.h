#pragma once

#include <variant>
#include "../StateMachine.h"
#include "../../../../../Library/Resource/Model/Animator.h"
#include "../../Source/Game/Object/Base/AnimatedObject.h"

#include "../../Library/3D/CameraBase.h"
#include "../../../../Other/SwordTrail/SwordTrail.h"
#include "../../Library/Easing.h"


class ColorFilter;

enum class PlayerAnimNum
{
	Attack,
	Drink,
	Idle,
	Run,
	Walk,
};

class Player : public AnimatedObject
{
public:
	// ゲーム中変動する値
	struct AbilityStatus {
		float hp          = 0;
		float moveSpeed   = 7;

		// 攻撃
		float strength      = 10;
		float strengthRange = 1;
		float attackCount   = 0;
		float attackTimer   = 0;
		float skillGauge    = 0;
		bool isSkillGaugeMax = false;

		// 回避
		float dodgeTimer         = 0;
		float notAcceptTimer     = 0;
		float justDodgeSlowTimer = 3;
		float justDodgeInvincibleTimer = 0;
		bool  isJustDodge        = false;

		// 体幹
		float bodyTrunkStrength      = 5;
		float bodyTrunkStrengthRange = 3; // ランダムの範囲

		float hitDamage = 0;
		bool isHitDamage = false;
		bool isInvincibleInvalidDamage = false;

		Vector3 flyVec = { 0,0,0 };
		bool isFlying = false;
	};

	// 外部要因(ImGuiとか)でのみ変動する値
	struct ConstantStatus {
		float maxHp = 1000;

		float walkSpeed      = 5;
		float shiftDashTimer = 0.4f;
		float dashSpeed      = 2.4f;
		float dashDeadZone   = 0.7f;

		// 回避
		float dodgePower      = 24;
		float dodgeTime       = 1.5f;
		float dodgeLowestTime = 0.42f;
		float dodgeInvincibleTime = 0.5f;
		float justDodgeTime   = 0.21f;
		float justDodgeInvincibleTime = 0.3f;

		// 攻撃
		float maxAttackCombo      = 4;
		float attackReceptionTime = 0.1f;
		float notAcceptTime       = 0.16f;
		float leastStrength       = 10;
		//float maxStrength         = 60;
		//float incrementStrength   = 2;
		float maxSkillGauge   = 100;
		float incrementSkill  = 20;
		float incSkillRange   = 5;
		float skillDamageRate = 3;

		float leastBt = 5;
		//float maxBt = 25;
		//float incrementBt = 2;

		// alpha
		float alphaTimer = 0.6f;
		float alphaTime = 0.6;
		Easing::EasingValue dodgeAlphaUp =
		{
			0.3f, 0.6f,
			0.0f,1.0f
		};
	};

private:

	Player(const char* filePath);

	~Player() override
	{
	}
public:
	static Player& Instance()
	{
		static Player instance("Data/Fbx/GaoPlayer/gaoplayer_6.model");
		return instance;
	}

	void Initialize();
	void Update() override;
	void Render(bool isShadow = false) override;
	void Render(ID3D11PixelShader* ps);
	void DrawDebugImGui(int number);

	void UpdateAnimationParam();

	// 入力データ取得
	void Input();
	void InputAttack();
	// 歩行移動量計算
	void CalcWalkVelocity();
	// 走り移動量計算
	void CalcRunVelocity();
	// 回避
	void CalcDodgeVelocity();
	// 攻撃
	void CalcAttackVelocity();
	void CalcJustDodge();

	// ダメージ
	void HitDamaged(float damage,bool invincibleInvalid = false,  bool flying = false, Vector3 vec = {0,0,0});

	// RootAnimation
	void CalcRootAnimationVelocity();

	// 移動している方向に向く
	void Turn();
	// 移動
	void Move();

	// 敵との衝突処理
	void CollisionVsEnemy();

	void SetCamera(CameraBase* camera) { this->camera = camera; }

	// 剣強化エフェクト
	void PowerSwordEffetUpdate();

	void DrawDebug();

	void OnHitAttack(bool hitWeak);

	void UpdateHitStopTimer();

	void AlphaUpdate();

public:
	enum class State
	{
		Normal,
	};

	enum class Normal
	{
		Idle,
		Walk,
		Run,
		Attack,
		Dodge,
		Damage,
		Drink,
	};

private:
	std::unique_ptr<StateMachine<Player>> stateMachine;

	DirectX::XMFLOAT3 velocity  = {0, 0, 0};
	//float             moveSpeed = 3.0f; // プレイヤーが１秒間に加速する加速度
	float             rot       = 720;  // プレイヤーが１秒間に回転する角度

	// 入力データ保管用
	using inputData = std::variant<bool, int, float, DirectX::XMFLOAT2>;
	std::map<std::string, inputData> inputMap;

#ifdef _DEBUG
	std::map<std::string, inputData> debug;
#endif

	// --- カメラのポインタ ---
	CameraBase* camera;

	ColorFilter* colorFilter;

	// ステータス
	AbilityStatus ability;
	ConstantStatus constant;

public:
	StateMachine<Player>* GetStateMachine() { return stateMachine.get(); }

	DirectX::XMFLOAT3 GetVelocity() { return velocity; }
	void              SetVelocity(DirectX::XMFLOAT3 velocity) { this->velocity = velocity; }

	void SetColorFilter(ColorFilter* cf) { colorFilter = cf; }

	template <typename T>
	const T& GetInputMap(const std::string& str)
	{
		return std::get<T>(inputMap[str]);
	}

	void SetInputMap(const std::string& str, const inputData& data) { inputMap[str] = data; }

	AbilityStatus& AStatus() { return ability; }
	ConstantStatus& CStatus() { return constant; }
	//Animator& GetAnimator() { return animator; }

	void ClampPosition(const float range);
	void Respawn();
	
	bool enterStage;
	bool lockOn;
	bool enterEntrance;

	Vector3 restRoomCenter = { 0.0f, 0.0f, 132.0f };
	float restRoomRadius = 28.0f;
	Vector3 size = { 10.0f, 10.0f, 10.0f };
	bool isHit;
	Vector3 entranceCubePosition = { 0.0f, 0.0f, 92.0f };
	Vector3 entranceCubeSize = { 7.0f, 1.0f, 31.0f };
	struct EntranceCube
	{
		Vector3 position;
		Vector3 size;
	}entrance = { {0.0f, 0.0f, 95.0f}, {7.0f, 1.0f, 31.0f} },
		entranceLWall = { {9.0f, 0.0f, 94.25f}, {11.0f, 12.0f, 26.0f} },
		entranceRWall = { {-9.0f, 0.0f, 94.25f}, {11.0f, 12.0f, 26.0f} };

	std::vector<Vector3> wallSpheres;
	float wallSphereRadius = 1.0f;
	float playerRadius = 1.0f;



	// SwordTrail
	std::unique_ptr<SwordTrail> swordTrail;
	float hitStopTimer;
	float hitStopTime = 0.025f;
	float weakHitStopTime = 0.05f;

public:
	// 描画カラー
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pbrPS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> wboitPbrPS;
	DirectX::XMFLOAT4 drawColor = { 1,1,1,1 };
	Vector4 hitColor = { 0.0f, 0.0f, 1.0f, 1.0f };
};
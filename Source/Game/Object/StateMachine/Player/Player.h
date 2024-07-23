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
	// �Q�[�����ϓ�����l
	struct AbilityStatus {
		float hp          = 0;
		float moveSpeed   = 7;

		// �U��
		float strength      = 10;
		float strengthRange = 1;
		float attackCount   = 0;
		float attackTimer   = 0;
		float skillGauge    = 0;
		bool isSkillGaugeMax = false;

		// ���
		float dodgeTimer         = 0;
		float notAcceptTimer     = 0;
		float justDodgeSlowTimer = 3;
		float justDodgeInvincibleTimer = 0;
		bool  isJustDodge        = false;

		// �̊�
		float bodyTrunkStrength      = 5;
		float bodyTrunkStrengthRange = 3; // �����_���͈̔�

		float hitDamage = 0;
		bool isHitDamage = false;
		bool isInvincibleInvalidDamage = false;

		Vector3 flyVec = { 0,0,0 };
		bool isFlying = false;
	};

	// �O���v��(ImGui�Ƃ�)�ł̂ݕϓ�����l
	struct ConstantStatus {
		float maxHp = 1000;

		float walkSpeed      = 5;
		float shiftDashTimer = 0.4f;
		float dashSpeed      = 2.4f;
		float dashDeadZone   = 0.7f;

		// ���
		float dodgePower      = 24;
		float dodgeTime       = 1.5f;
		float dodgeLowestTime = 0.42f;
		float dodgeInvincibleTime = 0.5f;
		float justDodgeTime   = 0.21f;
		float justDodgeInvincibleTime = 0.3f;

		// �U��
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

	// ���̓f�[�^�擾
	void Input();
	void InputAttack();
	// ���s�ړ��ʌv�Z
	void CalcWalkVelocity();
	// ����ړ��ʌv�Z
	void CalcRunVelocity();
	// ���
	void CalcDodgeVelocity();
	// �U��
	void CalcAttackVelocity();
	void CalcJustDodge();

	// �_���[�W
	void HitDamaged(float damage,bool invincibleInvalid = false,  bool flying = false, Vector3 vec = {0,0,0});

	// RootAnimation
	void CalcRootAnimationVelocity();

	// �ړ����Ă�������Ɍ���
	void Turn();
	// �ړ�
	void Move();

	// �G�Ƃ̏Փˏ���
	void CollisionVsEnemy();

	void SetCamera(CameraBase* camera) { this->camera = camera; }

	// �������G�t�F�N�g
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
	//float             moveSpeed = 3.0f; // �v���C���[���P�b�Ԃɉ�����������x
	float             rot       = 720;  // �v���C���[���P�b�Ԃɉ�]����p�x

	// ���̓f�[�^�ۊǗp
	using inputData = std::variant<bool, int, float, DirectX::XMFLOAT2>;
	std::map<std::string, inputData> inputMap;

#ifdef _DEBUG
	std::map<std::string, inputData> debug;
#endif

	// --- �J�����̃|�C���^ ---
	CameraBase* camera;

	ColorFilter* colorFilter;

	// �X�e�[�^�X
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
	// �`��J���[
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pbrPS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> wboitPbrPS;
	DirectX::XMFLOAT4 drawColor = { 1,1,1,1 };
	Vector4 hitColor = { 0.0f, 0.0f, 1.0f, 1.0f };
};
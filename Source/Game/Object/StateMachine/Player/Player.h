#pragma once

#include <variant>
#include "../StateMachine.h"
#include "../../../../../Library/Resource/Model/Animator.h"
#include "../../Source/Game/Object/Base/AnimatedObject.h"

#include "../../Library/3D/CameraBase.h"


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
		float hp        = 10;
		float strength  = 1;
		float moveSpeed = 7;
		float attackCount = 0;
		float attackTimer = 0;
		float dodgeTimer = 0;
		float notAcceptTimer = 0;
	};

	// �O���v��(ImGui�Ƃ�)�ł̂ݕϓ�����l
	struct ConstantStatus {
		float maxHp = 10;

		float shiftDashTimer = 0.4f;
		float walkSpeed = 5;
		float dashSpeed = 2.4f;
		float dashDeadZone = 0.7f;

		float dodgePower = 24;
		float dodgeLowestTime = 0.5f;
		float dodgeTime = 1.5f;

		float maxAttackCombo = 4;
		float attackReceptionTime = 0.1f;
		float notAcceptTime = 0.1f;
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

	// RootAnimation
	void CalcRootAnimationVelocity();

	// �ړ����Ă�������Ɍ���
	void Turn();
	// �ړ�
	void Move();

	// �G�Ƃ̏Փˏ���
	void CollisionVsEnemy();

	void SetCamera(CameraBase* camera) { this->camera = camera; }

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

	Animator animator;

	// �X�e�[�^�X
	AbilityStatus ability;
	ConstantStatus constant;

public:
	StateMachine<Player>* GetStateMachine() { return stateMachine.get(); }

	DirectX::XMFLOAT3 GetVelocity() { return velocity; }
	void              SetVelocity(DirectX::XMFLOAT3 velocity) { this->velocity = velocity; }


	template <typename T>
	const T& GetInputMap(const std::string& str)
	{
		return std::get<T>(inputMap[str]);
	}

	void SetInputMap(const std::string& str, const inputData& data) { inputMap[str] = data; }

	AbilityStatus& AStatus() { return ability; }
	ConstantStatus& CStatus() { return constant; }
	Animator& GetAnimator() { return animator; }
};

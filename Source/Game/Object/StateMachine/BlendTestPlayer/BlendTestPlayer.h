#pragma once
#include <variant>
#include "../StateMachine.h"
#include "../../Base/AnimatedObject.h"

enum class BlendTestPlayerAnimNum
{
	Idle,
	Run,
	Walk,
};

class BlendTestPlayer : public AnimatedObject
{
public:
	BlendTestPlayer(const char* filePath);
	~BlendTestPlayer(){}

	void Update() override;
	void Render(bool isShadow = false) override;
	void DrawDebugImGui(int number);


	// ���̓f�[�^�擾
	void Input();
	// �ړ��ʌv�Z
	void CalcVelocity();
	// �ړ����Ă�������Ɍ���
	void Turn();
	// �ړ�
	void Move();

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
	};

private:
	std::unique_ptr<StateMachine<BlendTestPlayer>> stateMachine;

	DirectX::XMFLOAT3 velocity = { 0, 0, 0 };
	float             moveSpeed = 0.5f; // �v���C���[���P�b�Ԃɉ�����������x
	float             maxMoveSpeed = 0.4f; // �v���C���[���P�b�Ԃɉ�������ő�����x
	float             rot = 720;  // �v���C���[���P�b�Ԃɉ�]����p�x
	float			  friction = 0.5f;


	// ���̓f�[�^�ۊǗp
	using inputData = std::variant<bool, int, float, DirectX::XMFLOAT2>;
	std::map<std::string, inputData> inputMap;

public:
	StateMachine<BlendTestPlayer>* GetStateMachine() { return stateMachine.get(); }

	DirectX::XMFLOAT3 GetVelocity() { return velocity; }
	void              SetVelocity(DirectX::XMFLOAT3 velocity) { this->velocity = velocity; }

	float GetMaxMoveSpeed() { return maxMoveSpeed; }

	template <typename T>
	T GetInputMap(std::string str)
	{
		return std::get<T>(inputMap[str]);
	}

};
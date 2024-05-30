#pragma once

#include <variant>
#include "../StateMachine.h"
#include "../../Source/Game/Object/Base/AnimatedObject.h"


enum class PlayerAnimNum
{
	Attack,
    Drink,
    Idle,
    Walk
};

class Player : public AnimatedObject
{
public:
    Player(const char* filePath);
    ~Player() override {}

    void Update() override;
    void Render(bool isShadow = false) override;
    void DrawDebugImGui(int number);


    // ���̓f�[�^�擾
    void Input();
    // �ړ��ʌv�Z
    void CalcMoveVelocity();
    // �ړ����Ă�������Ɍ���
    void Turn();

public:
    enum class State
    {
	    Normal,
    };
    enum class Normal
    {
	    Idle,
        Walk,
    };
private:
    std::unique_ptr<StateMachine<Player>> stateMachine;

    DirectX::XMFLOAT3 velocity = { 0,0,0 };
    float moveSpeed = 3.0f;     // �v���C���[���P�b�Ԃɉ�����������x
    float rot = 720;            // �v���C���[���P�b�Ԃɉ�]����p�x

    // ���̓f�[�^�ۊǗp
    using inputData = std::variant<bool, int, float, DirectX::XMFLOAT2>;
    std::map<std::string, inputData> inputMap;

public:
    StateMachine<Player>* GetStateMachine() { return stateMachine.get(); }

    DirectX::XMFLOAT3 GetVelocity() { return velocity; }
    void SetVelocity(DirectX::XMFLOAT3 velocity) { this->velocity = velocity; }


	template<typename T>
    T GetInputMap(std::string str)
	{
        return std::get<T>(inputMap[str]);
	}
};
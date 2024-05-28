#pragma once

#include <variant>

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

private:
    // ���̓f�[�^�擾
    void Input();
    // �ړ��ʌv�Z
    void CalcMoveVelocity();
    // �ړ����Ă�������Ɍ���
    void Turn();

private:
    DirectX::XMFLOAT3 velocity = { 0,0,0 };
    float moveSpeed = 3.0f;     // �v���C���[���P�b�Ԃɉ�����������x
    float rot = 360;            // �v���C���[���P�b�Ԃɉ�]����p�x

    // ���̓f�[�^�ۊǗp
    using inputData = std::variant<bool, int, float, DirectX::XMFLOAT2>;
    std::map<std::string, inputData> inputMap;
	template<typename T>
    T GetInputMap(std::string str)
	{
        return std::get<T>(inputMap[str]);
	}
};
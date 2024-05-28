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
    // 入力データ取得
    void Input();
    // 移動量計算
    void CalcMoveVelocity();
    // 移動している方向に向く
    void Turn();

private:
    DirectX::XMFLOAT3 velocity = { 0,0,0 };
    float moveSpeed = 3.0f;     // プレイヤーが１秒間に加速する加速度
    float rot = 360;            // プレイヤーが１秒間に回転する角度

    // 入力データ保管用
    using inputData = std::variant<bool, int, float, DirectX::XMFLOAT2>;
    std::map<std::string, inputData> inputMap;
	template<typename T>
    T GetInputMap(std::string str)
	{
        return std::get<T>(inputMap[str]);
	}
};
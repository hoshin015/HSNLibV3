#pragma once

#include <variant>
#include "../StateMachine.h"
#include "../../Source/Game/Object/Base/AnimatedObject.h"


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
    Player(const char* filePath);
    ~Player() override {}

    void Update() override;
    void Render(bool isShadow = false) override;
    void DrawDebugImGui(int number);


    // 入力データ取得
    void Input();
    // 歩行移動量計算
    void CalcWalkVelocity();
    // 走り移動量計算
    void CalcRunVelocity();
    // 移動している方向に向く
    void Turn();
    // 移動
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
        Attack,
        Drink,
    };
private:
    std::unique_ptr<StateMachine<Player>> stateMachine;

    DirectX::XMFLOAT3 velocity = { 0,0,0 };
    float moveSpeed = 3.0f;     // プレイヤーが１秒間に加速する加速度
    float rot = 720;            // プレイヤーが１秒間に回転する角度

    // 入力データ保管用
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
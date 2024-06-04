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


	// 入力データ取得
	void Input();
	// 移動量計算
	void CalcVelocity();
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
	};

private:
	std::unique_ptr<StateMachine<BlendTestPlayer>> stateMachine;

	DirectX::XMFLOAT3 velocity = { 0, 0, 0 };
	float             moveSpeed = 0.5f; // プレイヤーが１秒間に加速する加速度
	float             maxMoveSpeed = 0.4f; // プレイヤーが１秒間に加速する最大加速度
	float             rot = 720;  // プレイヤーが１秒間に回転する角度
	float			  friction = 0.5f;


	// 入力データ保管用
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
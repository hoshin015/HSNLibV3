#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

// 特定のクラスを

// タイマー情報
struct TimerConstants
{
	float deltaTime;
	float nowTime;
	DirectX::XMFLOAT2 pad;
};

// カメラ情報
struct CameraConstants
{
	DirectX::XMFLOAT4X4 viewProjection;
	DirectX::XMFLOAT4 cameraPosition;
};

// 平行光源情報
struct DirectionalLightData
{
	DirectX::XMFLOAT4 direction;	// 向き
	DirectX::XMFLOAT4 color;		// 色
};
// 点光源情報
struct PointLightData
{
	DirectX::XMFLOAT4 position;		// 座標
	DirectX::XMFLOAT4 color;		// 色
	float range;					// 範囲
	DirectX::XMFLOAT3 pad;
};
// 点光源の最大数
static constexpr int PointLightMax = 8;
// スポットライト情報
struct SpotLightData
{
	DirectX::XMFLOAT4 position;		// 座標
	DirectX::XMFLOAT4 direction;	// 向き
	DirectX::XMFLOAT4 color;		// 色
	float range;					// 範囲
	float innerCorn;				// インナー角度範囲
	float outerCorn;				// アウター角度範囲
	float pad;
};
// スポットライトの最大数
static constexpr int SpotLightMax = 8;

// ライト用定数
struct LightConstants
{
	DirectionalLightData directionalLightData;
	PointLightData pointLightData[PointLightMax];
	SpotLightData spotLightData[SpotLightMax];
	int pointLightCount = 0;
	int spotLightCount = 0;
	DirectX::XMFLOAT2 pad;
	DirectX::XMFLOAT4 ambientLightColor;
};


class RenderContext
{
private:
	RenderContext() {}
	~RenderContext() {}

public:
	static RenderContext& Instance()
	{
		static RenderContext instance;
		return instance;
	}

	void Initialize();

	CameraConstants cameraConstant;
	Microsoft::WRL::ComPtr<ID3D11Buffer> cameraConstantBuffer;

	LightConstants lightConstant;
	Microsoft::WRL::ComPtr<ID3D11Buffer> lightConstantBuffer;
	
	TimerConstants timerConstant;
	Microsoft::WRL::ComPtr<ID3D11Buffer> timerConstantBuffer;

};
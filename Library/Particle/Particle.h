#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include "../2D/Sprite.h"

class Particle
{
private:
	Particle();
	~Particle() {}
public:
	static Particle& Instance()
	{
		static Particle instance;
		return instance;
	}

	const int MAX_PARTICLE = 500000;
	const int THREAD_NUM_X = 16;

	// RWStructuredBuffer で GPU とやりとりする構造体データ(保存しておくべきデータみたいな)
	struct ParticleData
	{
		DirectX::XMFLOAT4 color = { 1,1,1,1 };
		DirectX::XMFLOAT3 position = { 0,0,0 };
		DirectX::XMFLOAT3 velocity = { 0,0,0 };
		float scale = 1;
		float depth = 0.0f;

		float lifeTime;				// パーティクルが生存できる時間
		bool isActive = false;		// このパーティクルが使用中かどうか
		float kind = 0;				// パーティクルの種類
	};
	
	void Initialize();
	void Update();
	void Render();
	
	// 数指定のパーティクル起動
	void Emit(int num);

	// 現在非アクティブ状態のパーティクル数を取得
	int GetPoolBufferCount();
private:
	size_t particleCount = 0;
	size_t freeParticleCount = MAX_PARTICLE;

	// 全てのパーティクルをもつバッファー
	Microsoft::WRL::ComPtr<ID3D11Buffer> particleBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particleBufferUav;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particleBufferSrv;

	// 未使用のパーティクルをもつバッファー
	Microsoft::WRL::ComPtr<ID3D11Buffer> particlePoolBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particlePoolBufferUav;

	// 未使用のバッファーから値を取得する読み取り専用バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> particlePoolReadBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometryShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> initCs;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> updateCs;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> emitCs;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

	std::unique_ptr<Sprite> sprParticle;
};
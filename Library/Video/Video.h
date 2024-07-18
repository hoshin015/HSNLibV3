#pragma once
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mferror.h>
#include <atlbase.h>
#include <d3d11.h>
#include <wrl/client.h>

#include "../Math/Vector.h"

class Video {
private:
	struct Vertex {
		DirectX::XMFLOAT3 position;
		//DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};

private:
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	ComPtr<IMFSourceReader> _sourceReader;

	ComPtr<IMFTransform>             _transform;
	ComPtr<ID3D11Texture2D>          _texture;
	ComPtr<ID3D11ShaderResourceView> _textureView;

	Vector2 _size;
	int     _moveUVPos = -1; //YUVÇÃYÇ©ÇÁUVÇ‹Ç≈ÇÃóvëfÇÃãóó£Çï€ë∂(ìÆâÊÇÃïùÇÇ©ÇØÇÈ)
	float   _frameRate = 0;

	LONGLONG _timeStamp = 0;

	float _timer;

	float _pauseTime = 0;
	bool  _isPlay    = false;
	bool  _isLoop    = false;
	bool  _isEmpty   = true;

	inline static ComPtr<ID3D11Buffer> _vertexBuffer;
	inline static ComPtr<ID3D11InputLayout> _inputLayout;
	inline static ComPtr<ID3D11VertexShader> _vertexShader;
	inline static ComPtr<ID3D11PixelShader> _pixelShader;

private:
	void InitializeVideo(ID3D11Device* device);

public:
	Video() = default;
	Video(ID3D11Device* device, const wchar_t* filePath) { LoadFile(device, filePath); }

	static void Initialize(ID3D11Device* device);
	static void Finalize();
	static void CreateCommonBuffer(ID3D11Device* device);

	void LoadFile(ID3D11Device* device, const wchar_t* filePath);

	void Update(ID3D11DeviceContext* deviceContext);

	bool Play(bool loop = false);
	void Pause();
	void Stop();

	void SeekPosition(LONGLONG seekTime/*10É s(1/10000000)Ç≈Ç¢ÇÍÇÈ*/);

	void Render(ID3D11DeviceContext* deviceContext, DirectX::XMFLOAT2 dPos, DirectX::XMFLOAT2 dSize,
		DirectX::XMFLOAT4 color, float angle, DirectX::XMFLOAT2 sPos, DirectX::XMFLOAT2 sSize);
	void Render(ID3D11DeviceContext* deviceContext, DirectX::XMFLOAT2 dPos, DirectX::XMFLOAT2 dSize, DirectX::XMFLOAT4 color = { 1,1,1,1 }) {
		Render(deviceContext, dPos, dSize, color, 0, { 0,0 }, _size.vec_);
	}
	//void Render(ID3D11DeviceContext* deviceContext);

	const ID3D11Texture2D*          GetTexture() const { return _texture.Get(); }
	const ID3D11ShaderResourceView* GetResourceView() const { return _textureView.Get(); }

	const Vector2&  GetSourceSize() const { return _size; }
	const float&    GetSourceFrameRate() const { return _frameRate; }
	const LONGLONG& GetTimeStamp() const { return _timeStamp; }
	float           GetTimeStampSec() const { return _timeStamp * 0.0000001f; }
};

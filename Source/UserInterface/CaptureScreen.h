#pragma once

#include <memory>

#include "../../Library/Graphics/FrameBuffer.h"

class CaptureScreen
{
private:
	CaptureScreen();
	~CaptureScreen() {}

	static constexpr size_t MAX_CAPTURE = 5;
	static constexpr size_t CAPTURE_FREQUENCY = 1000; // 1/1000でキャプチャ

public:
	static CaptureScreen& Instance()
	{
		static CaptureScreen instance;
		return instance;
	}

	void Initialize();
	bool BeginCapture();
	void EndCapture();
	void DrawDebugGui();

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetRandomTexture();

	struct Capture
	{
		std::unique_ptr<FrameBuffer> frameBuffer;
		bool captured;
	};
	Capture captures[MAX_CAPTURE];
	size_t currentCaptureIndex;
	bool capture;
};


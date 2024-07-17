#pragma once

#include <memory>

#include "../../Library/Graphics/FrameBuffer.h"

class CaptureScreen
{
private:
	CaptureScreen();
	~CaptureScreen() {}

	static constexpr size_t MAX_CAPTURE = 5;
	static constexpr size_t CAPTURE_FREQUENCY = 1000; // 1/1000‚ÅƒLƒƒƒvƒ`ƒƒ

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

	std::unique_ptr<FrameBuffer> frameBuffers[MAX_CAPTURE];
	size_t currentCaptureIndex;
	bool debugCapture;
};


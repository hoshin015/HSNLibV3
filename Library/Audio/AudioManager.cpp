#include "AudioManager.h"
#include "../ErrorLogger.h"

// ����������
void AudioManager::Initialize()
{
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// �t���O����
	DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_Default;

#if _DEBUG
	eflags |= DirectX::AudioEngine_Debug;
#endif

	// audioEngine ����
	audioEngine = std::make_unique<DirectX::AudioEngine>(eflags);


	LoadMusic(static_cast<int>(MUSIC_LABEL::WEAPON), L"Data/Audio/Impact.wav");
}


#include "AudioManager.h"
#include "../ErrorLogger.h"
#include "../../External/ImGui/imgui.h"

// èâä˙âªèàóù
void AudioManager::Initialize()
{
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// ÉtÉâÉOê∂ê¨
	DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_Default;

#if _DEBUG
	eflags |= DirectX::AudioEngine_Debug;
#endif

	// audioEngine ê∂ê¨
	audioEngine = std::make_unique<DirectX::AudioEngine>(eflags);


	LoadMusic(MUSIC_LABEL::WEAPON, L"Data/Audio/Impact.wav");
	LoadMusic(MUSIC_LABEL::SELECT_MOVE, L"Data/Audio/SelectMove.wav");
	LoadMusic(MUSIC_LABEL::BATTLE1, L"Data/Audio/Battle1.wav");
	LoadMusic(MUSIC_LABEL::BATTLE2, L"Data/Audio/Battle2.wav");
	LoadMusic(MUSIC_LABEL::Fire1, L"Data/Audio/Fire1.wav");
	LoadMusic(MUSIC_LABEL::Breath, L"Data/Audio/Breath.wav");
	LoadMusic(MUSIC_LABEL::MonsterRoar1, L"Data/Audio/MonsterRoar1.wav");
	LoadMusic(MUSIC_LABEL::NovaNoise1, L"Data/Audio/NovaNoise1.wav");
	LoadMusic(MUSIC_LABEL::NovaNoise2, L"Data/Audio/NovaNoise2.wav");
	LoadMusic(MUSIC_LABEL::Lightning1, L"Data/Audio/Lightning/Lightning1.wav");
	LoadMusic(MUSIC_LABEL::Lightning2, L"Data/Audio/Lightning/Lightning2.wav");
	LoadMusic(MUSIC_LABEL::Lightning3, L"Data/Audio/Lightning/Lightning3.wav");
	LoadMusic(MUSIC_LABEL::Lightning4, L"Data/Audio/Lightning/Lightning4.wav");
	LoadMusic(MUSIC_LABEL::Lightning5, L"Data/Audio/Lightning/Lightning5.wav");
	LoadMusic(MUSIC_LABEL::Lightning6, L"Data/Audio/Lightning/Lightning6.wav");
	LoadMusic(MUSIC_LABEL::Lightning7, L"Data/Audio/Lightning/Lightning7.wav");
	LoadMusic(MUSIC_LABEL::Lightning8, L"Data/Audio/Lightning/Lightning8.wav");
	LoadMusic(MUSIC_LABEL::Lightning9, L"Data/Audio/Lightning/Lightning9.wav");
	LoadMusic(MUSIC_LABEL::Lightning10, L"Data/Audio/Lightning/Lightning10.wav");
}


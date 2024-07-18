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
	LoadMusic(MUSIC_LABEL::Lightning1, L"Data/Audio/Lightning/Lightning11.wav");
	LoadMusic(MUSIC_LABEL::Lightning2, L"Data/Audio/Lightning/Lightning12.wav");
	LoadMusic(MUSIC_LABEL::Lightning3, L"Data/Audio/Lightning/Lightning13.wav");
	LoadMusic(MUSIC_LABEL::Lightning4, L"Data/Audio/Lightning/Lightning14.wav");
	LoadMusic(MUSIC_LABEL::Lightning5, L"Data/Audio/Lightning/Lightning15.wav");
	LoadMusic(MUSIC_LABEL::Lightning6, L"Data/Audio/Lightning/Lightning16.wav");
	LoadMusic(MUSIC_LABEL::Lightning7, L"Data/Audio/Lightning/Lightning17.wav");
	LoadMusic(MUSIC_LABEL::Lightning8, L"Data/Audio/Lightning/Lightning18.wav");
	LoadMusic(MUSIC_LABEL::Lightning9, L"Data/Audio/Lightning/Lightning19.wav");
	LoadMusic(MUSIC_LABEL::Lightning10, L"Data/Audio/Lightning/Lightning20.wav");
	LoadMusic(MUSIC_LABEL::SLASH, L"Data/Audio/Slash.wav");
	LoadMusic(MUSIC_LABEL::DODGE, L"Data/Audio/Dodge.wav");
	LoadMusic(MUSIC_LABEL::TITLE, L"Data/Audio/Title.wav");
	LoadMusic(MUSIC_LABEL::LOOSE, L"Data/Audio/Loose.wav");
	LoadMusic(MUSIC_LABEL::WIN, L"Data/Audio/Win.wav");
	LoadMusic(MUSIC_LABEL::DAMAGE, L"Data/Audio/Damage.wav");
	LoadMusic(MUSIC_LABEL::SKILL, L"Data/Audio/Skill.wav");
}


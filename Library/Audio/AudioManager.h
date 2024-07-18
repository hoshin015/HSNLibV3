#pragma once
#include <Windows.h>
#include <Audio.h>


//====================================
#define USE_AUDIO 1
//====================================

enum class MUSIC_LABEL
{
	BGM_TEST = 0,
	WEAPON,
	SELECT_MOVE,
	BATTLE1,
	BATTLE2,
	Fire1,
	MonsterRoar1,

	Breath,

	NovaNoise1,
	NovaNoise2,

	Lightning1,
	Lightning2,
	Lightning3,
	Lightning4,
	Lightning5,
	Lightning6,
	Lightning7,
	Lightning8,
	Lightning9,
	Lightning10,
	END,
};

class AudioManager
{
private:
	AudioManager() {}
	~AudioManager() {}

public:
	static AudioManager& Instance()
	{
		static AudioManager instance;
		return instance;
	}

private:
	// AudioEngine
	std::unique_ptr<DirectX::AudioEngine> audioEngine;

	static const int MUSIC_FILE_MAX = 32;
	static const int XWB_FILE_MAX = 16;

	// Music
	struct Music
	{
		std::unique_ptr<DirectX::SoundEffect>			music = nullptr;
		std::unique_ptr<DirectX::SoundEffectInstance>	musicInst = nullptr;
		float											volume = 1.0f;
	} musicData[MUSIC_FILE_MAX];

public:
	// ����������
	void Initialize();

	// ���y�̓ǂݍ���
	void LoadMusic(MUSIC_LABEL trackNo, const wchar_t* waveFilename, float volume = 1.0f)
	{
		if (musicData[static_cast<int>(trackNo)].musicInst) musicData[static_cast<int>(trackNo)].musicInst.reset();
		musicData[static_cast<int>(trackNo)].music.reset(new DirectX::SoundEffect(audioEngine.get(), waveFilename));
		musicData[static_cast<int>(trackNo)].volume = volume;
	}

	// ���y�̉��
	void UnLoadMusic(MUSIC_LABEL trackNo)
	{
		if (musicData[static_cast<int>(trackNo)].musicInst)  musicData[static_cast<int>(trackNo)].musicInst.reset();
		if (musicData[static_cast<int>(trackNo)].music)  musicData[static_cast<int>(trackNo)].music.reset(nullptr);
	}


	// ���y�Đ�
	void PlayMusic(MUSIC_LABEL trackNo, bool isLoop = false)
	{
#if USE_AUDIO
		if (!musicData[static_cast<int>(trackNo)].music) return;
		musicData[static_cast<int>(trackNo)].musicInst = musicData[static_cast<int>(trackNo)].music->CreateInstance();
		musicData[static_cast<int>(trackNo)].musicInst->Play(isLoop);
		musicData[static_cast<int>(trackNo)].musicInst->SetVolume(musicData[static_cast<int>(trackNo)].volume);
#endif
	}

	// ���y��~
	void StopMusic(MUSIC_LABEL trackNo)
	{
		if (!musicData[static_cast<int>(trackNo)].musicInst) return;
		musicData[static_cast<int>(trackNo)].musicInst->Stop();
	}

	// ���y�|�[�Y
	void PauseMusic(MUSIC_LABEL trackNo)
	{
		if (!musicData[static_cast<int>(trackNo)].musicInst) return;
		musicData[static_cast<int>(trackNo)].musicInst->Pause();
	}

	// ���y���W���[��
	void ResumeMusic(MUSIC_LABEL trackNo)
	{
		if (!musicData[static_cast<int>(trackNo)].musicInst) return;
		musicData[static_cast<int>(trackNo)].musicInst->Resume();
	}

	// ���y�{�����[���ݒ�
	void SetMusicVolume(MUSIC_LABEL trackNo, float volume)
	{
		if (!musicData[static_cast<int>(trackNo)].musicInst) return;
		musicData[static_cast<int>(trackNo)].musicInst->SetVolume(volume);
		musicData[static_cast<int>(trackNo)].volume = volume;
	}

	// ���y��Ԏ擾
	DirectX::SoundState GetSoundState(MUSIC_LABEL trackNo)
	{
		return musicData[static_cast<int>(trackNo)].musicInst->GetState();
	}

	// ���y���[�v��Ԏ擾
	bool IsLoopMusic(MUSIC_LABEL trackNo)
	{
		return musicData[static_cast<int>(trackNo)].musicInst->IsLooped();
	}

	// ���y�g�p�����ǂ���
	bool IsInUseMusic(MUSIC_LABEL trackNo)
	{
		return musicData[static_cast<int>(trackNo)].music->IsInUse();
	}
};
#pragma once
#include <Windows.h>
#include <Audio.h>


enum class MUSIC_LABEL
{
	BGM_TEST = 0,
	WEAPON,
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
	void LoadMusic(int trackNo, const wchar_t* waveFilename, float volume = 1.0f)
	{
		if (musicData[trackNo].musicInst) musicData[trackNo].musicInst.reset();
		musicData[trackNo].music.reset(new DirectX::SoundEffect(audioEngine.get(), waveFilename));
		musicData[trackNo].volume = volume;
	}

	// ���y�̉��
	void UnLoadMusic(int trackNo)
	{
		if (musicData[trackNo].musicInst)  musicData[trackNo].musicInst.reset();
		if (musicData[trackNo].music)  musicData[trackNo].music.reset(nullptr);
	}

	// ���y�Đ�
	void PlayMusic(int trackNo, bool isLoop = false)
	{
		if (!musicData[trackNo].music) return;
		musicData[trackNo].musicInst = musicData[trackNo].music->CreateInstance();
		musicData[trackNo].musicInst->Play(isLoop);
		musicData[trackNo].musicInst->SetVolume(musicData[trackNo].volume);
	}

	// ���y��~
	void StopMusic(int trackNo)
	{
		if (!musicData[trackNo].musicInst) return;
		musicData[trackNo].musicInst->Stop();
	}

	// ���y�|�[�Y
	void PauseMusic(int trackNo)
	{
		if (!musicData[trackNo].musicInst) return;
		musicData[trackNo].musicInst->Pause();
	}

	// ���y���W���[��
	void ResumeMusic(int trackNo)
	{
		if (!musicData[trackNo].musicInst) return;
		musicData[trackNo].musicInst->Resume();
	}

	// ���y�{�����[���ݒ�
	void SetMusicVolume(int trackNo, int volume)
	{
		if (!musicData[trackNo].musicInst) return;
		musicData[trackNo].musicInst->SetVolume(volume);
		musicData[trackNo].volume = volume;
	}

	// ���y��Ԏ擾
	DirectX::SoundState GetSoundState(int trackNo)
	{
		return musicData[trackNo].musicInst->GetState();
	}

	// ���y���[�v��Ԏ擾
	bool IsLoopMusic(int trackNo)
	{
		return musicData[trackNo].musicInst->IsLooped();
	}

	// ���y�g�p�����ǂ���
	bool IsInUseMusic(int trackNo)
	{
		return musicData[trackNo].music->IsInUse();
	}
};
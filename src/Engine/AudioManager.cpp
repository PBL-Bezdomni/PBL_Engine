#include "AudioManager.h"
#include <iostream>
#include "Loader.h"

#include <string>

int AudioManager::Initialize()
{
	ma_result result;
	m_AudioEngine = std::make_unique<ma_engine>();
	result = ma_engine_init(NULL, m_AudioEngine.get());
	if (result != MA_SUCCESS) {
		return result;  // Failed to initialize the engine.
	}
	// ma_engine_play_sound(m_AudioEngine.get(), (Loader::RelativePath() + "res/audio/pipe.mp3").c_str(), NULL);
	return MA_SUCCESS;
}

AudioManager::~AudioManager()
{
	if (m_AudioEngine)
	{
		ma_engine_uninit(m_AudioEngine.get());
	}
}

void AudioManager::PlaySound(const std::string& path)
{
	if (!m_AudioEngine) return;

	std::string full = Loader::RelativePath() + path;
	ma_result result = ma_engine_play_sound(m_AudioEngine.get(), full.c_str(), NULL);
	if (result != MA_SUCCESS)
	{
		std::cout << "ERROR: Failed to play sound: " << full << std::endl;
	}
}

void AudioManager::PlayLoop(const std::string& path)
{
	if (!m_AudioEngine) return;

	// Stop existing
	StopLoop();

	std::string full = Loader::RelativePath() + path;
	m_BackgroundSound = std::make_unique<ma_sound>();
	ma_result result = ma_sound_init_from_file(m_AudioEngine.get(), full.c_str(), MA_SOUND_FLAG_STREAM, NULL, NULL, m_BackgroundSound.get());
	if (result != MA_SUCCESS)
	{
		std::cout << "ERROR: Failed to init background sound: " << full << std::endl;
		m_BackgroundSound.reset();
		return;
	}

	ma_sound_set_looping(m_BackgroundSound.get(), MA_TRUE);
	ma_sound_start(m_BackgroundSound.get());
}

void AudioManager::StopLoop()
{
	if (m_BackgroundSound)
	{
		ma_sound_stop(m_BackgroundSound.get());
		ma_sound_uninit(m_BackgroundSound.get());
		m_BackgroundSound.reset();
	}
}

#pragma once
#include "miniaudio.h"
#include <memory>
#include <string>

class AudioManager
{
public:
	AudioManager() = default;
	~AudioManager();
	int Initialize();

	void PlaySound(const std::string& path, float volume);

	// Play a sound in loop (background music). Call StopLoop() to stop.
	void PlayLoop(const std::string& path);
	void StopLoop();
private:
	std::unique_ptr<ma_engine> m_AudioEngine;
	std::unique_ptr<ma_sound> m_BackgroundSound;
};

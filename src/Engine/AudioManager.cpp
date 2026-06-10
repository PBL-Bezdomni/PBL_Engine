#include "AudioManager.h"

#include "Loader.h"
#include <spdlog/spdlog.h>

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

void AudioManager::PlaySound(const std::string& path)
{
	if (!m_AudioEngine) return;

	std::string full = Loader::RelativePath() + path;
	ma_result result = ma_engine_play_sound(m_AudioEngine.get(), full.c_str(), NULL);
	if (result != MA_SUCCESS)
	{
		spdlog::error("Failed to play sound: {}", full);
	}
}

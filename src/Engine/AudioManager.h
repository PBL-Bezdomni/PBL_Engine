#pragma once
#include "miniaudio.h"
#include <memory>
#include <string>

class AudioManager
{
public:
	AudioManager() = default;
	int Initialize();

	void PlaySound(const std::string& path);
private:
	std::unique_ptr<ma_engine> m_AudioEngine;
};

#pragma once
#include "miniaudio.h"
#include <memory>

class AudioManager
{
public:
	AudioManager() = default;
	int Initialize();
private:
	std::unique_ptr<ma_engine> m_AudioEngine;
};

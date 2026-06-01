#include "Loader.h"
#include <filesystem>
#include "spdlog/spdlog.h"

Loader Loader::m_Instance;

void Loader::Initialize()
{
	m_Instance.TestLoad();
}

string Loader::RelativePath()
{
	return m_Instance.m_RelativePath;
}


void Loader::TestLoad()
{
	spdlog::info("Current path: {}",
	std::filesystem::current_path().string());
	int width, height, nrChannels;
	unsigned char* data = stbi_load((VS_BUILD_RELATIVE_PATH + TEST_PATH).c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		m_RelativePath = VS_BUILD_RELATIVE_PATH;
		spdlog::info("Loader established relative path");
	}
	else
	{
		data = stbi_load((EXE_BUILD_RELATIVE_PATH + TEST_PATH).c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			m_RelativePath = EXE_BUILD_RELATIVE_PATH;
			spdlog::info("Loader established relative path");
		}
		else
		{
			spdlog::error("Loader couldn't establish relative path");
		}
	}
	stbi_image_free(data);
}

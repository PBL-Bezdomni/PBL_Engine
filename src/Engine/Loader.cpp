#include "Loader.h"
#include <filesystem>
#include <iostream>

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
	std::cout << "Current path: " << std::filesystem::current_path().string() << std::endl;
	int width, height, nrChannels;
	unsigned char* data = stbi_load((VS_BUILD_RELATIVE_PATH + TEST_PATH).c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		m_RelativePath = VS_BUILD_RELATIVE_PATH;
		std::cout << "Loader established relative path\n";
	}
	else
	{
		data = stbi_load((EXE_BUILD_RELATIVE_PATH + TEST_PATH).c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			m_RelativePath = EXE_BUILD_RELATIVE_PATH;
			std::cout << "Loader established relative path\n";
		}
		else
		{
			std::cout << "ERROR: Loader couldn't establish relative path\n";
		}
	}
	stbi_image_free(data);
}

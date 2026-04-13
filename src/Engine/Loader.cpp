#include "Loader.h"

#include "spdlog/spdlog.h"

Loader::Loader()
{
	TestLoad();
}

void Loader::TestLoad()
{
	int width, height, nrChannels;
	unsigned char* data = stbi_load((VS_BUILD_RELATIVE_PATH + TEST_PATH).c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		RelativePath = VS_BUILD_RELATIVE_PATH;
	}
	else
	{
		data = stbi_load((EXE_BUILD_RELATIVE_PATH + TEST_PATH).c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			RelativePath = EXE_BUILD_RELATIVE_PATH;
		}
		else
		{
			spdlog::debug("Loader couldn't establish relative path");
		}
	}
}

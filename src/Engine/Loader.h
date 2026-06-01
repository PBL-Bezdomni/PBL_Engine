#pragma once

#include <string>
#include "stb_image.h"

using namespace std;

class Loader
{
private:
	static Loader m_Instance;
	
	const char* TEST_PATH = "res/textures/stone.jpg";
	const string VS_BUILD_RELATIVE_PATH = "../../";
	const string EXE_BUILD_RELATIVE_PATH = "../";

	string m_RelativePath;
	
	void TestLoad();
public:
	static string RelativePath();
	static void Initialize();
	Loader() = default;
};

#pragma once

#include <string>
#include "stb_image.h"

using namespace std;

class Loader
{
public:
	string RelativePath;
	
	Loader();
	// Loader operator=(const Loader&) const
	// {
	// 	return Loader();
	// }

private:
	const char* TEST_PATH = "res/textures/stone.jpg";
	const string VS_BUILD_RELATIVE_PATH = "../../";
	const string EXE_BUILD_RELATIVE_PATH = "../../../";

	void TestLoad();
};

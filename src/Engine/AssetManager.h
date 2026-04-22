#pragma once
#include <string>
#include <unordered_map>
#include <memory>

#include "AssetLoader.h"
#include "Shader.h"
// #include "Model.h"

class Model;


using namespace std;
class AssetManager
{
private:
	AssetLoader& m_AssetLoader;
	std::unordered_map<string, shared_ptr<Model>> m_LoadedModels;
public:
	AssetManager() = default;
	
	shared_ptr<Model> GetModel(Shader& shader, const char* path, unsigned int instancing = 1, vector<glm::mat4> instanceMatrix = {});
};

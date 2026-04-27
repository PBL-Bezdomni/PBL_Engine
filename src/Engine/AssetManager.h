#pragma once
#include <string>
#include <unordered_map>
#include <memory>

#include "Shader.h"

class Model;

using namespace std;
class AssetManager
{
private:
	unordered_map<string, shared_ptr<Model>> m_LoadedModels;
	list<shared_ptr<Shader>> m_Shaders;
public:
	AssetManager() = default;
	~AssetManager();
	void Initialize();
	
	shared_ptr<Shader> GetShader(const char* vertexPath, const char* fragmentPath);
	shared_ptr<Model> GetModel(Shader& shader, const char* path, unsigned int instancing = 1, vector<glm::mat4> instanceMatrix = {});
};

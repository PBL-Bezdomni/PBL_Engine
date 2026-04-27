#include "AssetManager.h"

#include "Loader.h"
#include "Model.h"

AssetManager::~AssetManager()
{
	for (auto const& i : m_Shaders)
	{
		i->DeleteProgram();
	}
}

void AssetManager::Initialize()
{
	// TODO Make some default shaders?
}

shared_ptr<Shader> AssetManager::GetShader(const char* vertexPath, const char* fragmentPath)
{
	// Check if string starts with relative path.
	// Just to make sure, in case something that passes path with relative path is still somewhere in code, this will still work.
	string relativePath = Loader::RelativePath();
	string vPath = vertexPath;
	string fPath = fragmentPath;
	
	if (vPath.rfind(relativePath, 0) != 0)
	{
		vPath = relativePath + vertexPath;
	}
	if (fPath.rfind(relativePath, 0) != 0)
	{
		fPath = relativePath + fragmentPath;
	}
	
	m_Shaders.push_back(make_shared<Shader>(vPath.c_str(), fPath.c_str()));
	return m_Shaders.back();	
}

shared_ptr<Model> AssetManager::GetModel(Shader& shader, const char* path, unsigned int instancing, vector<glm::mat4> instanceMatrix)
{
	std::string key(path);

	auto it = m_LoadedModels.find(key);
	if (it != m_LoadedModels.end())
	{
		return it->second;
	}
	
	Model model = Model(shader, path, instancing, instanceMatrix);
	m_LoadedModels[key] = make_shared<Model>(model);
	return m_LoadedModels[key];
}


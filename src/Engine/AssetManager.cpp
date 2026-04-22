#include "AssetManager.h"
#include "Model.h"

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
}

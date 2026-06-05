#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include "Texture.h"
#include "Shader.h"

class Model;

using namespace std;
class AssetManager
{
private:
	unordered_map<string, shared_ptr<Model>> m_LoadedModels;
	unordered_map<string, shared_ptr<Texture>> m_LoadedTextures;
	list<shared_ptr<Shader>> m_Shaders;

	string GetPathWithRelativePrefix(const char* cPath);
public:
	shared_ptr<Shader> BasicShader;
	shared_ptr<Shader> SkyboxShader;
	shared_ptr<Shader> UIShader;
	shared_ptr<Shader> TextShader;
	shared_ptr<Shader> UISliderShader;
	shared_ptr<Shader> LineShader;
	shared_ptr<Shader> LightSourceShader;
	shared_ptr<Shader> SimpleDepthShader;
	shared_ptr<Shader> PieChartShader;
	shared_ptr<Shader> ProgressBarShader;
	
	AssetManager() = default;
	~AssetManager();
	void Initialize();
	
	shared_ptr<Shader> GetShader(const char* vertexPath, const char* fragmentPath);
	void AddComputeShader(shared_ptr<Shader> shader, const char* computePath);
	shared_ptr<Model> GetModel(Shader& shader, const char* path, unsigned int instancing = 1, vector<glm::mat4> instanceMatrix = {});
	shared_ptr<Texture> GetTexture(const char* path, const char* type = "texture_diffuse");
	shared_ptr<Model> GetSphereModel();
};

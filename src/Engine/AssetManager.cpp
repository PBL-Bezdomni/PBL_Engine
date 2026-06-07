#include "AssetManager.h"

#include <filesystem>

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
	BasicShader = GetShader("res/shaders/basic.vert", "res/shaders/basic.frag");
	UIShader = GetShader("res/shaders/UIShader.vert", "res/shaders/UIShader.frag");
	UISliderShader = GetShader("res/shaders/UIShader.vert", "res/shaders/UISlider.frag");
	SkyboxShader = GetShader("res/shaders/cubemap.vert", "res/shaders/cubemap.frag");
	LineShader = GetShader("res/shaders/line.vert", "res/shaders/line.frag");
	LightSourceShader = GetShader("res/shaders/lightsource.vert", "res/shaders/lightsource.frag");
	TextShader = GetShader("res/shaders/text.vert", "res/shaders/text.frag");
	SimpleDepthShader = GetShader("res/shaders/simpleDepthShader.vert", "res/shaders/simpleDepthShader.frag");
	PieChartShader = GetShader("res/shaders/PieChart.vert", "res/shaders/PieChart.frag");
	ProgressBarShader = GetShader("res/shaders/ProgressBar.vert", "res/shaders/ProgressBar.frag");
	WorldUIShader = GetShader("res/shaders/worldUIShader.vert", "res/shaders/worldUIShader.frag");
}

shared_ptr<Shader> AssetManager::GetShader(const char* vertexPath, const char* fragmentPath)
{
	// Check if string starts with relative path.
	// Just to make sure, in case something that passes path with relative path is still somewhere in code, this wil]l still work.
	string vPath = GetPathWithRelativePrefix(vertexPath);
	string fPath = GetPathWithRelativePrefix(fragmentPath);
	string relativePath = Loader::RelativePath();
	
	m_Shaders.push_back(make_shared<Shader>(vPath.c_str(), fPath.c_str()));
	return m_Shaders.back();	
}

shared_ptr<Shader> AssetManager::GetComputeShader(const char* computePath)
{
	// Check if string starts with relative path.
	// Just to make sure, in case something that passes path with relative path is still somewhere in code, this wil]l still work.
	string cPath = GetPathWithRelativePrefix(computePath);
	string relativePath = Loader::RelativePath();

	m_Shaders.push_back(make_shared<Shader>(cPath.c_str()));
	return m_Shaders.back();
}

shared_ptr<Model> AssetManager::GetModel(Shader& shader, const char* path, unsigned int instancing, vector<glm::mat4> instanceMatrix)
{
	string mPath = GetPathWithRelativePrefix(path);
	std::string key(mPath);

	auto it = m_LoadedModels.find(key);
	if (it != m_LoadedModels.end())
	{
		spdlog::info("Model from {} already was loaded", mPath);
		return it->second;
	}
	
	Model model = Model(shader, mPath.c_str(), instancing, instanceMatrix);
	m_LoadedModels[key] = make_shared<Model>(model);
	return m_LoadedModels[key];
}

shared_ptr<Texture> AssetManager::GetTexture(const char* path, const char* type)
{
	string tPath = GetPathWithRelativePrefix(path);

	std::string key(tPath);

	auto it = m_LoadedTextures.find(key);
	if (it != m_LoadedTextures.end())
	{
		return it->second;
	}

	shared_ptr<Texture> tex = make_shared<Texture>();
	tex->Path = path;
	tex->Type = type;
	filesystem::path filePath(path);
	tex->FileName = filePath.filename().string();
	glGenTextures(1, &tex->ID);
	glBindTexture(GL_TEXTURE_2D, tex->ID);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//stbi_set_flip_vertically_on_load(1);
	// load and generate the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load(tPath.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format = GL_RGB;
		if (nrChannels == 4)
		{
			format = GL_RGBA;
		}
		spdlog::info("Loaded Texture: {} with {} channels", tPath, nrChannels);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		spdlog::error("Failed to load texture at path {}", tPath);
		std::filesystem::path p = tPath;
		spdlog::error("Absolute path: {}", std::filesystem::absolute(p).string());
		spdlog::error(stbi_failure_reason());
	}
	stbi_image_free(data);

	m_LoadedTextures[key] = tex;
	return m_LoadedTextures[key];
}

shared_ptr<Model> AssetManager::GetSphereModel()
{
	return GetModel(*BasicShader, "res/models/sphere/sphere.obj");
}

void AssetManager::SetShadersViewProjection(glm::mat4 view, glm::mat4 projection)
{
	for (shared_ptr<Shader> shader : m_Shaders)
	{
		shader->Use();
		shader->SetMat4("view", view);
		shader->SetMat4("projection", projection);
	}	
}

string AssetManager::GetPathWithRelativePrefix(const char* cPath)
{
	string relativePath = Loader::RelativePath();
	string path = cPath;

	return relativePath + cPath;
	
	if (path.rfind(relativePath, 0) != 0)
	{
		path = relativePath + cPath;
	}

	return path;
}

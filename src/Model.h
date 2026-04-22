#pragma once
#include "Shader.h"
#include "Mesh.h"
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <spdlog/spdlog.h>
#include "Engine/Components/Component.h"

using namespace std;

class Model : public Component
{
public:
    Model();
    Model(Shader& shader);
    Model(Shader& shader, vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, unsigned int instancing = 1, vector<glm::mat4> instanceMatrix = {});
    Model(Shader& shader, const char* path, unsigned int instancing = 1, vector<glm::mat4> instanceMatrix = {});
    void Draw(glm::mat4 modelMatrix);
    void AssignTexture(Texture tex);
    void UpdateVertices(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
    void UpdateInstanceMatrix(glm::vec3 transform, glm::vec3 rotation, glm::vec3 scale, int index, Shader& shader, bool saveNew);
    vector<Mesh> Meshes;
    unsigned int Instancing;
    vector<glm::mat4> InstanceMatrix;
    void ScaleTexture(float scale);
    void ReassignShader(Shader& shader);
private:
    // model data
    string m_FileDirectory;
    vector<Texture> m_TexturesLoaded;
    Shader m_Shader = Shader();

    void LoadModel(string path);
    void ProcessNode(aiNode* node, const aiScene* scene);
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
    vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};
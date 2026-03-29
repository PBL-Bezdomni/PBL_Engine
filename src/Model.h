#pragma once
#include "Shader.h"
#include "Mesh.h"
#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <spdlog/spdlog.h>

using namespace std;

class Model
{
public:
    Model();
    Model(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, unsigned int instancing = 1, vector<glm::mat4> instanceMatrix = {});
    Model(const char* path, unsigned int instancing = 1, vector<glm::mat4> instanceMatrix = {});
    void Draw(Shader& shader);
    void AssignTexture(Texture tex);
    void UpdateVertices(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
    vector<Mesh> Meshes;
    unsigned int Instancing;
    vector<glm::mat4> InstanceMatrix;
    void ScaleTexture(float scale);
private:
    // model data
    string m_FileDirectory;
    vector<Texture> m_TexturesLoaded;

    void LoadModel(string path);
    void ProcessNode(aiNode* node, const aiScene* scene);
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
    vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};
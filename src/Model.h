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
    //void DrawInstanced(Shader& shader, int amount);
    void AssignTexture(Texture tex);
    void UpdateVertices(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
    vector<Mesh> meshes;
    unsigned int instancing;
    vector<glm::mat4> instanceMatrix;
    void ScaleTexture(float scale);
private:
    // model data
    string directory;
    vector<Texture> textures_loaded;

    void loadModel(string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};
#pragma once

#include <glm/glm.hpp>
#include "Shader.h"
#include <string>
#include <vector>
#include "Texture.h"
using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;

    int m_BoneIDs[MAX_BONE_INFLUENCE];
    float m_Weights[MAX_BONE_INFLUENCE];
};

class Mesh
{
public:
    // mesh data
    vector<Vertex>       Vertices;
    vector<unsigned int> Indices;
    vector<Texture>      Textures;

    unsigned int Instancing;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, unsigned int instancing = 1, vector<glm::mat4> instanceMatrix = {});
    void Draw(Shader& shader);
    void RefreshInstanceMatrix(Shader& shader, vector<glm::mat4> newInstance);
    void ScaleTexture(float scale);

private:
    //  render data
    unsigned int m_VAO, m_VBO, m_EBO, m_InstanceVBO;
    
    void SetupMesh();
    void SetupInstanceMatrix(vector<glm::mat4> instanceMatrix);
};


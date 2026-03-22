#pragma once

#include <glm/glm.hpp>
#include "Shader.h"
#include <string>
#include <vector>
using namespace std;

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh
{
public:
    // mesh data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;

    unsigned int instancing;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, unsigned int instancing = 1, vector<glm::mat4> instanceMatrix = {});
    void Draw(Shader& shader);
    void RefreshInstanceMatrix(Shader& shader, vector<glm::mat4> newInstance);
    void ScaleTexture(float scale);
private:
    //  render data
    unsigned int VAO, VBO, EBO, instanceVBO;

    void setupMesh();
    void setupInstanceMatrix(vector<glm::mat4> instanceMatrix);
};


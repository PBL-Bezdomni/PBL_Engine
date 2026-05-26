#include "Mesh.h"

#include "Model.h"


Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, unsigned int instancing, vector<glm::mat4> instanceMatrix)
{
    this->Vertices = vertices;
    this->Indices = indices;
    this->Textures = textures;
    this->Instancing = instancing;

    SetupMesh();
    if (instancing > 1)
    {
        SetupInstanceMatrix(instanceMatrix);
    }
}

void Mesh::SetupMesh()
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int),
        &Indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::SetupInstanceMatrix(vector<glm::mat4> instanceMatrix)
{
    glBindVertexArray(m_VAO);
    glGenBuffers(1, &m_InstanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
    glBufferData(GL_ARRAY_BUFFER, instanceMatrix.size() * sizeof(glm::mat4), &instanceMatrix[0], GL_STATIC_DRAW);

    
    //glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    // vertex attributes
    std::size_t vec4Size = sizeof(glm::vec4);
    std::size_t mat4Size = sizeof(glm::mat4);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)(1 * vec4Size));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)(2 * vec4Size));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)(3 * vec4Size));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::RefreshInstanceMatrix(Shader& shader, vector<glm::mat4> newInstance)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
    glBufferData(GL_ARRAY_BUFFER, newInstance.size() * sizeof(glm::mat4), &newInstance[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::Draw(Shader& shader)
{
    shader.Use();
    glBindVertexArray(m_VAO);

    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;

    for (unsigned int i = 0; i < Textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        string number;
        string name = Textures[i].Type;
        TextureTypeNames tn;
        if (name == tn.DIFFUSE)
            number = std::to_string(diffuseNr++);
        else if (name == tn.SPECULAR)
            number = std::to_string(specularNr++);
        else if (name == tn.NORMAL)
            number = std::to_string(normalNr++);

        // shader.SetInt(("material." + name + number).c_str(), i);
        shader.SetInt((name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, Textures[i].ID);
    }
    glActiveTexture(GL_TEXTURE0);
    // draw mesh

    if (Instancing > 1)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
        glDrawElementsInstanced(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0, Instancing);
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);
    }
    
    glBindVertexArray(0);
}

void Mesh::ScaleTexture(float scale)
{
    for (int i = 0; i < Vertices.size(); i++)
    {
        Vertices[i].TexCoords *= scale;
    }
    SetupMesh();
}

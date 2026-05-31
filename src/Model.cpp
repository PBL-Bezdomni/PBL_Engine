#include "Model.h"

#include "EngineConsts.h"
#include "GameObject.h"
#include "assimp/postprocess.h"
#include "Engine/Engine.h"

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

Model::Model()
{
}

Model::Model(Shader& shader)
{
    this->m_Shader = shader;
    m_MainCamera = Engine::GetInstance().GetGameManager().GetSceneManager().GetMainCamera();
}

Model::Model(Shader& shader, vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, unsigned int instancing, vector<glm::mat4> instanceMatrix)
{
    this->m_Shader = shader;
    this->Instancing = instancing;
    this->InstanceMatrix = instanceMatrix;
    Meshes.push_back(Mesh(vertices, indices, textures, instancing, instanceMatrix));
    m_MainCamera = Engine::GetInstance().GetGameManager().GetSceneManager().GetMainCamera();
}

Model::Model(Shader& shader, const char* path, unsigned int instancing, vector<glm::mat4> instanceMatrix)
{
    this->m_Shader = shader;
    this->Instancing = instancing;
    this->InstanceMatrix = instanceMatrix;
    LoadModel(path);
    m_MainCamera = Engine::GetInstance().GetGameManager().GetSceneManager().GetMainCamera();
}

// Reducted TODO remove
void Model::UpdateVertices(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
{
    if (Meshes.size() > 0)
    {
        Meshes[0] = Mesh(vertices, indices, textures, Instancing, InstanceMatrix);
    }
    else
    {
        Meshes.push_back(Mesh(vertices, indices, textures, Instancing, InstanceMatrix));
    }
}

// TODO make at least applicable for other types of textures. 
void Model::AssignTexture(Texture tex)
{
    if (Meshes.size() > 0)
    {
        if (Meshes[0].Textures.size() > 0)
        {
            Meshes[0].Textures[0] = tex;
        }
        else
        {
            Meshes[0].Textures.push_back(tex);
        }
    }
}

void Model::AssignNormal(Texture tex)
{
    string typeName = "texture_normal";
    tex.Type = typeName;
    for (unsigned int i = 0; i < Meshes.size(); i++)
    {
        for (int j = 0; j < Meshes[i].Textures.size(); j++)
        {
            if (Meshes[i].Textures[j].Type == typeName)
            {
                Meshes[i].Textures[j] = tex;
                return;
            }
        }
        Meshes[i].Textures.push_back(tex);
    }
}

void Model::Draw(glm::mat4 modelMatrix, Shader* shader)
{
    Shader shaderToUse;
    if (shader) {
        shaderToUse = *shader;
    }
    else {
        shaderToUse = m_Shader;
    }
    shaderToUse.Use();
    shaderToUse.SetMat4("model", modelMatrix);
    
    for (unsigned int i = 0; i < Meshes.size(); i++)
    {
        if (m_CheckFrustum)
        {
            if (m_FrustumVolume.IsOnFrustrum(m_MainCamera->GetFrustum(), *GetOwner()->transform))
            {
                Meshes[i].Draw(shaderToUse);
            }
        }
        else
        {
            Meshes[i].Draw(shaderToUse);
        }
    }
}

void Model::LoadModel(string path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        spdlog::error("ERROR::ASSIMP::");
        spdlog::error(importer.GetErrorString());
        return;
    }

    filesystem::path filePath(path);
    m_FileDirectory = path.substr(0, path.find_last_of('/'));
    m_FileName = filePath.filename().string();

    ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        Meshes.push_back(ProcessMesh(mesh, scene));
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        // process vertex positions, normals and texture coordinates
        glm::vec3 vector;
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // normals
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;

        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
        {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }
    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // process material
    if(mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        vector<Texture> diffuseMaps = LoadMaterialTextures(material,
            aiTextureType_DIFFUSE, EngineConsts::DIFFUSE);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        vector<Texture> specularMaps = LoadMaterialTextures(material,
            aiTextureType_SPECULAR, EngineConsts::SPECULAR);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        vector<Texture> normalMaps = LoadMaterialTextures(material,
            aiTextureType_NORMALS, EngineConsts::NORMAL);
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    }

    return Mesh(vertices, indices, textures, Instancing, InstanceMatrix);
}

vector<Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
    vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (unsigned int j = 0; j < m_TexturesLoaded.size(); j++)
        {
            if (std::strcmp(m_TexturesLoaded[j].Path.data(), str.C_Str()) == 0)
            {
                textures.push_back(m_TexturesLoaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip)
        {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.ID = TextureFromFile(str.C_Str(), m_FileDirectory);
            texture.Type = typeName;
            texture.Path = str.C_Str();
            // filesystem::path filePath(str.C_Str());
            // texture.FileName = filePath.filename().string();
            // Don't override path, so debug want save texture as separate load.
            texture.FileName = "";
            textures.push_back(texture);
            m_TexturesLoaded.push_back(texture); // add to loaded textures
        }
    }
    return textures;
}

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// Reducted TODO remove
void Model::ScaleTexture(float scale)
{
    for (int i = 0; i < Meshes.size(); i++)
    {
        Meshes[i].ScaleTexture(scale);
    }
}

void Model::ReassignShader(Shader& shader)
{
    this->m_Shader = shader;
}

void Model::UpdateInstanceMatrix(glm::vec3 transform, glm::vec3 rotation, glm::vec3 scale, int index, Shader& shader, bool saveNew)
{
    if (index >= 0 && Instancing > 1 && InstanceMatrix.size() > index)
    {
        vector<glm::mat4> newInstanceMatrix;
        for (int i = 0; i < InstanceMatrix.size(); i++)
        {
            newInstanceMatrix.push_back(InstanceMatrix[i]);
        }
        newInstanceMatrix[index] = glm::translate(newInstanceMatrix[index], transform);
        newInstanceMatrix[index] = glm::rotate(newInstanceMatrix[index], glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        newInstanceMatrix[index] = glm::rotate(newInstanceMatrix[index], glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        newInstanceMatrix[index] = glm::rotate(newInstanceMatrix[index], glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        newInstanceMatrix[index] = glm::scale(newInstanceMatrix[index], scale);

        if (saveNew)
        {
            InstanceMatrix[index] = newInstanceMatrix[index];
        }

        for (int i = 0; i < Meshes.size(); i++)
        {
            Meshes[i].RefreshInstanceMatrix(shader, newInstanceMatrix);
        }
    }
}

string Model::GetFileName()
{
    return m_FileName;
}

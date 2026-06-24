#include "Model.h"

#include "EngineConsts.h"
#include "GameObject.h"
#include "assimp/postprocess.h"
#include "Engine/Engine.h"
#include <Engine/Animation/Animator.h>

unsigned int TextureFromFile(const char* path, const string& directory, const aiScene* scene, bool gamma = false);

static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
{
    glm::mat4 to;
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}

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
    tex.Type = EngineConsts::NORMAL;
    m_HasNormal = true;
    for (unsigned int i = 0; i < Meshes.size(); i++)
    {
        for (int j = 0; j < Meshes[i].Textures.size(); j++)
        {
            if (Meshes[i].Textures[j].Type == EngineConsts::NORMAL)
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
    if (shader != nullptr) {
        shaderToUse = *shader;
    }
    else {
        shaderToUse = m_Shader;
    }
    shaderToUse.Use();
    shaderToUse.SetFloat("u_Time", (float)glfwGetTime());
    if (GetOwner()->m_isWater) {
        shaderToUse.SetBool("isWater", true);
        
    }
    if (GetOwner()->m_isGrassWinded) {
        shaderToUse.SetBool("isGrassWind", true);
    }
    if (GetOwner()->m_isBambooWinded) {
        shaderToUse.SetBool("isBambooWind", true);
    }
    if (Instancing > 1)
    {
        shaderToUse.SetBool("u_IsInstanced", true);
    }

    shaderToUse.SetMat4("model", modelMatrix);

    shaderToUse.SetBool("useNormal", m_HasNormal);

    if (m_IsHighlighted) {
        shaderToUse.SetBool("u_IsHighlighted", true);
        shaderToUse.SetVec3("u_GlowColor", glm::vec3(0.2f, 0.2f, 0.2f));
    }
    else {
        shaderToUse.SetBool("u_IsHighlighted", false);
        shaderToUse.SetVec3("u_GlowColor", glm::vec3(0.0f));
    }

    
    Animator* animator = m_Owner->GetComponent<Animator>();
    if (animator != nullptr)
    {
        auto& transforms = animator->GetFinalBoneMatrices();
        for (int i = 0; i < transforms.size(); ++i)
        {
            shaderToUse.SetMat4("bonesMatrices[" + std::to_string(i) + "]", transforms[i]);
        }
    }

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

    if (m_IsHighlighted) {
        shaderToUse.SetBool("u_IsHighlighted", false);
    }
    shaderToUse.SetBool("u_IsInstanced", false);
    shaderToUse.SetBool("isWater", false);
    shaderToUse.SetBool("isGrassWind", false);
    shaderToUse.SetBool("isBambooWind", false);
}

void Model::LoadModel(string path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_LimitBoneWeights);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::";
        std::cout << importer.GetErrorString() << std::endl;
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
        SetVertexBoneDataToDefault(vertex);
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
    ExtractBoneWeightForVertices(vertices, mesh, scene);
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
            aiTextureType_DIFFUSE, EngineConsts::DIFFUSE, scene);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        vector<Texture> specularMaps = LoadMaterialTextures(material,
            aiTextureType_SPECULAR, EngineConsts::SPECULAR, scene);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        vector<Texture> normalMaps = LoadMaterialTextures(material,
            aiTextureType_NORMALS, EngineConsts::NORMAL, scene);
        if (normalMaps.size() > 0)
        {
            m_HasNormal = true;
        }
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    }
    return Mesh(vertices, indices, textures, Instancing, InstanceMatrix);
}

vector<Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName, const aiScene* scene)
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
            texture.ID = TextureFromFile(str.C_Str(), m_FileDirectory, scene);
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

unsigned int TextureFromFile(const char* path, const string& directory, const aiScene* scene, bool gamma)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    //unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    unsigned char* data = nullptr;

    if (scene != nullptr)
    {
        const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(path);
        if (embeddedTexture)
        {
            if (embeddedTexture->mHeight == 0)
            {
                data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(embeddedTexture->pcData), embeddedTexture->mWidth, &width, &height, &nrComponents, 0);
            }
            else
            {
                data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(embeddedTexture->pcData), embeddedTexture->mWidth * embeddedTexture->mHeight * 4, &width, &height, &nrComponents, 0);
            }
        }
    }

    if (!data)
    {
       data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    }

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

void Model::SetVertexBoneDataToDefault(Vertex& vertex)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        vertex.m_BoneIDs[i] = -1;
        vertex.m_Weights[i] = 0.0f;
    }
}

void Model::ExtractBoneWeightForVertices(vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
    //std::cout << "mesh loading: " << mesh->mName.C_Str(), << " bone found: " << mesh->mNumBones << std::endl;
    for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
    {
        int boneID = -1;
        string boneName = mesh->mBones[boneIndex]->mName.C_Str();

        if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end())
        {
            BoneInfo newBoneInfo;
            newBoneInfo.id = m_BoneCount;
            newBoneInfo.offset = ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
            m_BoneInfoMap[boneName] = newBoneInfo;
            boneID = m_BoneCount;
            m_BoneCount++;
            // std::cout << "loaded bone: " << boneName << " (ID: " << boneID << ")\n";
        }
        else
        {
            boneID = m_BoneInfoMap[boneName].id;
        }

        assert(boneID != -1);

        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
        {
            int vertexID = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            assert(vertexID <= vertices.size());

            for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
            {
                if (vertices[vertexID].m_BoneIDs[i] < 0)
                {
                    vertices[vertexID].m_Weights[i] = weight;
                    vertices[vertexID].m_BoneIDs[i] = boneID;
                    break;
                }
            }
        }
    }
}
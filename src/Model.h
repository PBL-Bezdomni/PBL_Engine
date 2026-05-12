#pragma once
#include "Shader.h"
#include "Mesh.h"
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <spdlog/spdlog.h>

#include "Camera.h"
#include "Engine/Components/Component.h"
#include "Engine/Components/Transform.h"

using namespace std;

// class Camera;

struct Volume
{
    virtual bool IsOnFrustrum(const Frustum& camFrustum, const Transform& modelTransform) const = 0;
    virtual bool IsOnOrForwardPlane(const Plane& plane) const = 0;
};

struct Sphere : Volume
{
    glm::vec3 Center { 0.f, 0.f, 0.f };
    float radius { 0.f };

    Sphere() = default;
    Sphere(glm::vec3 centr, float r)
    {
        Center = centr;
        radius = r;
    }

    bool IsOnOrForwardPlane(const Plane& plane) const final
    {
        return plane.GetSignedDistanceToPlane(Center) > -radius;
    }
    
    bool IsOnFrustrum(const Frustum& camFrustum, const Transform& modelTransform) const final
    {
        //Get global scale is computed by doing the magnitude of
        //X, Y and Z model matrix's column.
        const glm::vec3 globalScale = modelTransform.GetGlobalScale();

        //Get our global center with process it with the global model matrix of our transform
        const glm::vec3 globalCenter{ modelTransform.ModelMatrix * glm::vec4(Center, 1.f) };

        //To wrap correctly our shape, we need the maximum scale scalar.
        const float maxScale = std::max(std::max(globalScale.x, globalScale.y), globalScale.z);

        //Max scale is assuming for the diameter. So, we need the half to apply it to our radius
        Sphere globalSphere(globalCenter, radius * (maxScale * 0.5f));

        //Check Firstly the result that have the most chance
        //to faillure to avoid to call all functions.
        return (globalSphere.IsOnOrForwardPlane(camFrustum.LeftFace) &&
            globalSphere.IsOnOrForwardPlane(camFrustum.RightFace) &&
            globalSphere.IsOnOrForwardPlane(camFrustum.FarFace) &&
            globalSphere.IsOnOrForwardPlane(camFrustum.NearFace) &&
            globalSphere.IsOnOrForwardPlane(camFrustum.TopFace) &&
            globalSphere.IsOnOrForwardPlane(camFrustum.BottomFace));
    }
};

struct AABB : Volume
{
    glm::vec3 Center { 0.0f, 0.0f, 0.0f };
    glm::vec3 Extents { 0.0f, 0.0f, 0.0f };

    AABB() = default;

    AABB(const glm::vec3& min, const glm::vec3& max) : Volume() 
    {
        Center = (max + min) * 0.5f;
        Extents = { max.x - Center.x, max.y - Center.y, max.z - Center.z }; 
    }

    AABB(const glm::vec3& inCenter, float iI, float iJ, float iK) : Volume()
    {
        Center = inCenter;
        Extents = { iI, iJ, iK };
    }

    bool IsOnOrForwardPlane(const Plane& plane) const final
    {
        // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
        const float r = Extents.x * std::abs(plane.normal.x) +
                Extents.y * std::abs(plane.normal.y) + Extents.z * std::abs(plane.normal.z);

        return -r <= plane.GetSignedDistanceToPlane(Center);
    }

    bool IsOnFrustrum(const Frustum& camFrustum, const Transform& modelTransform) const final
    {
        //Get global scale thanks to our transform
        const glm::vec3 globalCenter{ modelTransform.ModelMatrix * glm::vec4(Center, 1.f) };

        // Scaled orientation
        const glm::vec3 right = modelTransform.GetRight() * Extents.x;
        const glm::vec3 up = modelTransform.GetUp() * Extents.y;
        const glm::vec3 forward = modelTransform.GetForward() * Extents.z;

        const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
            std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
            std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

        const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
            std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
            std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

        const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
            std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
            std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

        //We not need to divise scale because it's based on the half extention of the AABB
        const AABB globalAABB(globalCenter, newIi, newIj, newIk);

        return (globalAABB.IsOnOrForwardPlane(camFrustum.LeftFace) &&
            globalAABB.IsOnOrForwardPlane(camFrustum.RightFace) &&
            globalAABB.IsOnOrForwardPlane(camFrustum.TopFace) &&
            globalAABB.IsOnOrForwardPlane(camFrustum.BottomFace) &&
            globalAABB.IsOnOrForwardPlane(camFrustum.NearFace) &&
            globalAABB.IsOnOrForwardPlane(camFrustum.FarFace));
    }
};

class Model : public Component
{
public:
    Model();
    Model(Shader& shader);
    Model(Shader& shader, vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, unsigned int instancing = 1, vector<glm::mat4> instanceMatrix = {});
    Model(Shader& shader, const char* path, unsigned int instancing = 1, vector<glm::mat4> instanceMatrix = {});
    void Draw(glm::mat4 modelMatrix);
    void AssignTexture(Texture tex);
    void AssignNormal(Texture tex);
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
    AABB m_FrustumVolume;
    shared_ptr<Camera> m_MainCamera;

    void LoadModel(string path);
    void ProcessNode(aiNode* node, const aiScene* scene);
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
    vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);

    bool m_CheckFrustum = false;
};
#include "Entity.h"

Entity::Entity()
{
    m_Shader = Shader();
    instancing = 0;
}

Entity::Entity(Shader& shader)
{
    m_Shader = shader;
    instancing = 0;
}

Entity::Entity(Shader& shader, vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, unsigned int instancing, vector<glm::mat4> instanceMatrix) : Model(vertices, indices, textures, instancing, instanceMatrix)
{
    m_Shader = shader;
}

Entity::Entity(Shader& shader, const char* path, unsigned int instancing, vector<glm::mat4> instanceMatrix) : Model(path, instancing, instanceMatrix)
{
    m_Shader = shader;
}

void Entity::addChild(Entity* child) 
{
	children.push_back(child);
	children.back()->parent = this;
}

glm::mat4 Entity::getLocalModelMatrix()
{
    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(transform.eulerRot.x), glm::vec3(1.0f, 0.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(transform.eulerRot.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(transform.eulerRot.z), glm::vec3(0.0f, 0.0f, 1.0f));

    // Y * X * Z
    /*const glm::mat4 rotationMatrix = transformY * transformX * transformZ;*/

    // translation * rotation * scale (also know as TRS matrix)
    return glm::translate(glm::mat4(1.0f), transform.pos) * rotationMatrix * glm::scale(glm::mat4(1.0f), transform.scale);//glm::translate(glm::mat4(1.0f), transform.pos) * 
}

glm::vec3 Entity::GetWorldPosition()
{
    return glm::vec3(transform.modelMatrix[3]);
}

void Entity::updateSelfAndChild()
{
    if (parent != nullptr)
    {
        transform.modelMatrix = parent->transform.modelMatrix * getLocalModelMatrix();
    }
    else
    {
        transform.modelMatrix = getLocalModelMatrix();
    }

    for (auto&& child : children)
    {
        child->updateSelfAndChild();
    }
}

void Entity::updateSelfAndChildInstanceMatrix(glm::vec3 transform, glm::vec3 rotation, glm::vec3 scale, int index, Shader& shader, bool saveNew)
{
    if (index >= 0 && instancing > 1 && instanceMatrix.size() > index)
    {
        vector<glm::mat4> newInstanceMatrix;
        for (int i = 0; i < instanceMatrix.size(); i++)
        {
            newInstanceMatrix.push_back(instanceMatrix[i]);
        }
        newInstanceMatrix[index] = glm::translate(newInstanceMatrix[index], transform);
        newInstanceMatrix[index] = glm::rotate(newInstanceMatrix[index], glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        newInstanceMatrix[index] = glm::rotate(newInstanceMatrix[index], glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        newInstanceMatrix[index] = glm::rotate(newInstanceMatrix[index], glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        newInstanceMatrix[index] = glm::scale(newInstanceMatrix[index], scale);

        if (saveNew)
        {
            instanceMatrix[index] = newInstanceMatrix[index];
        }

        for (int i = 0; i < meshes.size(); i++)
        {
            meshes[i].RefreshInstanceMatrix(shader, newInstanceMatrix);
        }

        for (auto&& child : children)
        {
            child->updateSelfAndChildInstanceMatrix(transform, rotation, scale, index, shader, saveNew);
        }
    }
}

void Entity::drawSelfAndChild()
{
    if (RenderModel)
    {
        m_Shader.use();
        m_Shader.setMat4("model", transform.modelMatrix);

        Draw(m_Shader);

        for (auto&& child : children)
        {
            child->drawSelfAndChild();
        }
    }
}

void Entity::drawSelf()
{
    if (RenderModel)
    {
        m_Shader.use();
        m_Shader.setMat4("model", transform.modelMatrix);
        Draw(m_Shader);
    }
}

void Entity::ReassignShader(Shader& shader)
{
    m_Shader = shader;
}

void Entity::SetModelRenderBool(bool render)
{
    RenderModel = render;
}
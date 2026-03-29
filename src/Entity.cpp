#include "Entity.h"

Entity::Entity()
{
    m_Shader = Shader();
    Instancing = 0;
}

Entity::Entity(Shader& shader)
{
    m_Shader = shader;
    Instancing = 0;
}

Entity::Entity(Shader& shader, vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, unsigned int instancing, vector<glm::mat4> instanceMatrix) : Model(vertices, indices, textures, instancing, instanceMatrix)
{
    m_Shader = shader;
}

Entity::Entity(Shader& shader, const char* path, unsigned int instancing, vector<glm::mat4> instanceMatrix) : Model(path, instancing, instanceMatrix)
{
    m_Shader = shader;
}

void Entity::AddChild(Entity* child) 
{
	Children.push_back(child);
	Children.back()->Parent = this;
}

glm::mat4 Entity::GetLocalModelMatrix()
{
    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(transform.EulerAngles.x), glm::vec3(1.0f, 0.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(transform.EulerAngles.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(transform.EulerAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));

    // Y * X * Z
    /*const glm::mat4 rotationMatrix = transformY * transformX * transformZ;*/

    // translation * rotation * scale (also know as TRS matrix)
    return glm::translate(glm::mat4(1.0f), transform.Position) * rotationMatrix * glm::scale(glm::mat4(1.0f), transform.Scale);//glm::translate(glm::mat4(1.0f), transform.pos) * 
}

glm::vec3 Entity::GetWorldPosition()
{
    return glm::vec3(transform.ModelMatrix[3]);
}

void Entity::UpdateSelfAndChild()
{
    if (Parent != nullptr)
    {
        transform.ModelMatrix = Parent->transform.ModelMatrix * GetLocalModelMatrix();
    }
    else
    {
        transform.ModelMatrix = GetLocalModelMatrix();
    }

    for (auto&& child : Children)
    {
        child->UpdateSelfAndChild();
    }
}

void Entity::UpdateSelfAndChildInstanceMatrix(glm::vec3 transform, glm::vec3 rotation, glm::vec3 scale, int index, Shader& shader, bool saveNew)
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

        for (auto&& child : Children)
        {
            child->UpdateSelfAndChildInstanceMatrix(transform, rotation, scale, index, shader, saveNew);
        }
    }
}

void Entity::DrawSelfAndChild()
{
    if (m_IsModelRendered)
    {
        m_Shader.Use();
        m_Shader.SetMat4("model", transform.ModelMatrix);

        Draw(m_Shader);

        for (auto&& child : Children)
        {
            child->DrawSelfAndChild();
        }
    }
}

void Entity::DrawSelf()
{
    if (m_IsModelRendered)
    {
        m_Shader.Use();
        m_Shader.SetMat4("model", transform.ModelMatrix);
        Draw(m_Shader);
    }
}

void Entity::ReassignShader(Shader& shader)
{
    m_Shader = shader;
}

void Entity::SetModelRenderBool(bool render)
{
    m_IsModelRendered = render;
}
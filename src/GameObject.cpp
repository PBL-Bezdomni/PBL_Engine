#include "GameObject.h"

GameObject::GameObject()
{
    AddComponent<Transform>();
    transform = GetComponent<Transform>();
    // Instancing = 0;
}

void GameObject::AddChild(GameObject* child) 
{
	Children.push_back(child);
	Children.back()->Parent = this;
}

glm::vec3 GameObject::GetWorldPosition()
{
    return glm::vec3(transform->ModelMatrix[3]);
}

void GameObject::UpdateSelfAndChild()
{
    if (Parent != nullptr)
    {
        transform->ModelMatrix = Parent->transform->ModelMatrix * transform->GetLocalModelMatrix();
    }
    else
    {
        transform->ModelMatrix = transform->GetLocalModelMatrix();
    }

    for (auto&& child : Children)
    {
        child->UpdateSelfAndChild();
    }
}

void GameObject::UpdateSelfAndChildInstanceMatrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, int index, Shader& shader, bool saveNew)
{
    Model* model = GetComponent<Model>();
    if (model != nullptr)
    {
        if (model->Instancing > 1)
        {
            model->UpdateInstanceMatrix(position, rotation, scale, index, shader, saveNew);   
        }
    }

    for (auto&& child : Children)
    {
        child->UpdateSelfAndChildInstanceMatrix(position, rotation, scale, index, shader, saveNew);
    }
}

void GameObject::DrawSelfAndChild()
{
    if (m_IsModelRendered)
    {
        Model* model = GetComponent<Model>();
        if (model != nullptr)
        {
            model->Draw(transform->ModelMatrix);
        }

        for (auto&& child : Children)
        {
            child->DrawSelfAndChild();
        }
    }
}

void GameObject::DrawSelf()
{
    if (m_IsModelRendered)
    {
        Model* model = GetComponent<Model>();
        if (model != nullptr)
        {
            model->Draw(transform->ModelMatrix);
        }
    }
}

void GameObject::SetModelRenderBool(bool render)
{
    m_IsModelRendered = render;
}

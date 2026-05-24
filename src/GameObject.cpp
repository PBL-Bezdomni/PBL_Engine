#include "GameObject.h"

#include "Engine/Components/RigidBody.h"
#include "Engine/Light/PointLight.h"

GameObject::GameObject()
{
    AddComponent<Transform>();
    transform = GetComponent<Transform>();
    // Instancing = 0;
}

void GameObject::AddChild(GameObject* child) 
{
	m_PendingChildren.push_back(child);
	// Children.back()->Parent = this;
}

void GameObject::AddPendingChildren()
{
    for (auto&& child : m_PendingChildren)
    {
        child->Parent = this;
    }
    Children.insert(Children.end(), m_PendingChildren.begin(), m_PendingChildren.end());
    m_PendingChildren.clear();
}


void GameObject::RemoveChild(GameObject* child)
{
    for (int i = 0; i < Children.size(); i++)
    {
        if (Children[i] == child)
        {
            child->Parent = nullptr;
            Children.erase(remove(Children.begin(), Children.end(), child), Children.end());
            return;
        }
    }
}

glm::vec3 GameObject::GetWorldPosition()
{
    return glm::vec3(transform->ModelMatrix[3]);
}

void GameObject::StartSelfAndChild()
{
    for (auto& [type, vec] : m_Components)
    {
        for (auto& comp : vec)
        {
            comp->Start();
        }
    }

    for (auto&& child : Children)
    {
        child->StartSelfAndChild();
    }
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

    RigidBody* rb = GetComponent<RigidBody>();

    for (auto& [type, vec] : m_Components)
    {
        for (auto& comp : vec)
        {
            comp->Update();
        }
    }

    AddPendingChildren();

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

void GameObject::DrawSelfAndChild(Shader* shader)
{
    if (m_IsActive)
    {
        Model* model = GetComponent<Model>();
        if (model != nullptr && model->IsActive())
        {
            model->Draw(transform->ModelMatrix, shader);
        }

        for (auto&& child : Children)
        {
            child->DrawSelfAndChild(shader);
        }
    }
}

void GameObject::DrawSelf(Shader* shader)
{
    if (m_IsActive)
    {
        Model* model = GetComponent<Model>();
        if (model != nullptr && model->IsActive())
        {
            model->Draw(transform->ModelMatrix, shader);
        }
    }
}

void GameObject::SetActive(bool active)
{
    m_IsActive = active;
}

#pragma once
#include <typeindex>
#include <memory>
#include <vector>
#include <unordered_map>
#include "Model.h"
#include <glm/glm.hpp>
#include <Engine/Components/Transform.h>
#include <Engine/Components/Component.h>

using namespace std;

class GameObject
{
private:
	unordered_map<type_index, vector<unique_ptr<Component>>> m_Components;
	bool m_IsActive = true;
	vector<GameObject*> m_PendingChildren;

	void AddPendingChildren();
public:
	Transform* transform;
	string Name;
	int ID;
	GameObject();
	vector<GameObject*> Children;
	
	GameObject* Parent = nullptr;
	int ParentID;

	void UpdateSelfAndChild();
	void UpdateSelfAndChildInstanceMatrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, int index, Shader& shader, bool saveNew);
	void AddChild(GameObject* child);
	void RemoveChild(GameObject* child);
	void DrawSelfAndChild(Shader* shader);
	void DrawSelf(Shader* shader);
	glm::vec3 GetWorldPosition();
	void SetActive(bool active);
	void StartSelfAndChild();
	void AssignScript(string scriptName);

	// template methods, must be in header apparently
	template<typename T, typename... Args>
	T* AddComponent(Args&&... args)
	{
		type_index type = typeid(T);

		auto& vec = m_Components[type];

		auto component = make_unique<T>(forward<Args>(args)...);

		if (!component->AllowMultiple() && !vec.empty())
		{
			// Component already exist and only one is allowed
			return static_cast<T*>(vec.front().get());
		}

		component->SetOwner(this);
		component->Awake();

		T* ptr = component.get();
		vec.push_back(move(component));

		return ptr;
	}

	template<typename T, typename... Args>
	void RemoveComponent(Args&&... args)
	{
		type_index type = typeid(T);
		// auto& vec = m_Components[type];
		m_Components.erase(type);
	}
	
	template<typename T>
	T* GetComponent()
	{
		auto it = m_Components.find(typeid(T));
		if (it == m_Components.end() || it->second.empty())
		{
			return nullptr;
		}

		return static_cast<T*>(it->second.front().get());
	}
	
	template<typename T>
	vector<T*> GetVectorOfComponents()
	{
		vector<T*> result;

		for (auto& [type, components] : m_Components)
		{
			for (auto& component : components)
			{
				if (auto* casted = dynamic_cast<T*>(component.get()))
				{
					result.push_back(casted);
				}
			}
		}

		return result;
	}

	template<typename T>
	T* GetDerivedComponent()
	{
		for (auto& pair : m_Components)
		{
			for (auto& componentPtr : pair.second)
			{
				T* target = dynamic_cast<T*>(componentPtr.get());
				if (target != nullptr)
				{
					return target;
				}
			}
		}
		return nullptr;
	}

};
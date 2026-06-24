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

struct GameObjectData
{
	GameObject* gameObject;
	string Name;
	int ID;
	string ParentName;
	int ParentID;

	string Mesh;
	string DiffuseTex;
	bool HasNormal;
	string NormalTex;
	
	float PosX;
	float PosY;
	float PosZ;

	float RotX;
	float RotY;
	float RotZ;

	float ScaX;
	float ScaY;
	float ScaZ;

	bool HasRigidBody;
	bool IsStatic;
	bool IsTrigger;

	float ColliderSizeX;
	float ColliderSizeY;
	float ColliderSizeZ;

	vector<string> Scripts;

	vector<GameObjectData> Children;

};

class GameObject
{
protected:
	bool m_PendingDestroy = false;
private:
	unordered_map<type_index, vector<unique_ptr<Component>>> m_Components;
	bool m_IsActive = true;
	vector<GameObject*> m_PendingChildren;

	void AddPendingChildren();
public:
	Transform* transform;
	string Name;
	int ID;
	bool m_isVisible = true;
	bool m_isWater = false;
	bool m_isGrassWinded = false;
	bool m_isBambooWinded = false;
	GameObject();
	vector<GameObject*> Children;
	
	GameObject* Parent = nullptr;
	int ParentID;

	void UpdateSelfAndChild();
	void UpdateSelfAndChildInstanceMatrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, int index, Shader& shader, bool saveNew);
	void AddChild(GameObject* child);
	void RemoveChild(GameObject* child);
	void DrawSelfAndChild();
	void DrawSelfAndChildFiltered(bool filter);
	void DrawSekfAndChildShadow(Shader* shader, bool drawOnlyDynamic);
	void DrawSelf(Shader* shader);
	glm::vec3 GetWorldPosition();
	void SetActive(bool active);
	bool IsActive();
	void StartSelfAndChild();
	void AssignScript(string scriptName);
	void RemoveScript(string scriptName);

	void Destroy();
	bool IsPendingDestroy() const { return m_PendingDestroy; }

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

	GameObject* GetChildByName(const string& findName);

	template<typename T>
	vector<T*> FindComponentsInChildren(bool getFirst = false)
	{
		vector<T*> result;

		for (auto& [type, components] : m_Components)
		{
			for (auto& component : components)
			{
				if (auto* casted = dynamic_cast<T*>(component.get()))
				{
					result.push_back(casted);
					if (getFirst)
					{
						return result;
					}
				}
			}
		}

		vector<T*> childrenResult;
		for (auto child : Children)
		{
			childrenResult = child->FindComponentsInChildren<T>(getFirst);
			result.insert(result.end(), childrenResult.begin(), childrenResult.end());
			if (!childrenResult.empty() && getFirst)
			{
				break;
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
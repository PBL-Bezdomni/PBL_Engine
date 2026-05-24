#include "JSONImporter.h"
#include "Engine/Components/RigidBody.h"
#include "Game/Scripts/Animal.h"
#include "Game/Scripts/MassageTable.h"
#include "AssetManager.h"
#include "Engine.h"
#include "GameObject.h"
#include "Model.h"
#include <iostream>
#include <fstream>

void JSONImporter::Initialize()
{
    m_AssetMgr = &Engine::GetInstance().GetAssetManager();
}

shared_ptr<GameObject> JSONImporter::ImportObjectFromData(nlohmann::basic_json<>& obj, GameObject* root)
{
    shared_ptr<GameObject> gameObject = make_shared<GameObject>();
    if (!gameObject)
    {
        std::cerr << "Failed to create GameObject for: " << obj.value("name", "Unnamed") << std::endl;
        return nullptr;
    }
    else
    {
        std::cerr << "Created GameObject for: " << obj.value("name", "Unnamed") << std::endl;
    }
    gameObject->Name = obj.value("name", "NewObject");
    gameObject->ID = obj.value("id", 0);
    gameObject->ParentID = obj.value("parentID", -1);

    std::string modelPath = obj.value("mesh", "");
    if (modelPath != "")
    {
        Model model = *m_AssetMgr->GetModel(*m_AssetMgr->BasicShader, (SCENE_MODELS_PATH + modelPath).c_str());
        gameObject->AddComponent<Model>(model);
        string baseTexPath = obj.value("base_color_texture", "");
        if (baseTexPath != "")
        {
            Texture baseTex = *m_AssetMgr->GetTexture((SCENE_TEXTURE_PATH + baseTexPath).c_str());
            gameObject->GetComponent<Model>()->AssignTexture(baseTex);
        }
    }

    auto pos = obj["position"];
    auto rot = obj["rotation"];
    auto sca = obj["scale"];
    gameObject->transform->Position = glm::vec3(pos[0], pos[1], pos[2]);
    gameObject->transform->Position *= TRANSFORM_MOD;
    gameObject->transform->Scale = glm::vec3(sca[0], sca[1], sca[2]);
    gameObject->transform->EulerAngles = glm::vec3(rot[0], rot[1], rot[2]);

    bool hasRB = obj.value("has_rigid_body", false);
    bool hasColl = obj.value("has_collider", false);
    if (hasRB || hasColl)
    {
        gameObject->AddComponent<RigidBody>();
        RigidBody* rb = gameObject->GetComponent<RigidBody>();
        
        auto size = obj.value("collider_size", std::vector<float>{1.0f, 1.0f, 1.0f});
        auto cPos = obj.value("collider_pos", std::vector<float>{0.0f, 0.0f, 0.0f});
        
        glm::vec3 colliderSize = glm::vec3(size[0], size[1], size[2]);
        colliderSize *= COLLIDER_MOD;
        glm::vec3 colliderOffset = glm::vec3(cPos[0], cPos[1], cPos[2]);
        
        // TODO collider offset is not used now
        rb->PrepareInit(colliderSize, !hasRB);        
    }
    
    if (obj.contains("scripts"))
    {
        for (nlohmann::basic_json<>& scriptName : obj["scripts"])
        {
            AssignScript(gameObject.get(), scriptName);
        }
    }

    return gameObject;
}

void JSONImporter::AssignGraph(vector<shared_ptr<GameObject>> gameObjects, GameObject* root)
{
    for (shared_ptr<GameObject> go : gameObjects)
    {
        if (go->ParentID == -1)
        {
            root->AddChild(go.get());
            continue;
        }

        for (shared_ptr<GameObject> parent : gameObjects)
        {
            if (parent->ID == go->ParentID)
            {
                parent->AddChild(go.get());
                break;
            }
        }
    }
}

std::vector<shared_ptr<GameObject>> JSONImporter::ImportScene(const char* fileName, GameObject* root)
{
    json data = GetData(fileName);
    std::vector<shared_ptr<GameObject>> objs;
    for (nlohmann::basic_json<>& obj : data["objects"])
    {
        objs.push_back(ImportObjectFromData(obj, root));
    }
    AssignGraph(objs, root);
    root->UpdateSelfAndChild();
    return objs;
}

std::vector<std::shared_ptr<GameObject>> JSONImporter::ImportPrefab(const char* fileName, GameObject* root)
{
    // TODO import prefab and all it's children here, similarly like in import scene.
    // If process will be exatly the same, maybe remove this method and rename the other
    vector<shared_ptr<GameObject>> objs;
    return objs;
}

GameObject* JSONImporter::FindByName(GameObject* root, const std::string& name)
{
    if (!root) return nullptr;
    if (root->Name == name) return root;
    for (auto* child : root->Children) {
        GameObject* found = FindByName(child, name);
        if (found) return found;
    }
    return nullptr;
}

json JSONImporter::GetData(const char* fileName)
{
    std::string basePath = "res/json/";
    std::string fullPath = Loader::RelativePath() + basePath + fileName + ".json";

    std::ifstream file(fullPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open JSON: " << fullPath << std::endl;
        return NULL;
    }

    json data;
    file >> data;
    return data;
}

void JSONImporter::AssignScript(GameObject* go, nlohmann::basic_json<>& scriptName)
{
    if (scriptName == "SpawnManager")
    {
        go->AddComponent<SpawnManager>();
    }
    else if (scriptName == "MassageTable")
    {
        if (!go->GetComponent<MassageTable>()) go->AddComponent<MassageTable>();
    }
}



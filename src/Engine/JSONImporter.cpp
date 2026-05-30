#include "JSONImporter.h"
#include "Engine/Components/RigidBody.h"
#include "Game/Scripts/Animal.h"
#include "Game/Scripts/MassageTable.h"
#include "AssetManager.h"
#include "Engine.h"
#include "GameObject.h"
#include "Model.h"
#include "Camera.h"
#include <iostream>
#include <fstream>
#include "DebugManager.h"
#include "Game/Scripts/Bath.h"

void JSONImporter::Initialize()
{
    m_AssetMgr = &Engine::GetInstance().GetAssetManager();
}

shared_ptr<GameObject> JSONImporter::ImportObjectFromData(nlohmann::basic_json<>& obj, GameObject* root)
{
    GameObjectNames gn;
    shared_ptr<GameObject> gameObject = make_shared<GameObject>();
    if (!gameObject)
    {
        spdlog::error("Failed to create GameObject for: " + obj.value("name", "Unnamed"));
        return nullptr;
    }
    else
    {
        spdlog::info("Created GameObject for: " + obj.value("name", "Unnamed"));
    }
    gameObject->Name = obj.value(gn.NAME, "NewObject");
    gameObject->ID = obj.value(gn.ID, 0);
    gameObject->ParentID = obj.value(gn.PARENT_ID, -1);

    std::string modelPath = obj.value(gn.MESH, "");
    if (modelPath != "")
    {
        Model model = *m_AssetMgr->GetModel(*m_AssetMgr->BasicShader, (SCENE_MODELS_PATH + modelPath).c_str());
        gameObject->AddComponent<Model>(model);
        string baseTexPath = obj.value(gn.DIFFUSE, "");
        if (baseTexPath != "")
        {
            Texture baseTex = *m_AssetMgr->GetTexture((SCENE_TEXTURE_PATH + baseTexPath).c_str());
            gameObject->GetComponent<Model>()->AssignTexture(baseTex);
        }
        string normalPath = obj.value(gn.NORMAL, "");
        bool hasNormal = obj.value(gn.HAS_NORMAL, false);
        TextureTypeNames tn;
        if (hasNormal && normalPath != "")
        {
            // TODO assign to model info that it has normal
            Texture normal = *m_AssetMgr->GetTexture(normalPath.c_str(), tn.NORMAL);
            gameObject->GetComponent<Model>()->AssignNormal(normal);
        }
    }

    auto pos = obj[gn.POSITION];
    auto rot = obj[gn.ROTATION];
    auto sca = obj[gn.SCALE];
    gameObject->transform->Position = glm::vec3(pos[0], pos[1], pos[2]);
    // gameObject->transform->Position *= TRANSFORM_MOD;
    gameObject->transform->Scale = glm::vec3(sca[0], sca[1], sca[2]);
    gameObject->transform->EulerAngles = glm::vec3(rot[0], rot[1], rot[2]);

    bool hasRB = obj.value(gn.HAS_RB, false);
    bool isStatic = obj.value(gn.IS_STATIC, false);
    bool isTrigger = obj.value(gn.IS_TRIGGER, false);
    if (hasRB)
    {
        gameObject->AddComponent<RigidBody>();
        RigidBody* rb = gameObject->GetComponent<RigidBody>();
        
        auto size = obj.value(gn.COLLIDER_SIZE, std::vector<float>{1.0f, 1.0f, 1.0f});
        
        glm::vec3 colliderSize = glm::vec3(size[0], size[1], size[2]);
        // colliderSize *= COLLIDER_MOD;
        
        // TODO collider offset is not used now
        rb->PrepareInit(colliderSize, isStatic, isTrigger);        
    }
    
    if (obj.contains(gn.SCRIPTS))
    {
        for (nlohmann::basic_json<>& scriptName : obj[gn.SCRIPTS])
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
    GameObjectNames gn;
    for (nlohmann::basic_json<>& obj : data[gn.OBJECT])
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
    std::string fullPath = Loader::RelativePath() + BASE_PATH + fileName + JSON_SUFFIX;

    std::ifstream file(fullPath);
    if (!file.is_open()) {
        spdlog::error("Failed to open JSON: " + fullPath);
        return NULL;
    }

    json data;
    file >> data;
    return data;
}

void JSONImporter::SaveData(string fileName, json data)
{
    string fullPath = Loader::RelativePath() + BASE_PATH + fileName + JSON_SUFFIX;

    std::ofstream file(fullPath);
    if (!file.is_open())
    {
        spdlog::error("Failed to create JSON: " + fullPath);
        return;
    }

    file << data.dump(4);
    file.close();
    spdlog::info("Crated file: " + fullPath);
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
    else if (scriptName == "Bath")
    {
        if (!go->GetComponent<Bath>()) go->AddComponent<Bath>();
    }
}

void JSONImporter::SaveCameraData(const char*  fileName, Camera* camera)
{
    CameraNames camNames;
    json data;
    glm::vec3 pos = camera->GetPosition();
    data[camNames.POS] = json::array({pos.x, pos.y, pos.z});
    data[camNames.PITCH] = camera->GetPitch();
    data[camNames.YAW] = camera->GetYaw();
    data[camNames.FOV] = camera->GetZoom();

    SaveData(fileName, data);
}

void JSONImporter::LoadCameraData(const char*  filename, Camera* camera)
{
    CameraNames camNames;
    json data = GetData(filename);
    if (data == NULL)
    {
        return;
    }
    auto pos = data[camNames.POS];
    glm::vec3 posVec = glm::vec3(pos[0], pos[1], pos[2]);
    float pitch = data[camNames.PITCH];
    float yaw = data[camNames.YAW];
    float fov = data[camNames.FOV];

    camera->SetPosition(posVec);
    camera->SetPitch(pitch);
    camera->SetYaw(yaw);
    camera->SetZoom(fov);
    
}

void JSONImporter::SaveSceneData(const char* fileName, GameObjectData& objData)
{
    json data;
    GameObjectNames name;
    m_GameObjectID = 0;
    SetGameObjectsID(objData);
    data[name.OBJECT].push_back(SaveGameObjectData(objData));
    SaveChildrenObjectData(data, objData);

    SaveData(fileName, data);
}

void JSONImporter::SaveChildrenObjectData(json& data, GameObjectData& objData)
{
    GameObjectNames name;
    for (GameObjectData childData : objData.Children)
    {
        data[name.OBJECT].push_back(SaveGameObjectData(childData));
        SaveChildrenObjectData(data, childData);
    }
}

nlohmann::basic_json<> JSONImporter::SaveGameObjectData(GameObjectData& objData)
{
    GameObjectNames n;
    json data;

    data[n.NAME] = objData.gameObject->Name;
    data[n.ID] = objData.gameObject->ID;
    if (objData.ParentID != -1)
    {
        data[n.PARENT_ID] = objData.gameObject->Parent->ID;
        data[n.PARENT] = objData.gameObject->Parent->Name;
    }
    else
    {
        data[n.PARENT_ID] = -1;
        data[n.PARENT] = "";
    }

    data[n.POSITION] = { objData.PosX, objData.PosY, objData.PosZ };
    data[n.ROTATION] = { objData.RotX, objData.RotY, objData.RotZ };
    data[n.SCALE] = { objData.ScaX, objData.ScaY, objData.ScaZ };
    
    data[n.MESH] = objData.Mesh;
    data[n.DIFFUSE] = objData.DiffuseTex;
    data[n.HAS_NORMAL] = objData.HasNormal;
    data[n.NORMAL] = objData.NormalTex;

    data[n.HAS_RB] = objData.HasRigidBody;
    data[n.IS_STATIC] = objData.IsStatic;
    data[n.IS_TRIGGER] = objData.IsTrigger;
    data[n.COLLIDER_SIZE] = { objData.ColliderSizeX, objData.ColliderSizeY, objData.ColliderSizeZ };

    data[n.SCRIPTS] = objData.Scripts;

    return data;
}

void JSONImporter::SetGameObjectsID(GameObjectData& objData)
{
    objData.gameObject->ID = ++m_GameObjectID;
    for (GameObjectData& child : objData.Children)
    {
        SetGameObjectsID(child);
    }
}

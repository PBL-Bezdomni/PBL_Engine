#pragma once
#include "nlohmann/json.hpp"
#include <fstream>
#include <string>
#include "AssetManager.h"
// #include "DebugManager.h"
#include "GameObject.h"

using json = nlohmann::json;

class GameObject;

struct CameraNames
{
    const char* POS = "position";
    const char* PITCH = "pitch";
    const char* YAW = "yaw";
    const char* FOV = "fov";
};

struct GameObjectNames
{
    const char* OBJECT = "objects";
    const char* NAME = "name";
    const char* PREFAB = "prefab";
    const char* ID = "id";
    const char* PARENT = "parent";
    const char* PARENT_ID = "parentID";
    const char* MESH = "mesh";
    const char* DIFFUSE = "base_color_texture";
    const char* HAS_NORMAL = "has_normal";
    const char* NORMAL = "normal_texture";
    const char* POSITION = "position";
    const char* ROTATION = "rotation";
    const char* SCALE = "scale";
    const char* HAS_RB = "has_rigid_body";
    const char* IS_STATIC = "is_static";
    const char* IS_TRIGGER = "is_trigger";
    const char* COLLIDER_SIZE = "collider_size";
    const char* SCRIPTS = "scripts";
};

struct GameObjectData;

class JSONImporter
{
public:
    void Initialize();
    std::vector<std::shared_ptr<GameObject>> ImportScene(const char* fileName, GameObject* root);
    std::vector<std::shared_ptr<GameObject>> ImportPrefab(const char* fileName, GameObject* root);
    static GameObject* FindByName(GameObject* root, const std::string& name);
    void SaveCameraData(const char* fileName, Camera* camera);
    void SaveSceneData(const char* fileName, GameObjectData& objData);
    void LoadCameraData(const char* filename, Camera* camera);

private:
    const char* BASE_PATH = "res/json/";
    const char* JSON_SUFFIX = ".json";
  
    int m_GameObjectID = 0;
    
    AssetManager* m_AssetMgr;
    
    shared_ptr<GameObject> ImportObjectFromData(nlohmann::basic_json<>& obj, GameObject* root);
    void AssignGraph(vector<shared_ptr<GameObject>> gameObjects, GameObject* root);
    json GetData(const char* fileName);
    void SaveData(string fileName, json data);
    void SaveChildrenObjectData(json& data, GameObjectData& objData);
    nlohmann::basic_json<> SaveGameObjectData(GameObjectData& objData);
    void SetGameObjectsID(GameObjectData& objData);
};
#pragma once
#include "nlohmann/json.hpp"
#include <fstream>
#include <string>

#include "AssetManager.h"
#include "GameObject.h"

using json = nlohmann::json;

class GameObject;

class JSONImporter
{
public:
    void Initialize();
    std::vector<std::shared_ptr<GameObject>> ImportScene(const char* fileName, GameObject* root);
    std::vector<std::shared_ptr<GameObject>> ImportPrefab(const char* fileName, GameObject* root);
    static GameObject* FindByName(GameObject* root, const std::string& name);

private:
    const char* SCENE_MODELS_PATH = "res/models/scene_models/";
    const char* SCENE_TEXTURE_PATH = "res/textures/scene_textures/";
    // HACK
    const float TRANSFORM_MOD = 27;
    const float COLLIDER_MOD = 12;
    
    AssetManager* m_AssetMgr;
    
    shared_ptr<GameObject> ImportObjectFromData(nlohmann::basic_json<>& obj, GameObject* root);
    void AssignGraph(vector<shared_ptr<GameObject>> gameObjects, GameObject* root);
    json GetData(const char* fileName);
    void AssignScript(GameObject* go, nlohmann::basic_json<>& scriptName);
};
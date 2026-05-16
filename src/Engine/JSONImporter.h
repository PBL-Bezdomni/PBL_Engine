#pragma once
#include "nlohmann/json.hpp"
#include <fstream>
#include <string>

class GameObject;

class JSONImporter
{
public:
    void ImportScene(GameObject* root);
    void ImportObjectFromFile(const std::string& fileName, GameObject* root);
    static GameObject* FindByName(GameObject* root, const std::string& name);
};
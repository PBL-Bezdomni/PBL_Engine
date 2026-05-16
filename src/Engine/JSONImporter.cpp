#include "JSONImporter.h"
#include "Engine/Components/RigidBody.h"
#include "Engine/Components/Animal.h"
#include "Engine/Components/MassageTable.h"
#include "AssetManager.h"
#include "Engine.h"
#include "GameObject.h"
#include "Model.h"
#include <iostream>
#include <fstream>

using json = nlohmann::json;

void JSONImporter::ImportObjectFromFile(const std::string& fileName, GameObject* root)
{
    std::string basePath = "res/json/";
    std::string fullPath = Loader::RelativePath() + basePath + fileName + ".json";

    std::ifstream file(fullPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open JSON: " << fullPath << std::endl;
        return;
    }

    json data;
    file >> data;
    Engine& engine = Engine::GetInstance();

    for (auto& obj : data["objects"])
    {
        GameObject* gameObject = new GameObject();
        if (!gameObject) {
            std::cerr << "Failed to create GameObject for: " << obj.value("name", "Unnamed") << std::endl;
            continue;
        }
        else {
            std::cerr << "Created GameObject for: " << obj.value("name", "Unnamed") << std::endl;
        }
        gameObject->Name = obj.value("name", "NewObject");

        std::string modelPath = "";
        std::string prefabType = obj.value("prefab", "");
        std::string texturePath = "";

        if (obj.contains("model_path")) {
            modelPath = obj["model_path"];
        }
        else if (prefabType == "BunnyNPC") {
            modelPath = "res/models/animals/bunny/bunny.obj";
        }
        else if (prefabType == "MassageTables") {
            modelPath = "res/models/scena_v1/table/table2.fbx";
        }
        else if (prefabType == "DryTowels") {
            modelPath = "res/models/scena_v1/for_towels/towels2.obj";
            texturePath = "res/models/scena_v1/for_towels/drewno.jpg";
        }

        if (!modelPath.empty()) {
            auto modelPtr = engine.GetAssetManager().GetModel(*engine.GetAssetManager().BasicShader, modelPath.c_str());
            if (modelPtr) {
                gameObject->AddComponent<Model>(*modelPtr);

                if (!texturePath.empty()) {
                    auto texturePtr = engine.GetAssetManager().GetTexture(texturePath.c_str());
                    if (texturePtr) {
                        gameObject->GetComponent<Model>()->AssignTexture(*texturePtr);
                    }
                }
            }
        }

        auto pos = obj["position"];
        auto rot = obj["rotation"];
        auto sca = obj["scale"];
        gameObject->transform->Position = glm::vec3(pos[0], pos[1], pos[2]);
        gameObject->transform->EulerAngles = glm::vec3(rot[0], rot[1], rot[2]);
        gameObject->transform->Scale = glm::vec3(sca[0], sca[1], sca[2]);

        root->AddChild(gameObject);
        root->UpdateSelfAndChild();

        if (obj.value("has_rigid_body", false) || obj.value("has_collider", false))
        {
            RigidBody* rb = gameObject->AddComponent<RigidBody>();

            auto size = obj.value("collider_size", std::vector<float>{1.0f, 1.0f, 1.0f});
            auto cPos = obj.value("collider_pos", std::vector<float>{0.0f, 0.0f, 0.0f});

            glm::vec3 colliderSize = glm::vec3(size[0], size[1], size[2]);
            glm::vec3 colliderOffset = glm::vec3(cPos[0], cPos[1], cPos[2]);

            rb->Init(colliderSize, obj.value("is_static", false));

            glm::vec3 worldPos = glm::vec3(gameObject->transform->ModelMatrix[3]);
            rb->Teleport(worldPos + colliderOffset);
        }

        if (obj.contains("scripts"))
        {
            for (auto& scriptName : obj["scripts"])
            {
                if (scriptName == "AnimalNeeds" || scriptName == "BunnyCharacter" || scriptName == "AnimalWander" || scriptName == "AnimalInteractions")
                {
                    if (!gameObject->GetComponent<Animal>())
                        gameObject->AddComponent<Animal>();
                }
                else if (scriptName == "MassageTable" || scriptName == "DryTowel")
                {
                    if (!gameObject->GetComponent<MassageTable>())
                        gameObject->AddComponent<MassageTable>();
                }
            }
        }
    }
}

void JSONImporter::ImportScene(GameObject* root)
{
    ImportObjectFromFile("scene", root);
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
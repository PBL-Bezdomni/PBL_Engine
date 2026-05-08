#include "JSONImporter.h"

#include "AssetManager.h"
#include "Engine.h"

using json = nlohmann::json;

void JSONImporter::ImportScene()
{
	AssetManager* assetMgr = &Engine::GetInstance().GetAssetManager();
	std::ifstream file(Loader::RelativePath() + "res/json/scene.json");
	json data;
	file >> data;

	for (auto& obj : data["objects"])
	{
		std::string name = obj["name"];
		std::string parent = obj["parent"];
		std::string mesh = obj["mesh"];

		auto pos = obj["position"];
		auto rot = obj["rotation"];
		auto sca = obj["scale"];

		GameObject* gameObject = new GameObject();
		gameObject->transform->Position = glm::vec3(pos[0], pos[1], pos[2]);
		gameObject->transform->EulerAngles = glm::vec3(rot[0], rot[1], rot[2]);
		gameObject->transform->Scale = glm::vec3(sca[0], sca[1], sca[2]);
		
	}
}

#include <Player.h>
#include "Engine/Engine.h"

Player::Player(InputManager& input, SpawnManager& spawner, Shader& shader, int deviceid)
{
	deviceID = deviceid;

	body = make_unique<GameObject>();
	AssetManager* am = &Engine::GetInstance().GetAssetManager();
	Model bodyModel = *am->GetModel(shader, "res/models/sphere/sphere.obj");
	body->AddComponent<Model>(bodyModel);

	input.subscribe(deviceID, "MoveForward", [this](float val, InputEventType type, int id) {
		if(id == deviceID)
			this->moveInput.y = val;
		});

	input.subscribe(deviceID, "MoveStrafe", [this](float val, InputEventType type, int id) {
		if (id == deviceID)
			this->moveInput.x = val;
		});
}

void Player::Update(float deltaTime)
{
	if (!body) return;


	glm::vec3 direction = glm::vec3(moveInput.x, 0.0f, moveInput.y);


	if (glm::length(direction) > 0.01f) {
		body->transform->Position += direction * speed * deltaTime;
	}
}


#include <glm/glm.hpp>
#include "Shader.h"
#include <string>
#include <vector>

#include "GameObject.h"
#include "Engine/InputManager.h"
#include "SpawnManager.h"

class Player {
public:
    GameObject* body = nullptr;
    glm::vec2 moveInput{ 0.0f };
    float speed = 10.0f;
    int deviceID;

    Player(InputManager& input, SpawnManager& spawner, Shader& shader, int deviceid) {

        deviceID = deviceid;

        body = spawner.SpawnBall(shader);

        input.subscribe(deviceID, "MoveForward", [this](float val, InputEventType type, int id) {
            if(id == deviceID)
                this->moveInput.y = val;
            });

        input.subscribe(deviceID, "MoveStrafe", [this](float val, InputEventType type, int id) {
            if (id == deviceID)
                this->moveInput.x = val;
            });
    }

    void update(float deltaTime) {
        if (!body) return;


        glm::vec3 direction = glm::vec3(moveInput.x, 0.0f, moveInput.y);


        if (glm::length(direction) > 0.01f) {
            body->transform->Position += direction * speed * deltaTime;
        }
    }
};
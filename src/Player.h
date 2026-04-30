#include <glm/glm.hpp>
#include "Shader.h"
#include <string>
#include <vector>

#include "GameObject.h"
#include "Engine/InputManager.h"
#include "SpawnManager.h"

class Player {
public:
    unique_ptr<GameObject> body = nullptr;
    glm::vec2 moveInput{ 0.0f };
    float speed = 10.0f;
    int deviceID;

    Player(InputManager& input, SpawnManager& spawner, Shader& shader, int deviceid);
    void Update(float deltaTime);
};

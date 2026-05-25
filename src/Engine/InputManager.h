#pragma once
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <map>
#include <functional>
#include <algorithm>

enum class BindingType { Button, Axis };

struct Binding {
    BindingType type;
    int code;        
    float threshold; 
};

enum class InputEventType { Started, Performed, Ended };
using ActionCallback = std::function<void(float value, InputEventType type, int deviceID)>;

struct InputAction {
    std::string name;
    std::vector<Binding> bindings;
    std::vector<ActionCallback> callbacks;

    float lastValue = 0.0f;
    bool isPressed = false;
};

struct InputName
{
    const char* MOVE_FORWARD = "MoveForward";
    const char* MOVE_STRAFE = "MoveStrafe";
    const char* ACTION = "Action";
    const char* THROW = "Throw";
};

class InputManager {
private:
    std::map<std::string, InputAction> actions;
    std::map<int, std::map<std::string, InputAction>> devices;
    float deadzone = 0.1f;

public:
    InputName InputName;

    void Initialize();
    
    void createAction(const std::string& name);

    void removeAction(const std::string& name);

    void addBinding(const std::string& actionName, Binding binding);

    void clearBindings(const std::string& actionName);

    void subscribe(int deviceID, const std::string& actionName, ActionCallback callback);

    void update();

private:
    void processStateTransition(InputAction& action, float currentValue, int deviceID);

    void trigger(InputAction& action, float value, InputEventType type, int deviceID);
};
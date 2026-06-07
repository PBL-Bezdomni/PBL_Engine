#include "InputManager.h"

void InputManager::Initialize()
{
    createAction(InputName.MOVE_FORWARD);
    createAction(InputName.MOVE_STRAFE);
    createAction(InputName.ACTION);
    createAction(InputName.THROW);

    addBinding(InputName.MOVE_FORWARD, {BindingType::Axis, GLFW_GAMEPAD_AXIS_LEFT_Y });
    addBinding(InputName.MOVE_STRAFE, {BindingType::Axis, GLFW_GAMEPAD_AXIS_LEFT_X });
    addBinding(InputName.ACTION, {BindingType::Button, GLFW_GAMEPAD_BUTTON_A});
    addBinding(InputName.THROW, {BindingType::Button, GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER});
    addBinding(InputName.THROW, { BindingType::Button, GLFW_GAMEPAD_BUTTON_LEFT_BUMPER });
}

void InputManager::createAction(const std::string& name) {
    if (actions.find(name) == actions.end()) {
        actions[name] = { name };
    }
}

void InputManager::removeAction(const std::string& name) {
    actions.erase(name);
}

void InputManager::addBinding(const std::string& actionName, Binding binding) {
    if (actions.count(actionName)) {
        actions[actionName].bindings.push_back(binding);
    }
}

void InputManager::clearBindings(const std::string& actionName) {
    if (actions.count(actionName)) {
        actions[actionName].bindings.clear();
    }
}

void InputManager::subscribe(int deviceID, const std::string& actionName, ActionCallback callback) {
    if (devices[deviceID].find(actionName) == devices[deviceID].end()) {
        if (actions.count(actionName)) {
            devices[deviceID][actionName] = actions[actionName];
        }
    }
        devices[deviceID][actionName].callbacks.push_back(callback);
}


void InputManager::update() {
    for (auto& [deviceID, actions] : devices) {
        for (auto& [name, action] : actions) {
            float maxValue = 0.0f;
            for (const auto& bind : action.bindings) {
                GLFWgamepadstate state;
                if (deviceID != -1) {
                    if (glfwGetGamepadState(deviceID, &state)) {
                        if (bind.type == BindingType::Button) {
                            if (state.buttons[bind.code])
                            {
                                maxValue = 1.0f;
                            }
                        }
                        else {
                            float axisVal = state.axes[bind.code];

                            if (std::abs(axisVal) > std::abs(maxValue) && std::abs(axisVal) > deadzone) {
                                maxValue = axisVal;
                            }
                        }
                    }
                }
            }
            processStateTransition(action, maxValue, deviceID);
        }
    }
}
void InputManager::processStateTransition(InputAction& action, float currentValue, int deviceID) {
    bool currentlyPressed = (std::abs(currentValue) > deadzone);

    if (currentlyPressed && !action.isPressed) {
        trigger(action, currentValue, InputEventType::Started, deviceID);
    }
    else if (!currentlyPressed && action.isPressed) {
        trigger(action, 0.0f, InputEventType::Ended, deviceID);
    }
    else if (std::abs(currentValue - action.lastValue) > 0.001f && currentlyPressed) {
        trigger(action, currentValue, InputEventType::Performed, deviceID);
    }

    action.lastValue = currentValue;
    action.isPressed = currentlyPressed;
}

void InputManager::trigger(InputAction& action, float value, InputEventType type, int deviceID) {
    for (auto& cb : action.callbacks) {
        cb(value, type, deviceID);
    }
}
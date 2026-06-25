#include "InputManager.h"
#include <iostream>

void InputManager::Initialize()
{
    createAction(InputName.MOVE_FORWARD);
    createAction(InputName.MOVE_STRAFE);
    createAction(InputName.LOOK_FORWARD);
    createAction(InputName.LOOK_STRAFE);
    createAction(InputName.ACTION);
    createAction(InputName.INTERACTION);
    createAction(InputName.THROW);

    addBinding(InputName.MOVE_FORWARD, {BindingType::Axis, GLFW_GAMEPAD_AXIS_LEFT_Y });
    addBinding(InputName.MOVE_STRAFE, {BindingType::Axis, GLFW_GAMEPAD_AXIS_LEFT_X });
    addBinding(InputName.LOOK_FORWARD, {BindingType::Axis, GLFW_GAMEPAD_AXIS_RIGHT_Y });
    addBinding(InputName.LOOK_STRAFE, {BindingType::Axis, GLFW_GAMEPAD_AXIS_RIGHT_X });
    addBinding(InputName.ACTION, {BindingType::Button, GLFW_GAMEPAD_BUTTON_A});
    addBinding(InputName.INTERACTION, {BindingType::Button, GLFW_GAMEPAD_BUTTON_X});
    addBinding(InputName.THROW, {BindingType::Button, GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER});
    addBinding(InputName.THROW, { BindingType::Button, GLFW_GAMEPAD_BUTTON_LEFT_BUMPER });
    addBinding(InputName.THROW, { BindingType::Button, GLFW_GAMEPAD_BUTTON_A });
    addBinding(InputName.THROW, { BindingType::TriggerButton, GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER, 1.0f});

    addBinding(InputName.ACTION, { BindingType::TriggerButton, GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER, 0.0f });
    addBinding(InputName.THROW, { BindingType::TriggerButton, GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER, 0.0f });
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
        if (deviceID == -1) continue;

        GLFWgamepadstate state;
        if (!glfwGetGamepadState(deviceID, &state)) continue;
        glm::vec2 leftStick(state.axes[GLFW_GAMEPAD_AXIS_LEFT_X], state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]);
        glm::vec2 rightStick(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X], state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]);
        if (glm::length(leftStick) < deadzone) {
            leftStick = glm::vec2(0.0f);
        }
        else {
            leftStick = glm::normalize(leftStick) * ((glm::length(leftStick) - deadzone) / (1.0f - deadzone));
        }

        if (glm::length(rightStick) < deadzone) {
            rightStick = glm::vec2(0.0f);
        }
        else {
            rightStick = glm::normalize(rightStick) * ((glm::length(rightStick) - deadzone) / (1.0f - deadzone));
        }
        for (auto& [name, action] : actions) {
            float maxValue = 0.0f;
            for (const auto& bind : action.bindings) {

                if (bind.type == BindingType::Button) {
                    if (state.buttons[bind.code]) maxValue = 1.0f;
                }
                else if (bind.type == BindingType::TriggerButton) {
                    if (state.axes[bind.code] >= bind.maxPressure) maxValue = 1.0f;
                }
                else if (bind.type == BindingType::Trigger) {
                    maxValue = (state.axes[bind.code] + 1) / 2;
                }
                else if (bind.type == BindingType::Axis) {
                    if (bind.code == GLFW_GAMEPAD_AXIS_LEFT_X)       maxValue = leftStick.x;
                    else if (bind.code == GLFW_GAMEPAD_AXIS_LEFT_Y)  maxValue = leftStick.y;
                    else if (bind.code == GLFW_GAMEPAD_AXIS_RIGHT_X) maxValue = rightStick.x;
                    else if (bind.code == GLFW_GAMEPAD_AXIS_RIGHT_Y) maxValue = rightStick.y;
                    else maxValue = state.axes[bind.code];
                }
            }
            processStateTransition(action, maxValue, deviceID);
        }
    }
}
void InputManager::processStateTransition(InputAction& action, float currentValue, int deviceID) {
    bool currentlyPressed = (std::abs(currentValue) > 0.001f);

    if (currentlyPressed && !action.isPressed) {
        trigger(action, currentValue, InputEventType::Started, deviceID);
    }
    else if (!currentlyPressed && action.isPressed) {
        trigger(action, 0.0f, InputEventType::Ended, deviceID);
    }
    else if (currentValue != action.lastValue && currentlyPressed) {
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
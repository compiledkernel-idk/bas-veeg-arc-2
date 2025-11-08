#include "core/InputManager.hpp"
#include <iostream>

namespace BVA {

InputManager::InputManager() {
    // Initialize gamepad support for up to 4 players
    gamepads.resize(4);
}

InputManager::~InputManager() {
    shutdown();
}

bool InputManager::initialize() {
    std::cout << "Input manager initialized" << std::endl;

    // Register default actions
    registerAction("Move Forward", {KeyCode::W, KeyCode::Up}, {GamepadButton::DPadUp});
    registerAction("Move Backward", {KeyCode::S, KeyCode::Down}, {GamepadButton::DPadDown});
    registerAction("Move Left", {KeyCode::A, KeyCode::Left}, {GamepadButton::DPadLeft});
    registerAction("Move Right", {KeyCode::D, KeyCode::Right}, {GamepadButton::DPadRight});
    registerAction("Jump", {KeyCode::Space}, {GamepadButton::A});
    registerAction("Attack", {KeyCode::Mouse1}, {GamepadButton::X});
    registerAction("Ability", {KeyCode::Mouse2}, {GamepadButton::B});
    registerAction("Pause", {KeyCode::Escape}, {GamepadButton::Start});

    return true;
}

void InputManager::shutdown() {
    keyStates.clear();
    prevKeyStates.clear();
    actions.clear();
}

void InputManager::update(float dt) {
    // Store previous states
    prevKeyStates = keyStates;
    prevMousePosition = mousePosition;

    // Update input states
    updateKeyboardState();
    updateMouseState();
    updateGamepadState();
}

void InputManager::updateKeyboardState() {
    // TODO: Implement actual keyboard state polling
    // This would use platform-specific APIs or SDL
}

void InputManager::updateMouseState() {
    // TODO: Implement actual mouse state polling
    // Calculate delta
    mouseDelta = mousePosition - prevMousePosition;
}

void InputManager::updateGamepadState() {
    // TODO: Implement actual gamepad state polling
    // This would use SDL_GameController or similar
    connectedGamepads = 0;
    for (auto& gamepad : gamepads) {
        gamepad.connected = false;
    }
}

bool InputManager::isKeyDown(KeyCode key) const {
    auto it = keyStates.find(key);
    return it != keyStates.end() && it->second;
}

bool InputManager::isKeyPressed(KeyCode key) const {
    auto it = keyStates.find(key);
    auto prevIt = prevKeyStates.find(key);

    bool currentlyDown = (it != keyStates.end() && it->second);
    bool wasDown = (prevIt != prevKeyStates.end() && prevIt->second);

    return currentlyDown && !wasDown;
}

bool InputManager::isKeyReleased(KeyCode key) const {
    auto it = keyStates.find(key);
    auto prevIt = prevKeyStates.find(key);

    bool currentlyDown = (it != keyStates.end() && it->second);
    bool wasDown = (prevIt != prevKeyStates.end() && prevIt->second);

    return !currentlyDown && wasDown;
}

bool InputManager::isButtonDown(GamepadButton button, int playerIndex) const {
    if (playerIndex < 0 || playerIndex >= gamepads.size()) return false;
    if (!gamepads[playerIndex].connected) return false;

    auto it = gamepads[playerIndex].buttonStates.find(button);
    return it != gamepads[playerIndex].buttonStates.end() && it->second;
}

bool InputManager::isButtonPressed(GamepadButton button, int playerIndex) const {
    if (playerIndex < 0 || playerIndex >= gamepads.size()) return false;
    if (!gamepads[playerIndex].connected) return false;

    auto it = gamepads[playerIndex].buttonStates.find(button);
    auto prevIt = gamepads[playerIndex].prevButtonStates.find(button);

    bool currentlyDown = (it != gamepads[playerIndex].buttonStates.end() && it->second);
    bool wasDown = (prevIt != gamepads[playerIndex].prevButtonStates.end() && prevIt->second);

    return currentlyDown && !wasDown;
}

bool InputManager::isButtonReleased(GamepadButton button, int playerIndex) const {
    if (playerIndex < 0 || playerIndex >= gamepads.size()) return false;
    if (!gamepads[playerIndex].connected) return false;

    auto it = gamepads[playerIndex].buttonStates.find(button);
    auto prevIt = gamepads[playerIndex].prevButtonStates.find(button);

    bool currentlyDown = (it != gamepads[playerIndex].buttonStates.end() && it->second);
    bool wasDown = (prevIt != gamepads[playerIndex].prevButtonStates.end() && prevIt->second);

    return !currentlyDown && wasDown;
}

Ogre::Vector2 InputManager::getLeftStick(int playerIndex) const {
    if (playerIndex < 0 || playerIndex >= gamepads.size()) return Ogre::Vector2::ZERO;
    if (!gamepads[playerIndex].connected) return Ogre::Vector2::ZERO;
    return gamepads[playerIndex].leftStick;
}

Ogre::Vector2 InputManager::getRightStick(int playerIndex) const {
    if (playerIndex < 0 || playerIndex >= gamepads.size()) return Ogre::Vector2::ZERO;
    if (!gamepads[playerIndex].connected) return Ogre::Vector2::ZERO;
    return gamepads[playerIndex].rightStick;
}

float InputManager::getLeftTrigger(int playerIndex) const {
    if (playerIndex < 0 || playerIndex >= gamepads.size()) return 0.0f;
    if (!gamepads[playerIndex].connected) return 0.0f;
    return gamepads[playerIndex].leftTrigger;
}

float InputManager::getRightTrigger(int playerIndex) const {
    if (playerIndex < 0 || playerIndex >= gamepads.size()) return 0.0f;
    if (!gamepads[playerIndex].connected) return 0.0f;
    return gamepads[playerIndex].rightTrigger;
}

void InputManager::registerAction(const std::string& name, const std::vector<KeyCode>& keys,
                                    const std::vector<GamepadButton>& buttons) {
    InputAction action;
    action.name = name;
    action.keys = keys;
    action.buttons = buttons;
    actions[name] = action;
}

bool InputManager::isActionDown(const std::string& actionName) const {
    auto it = actions.find(actionName);
    if (it == actions.end()) return false;

    const InputAction& action = it->second;

    // Check keyboard
    for (const auto& key : action.keys) {
        if (isKeyDown(key)) return true;
    }

    // Check gamepad
    for (const auto& button : action.buttons) {
        if (isButtonDown(button, 0)) return true;
    }

    return false;
}

bool InputManager::isActionPressed(const std::string& actionName) const {
    auto it = actions.find(actionName);
    if (it == actions.end()) return false;

    const InputAction& action = it->second;

    // Check keyboard
    for (const auto& key : action.keys) {
        if (isKeyPressed(key)) return true;
    }

    // Check gamepad
    for (const auto& button : action.buttons) {
        if (isButtonPressed(button, 0)) return true;
    }

    return false;
}

bool InputManager::isActionReleased(const std::string& actionName) const {
    auto it = actions.find(actionName);
    if (it == actions.end()) return false;

    const InputAction& action = it->second;

    // Check keyboard
    for (const auto& key : action.keys) {
        if (isKeyReleased(key)) return true;
    }

    // Check gamepad
    for (const auto& button : action.buttons) {
        if (isButtonReleased(button, 0)) return true;
    }

    return false;
}

bool InputManager::isGamepadConnected(int playerIndex) const {
    if (playerIndex < 0 || playerIndex >= gamepads.size()) return false;
    return gamepads[playerIndex].connected;
}

} // namespace BVA

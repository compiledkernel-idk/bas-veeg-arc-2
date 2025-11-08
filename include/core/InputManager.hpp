#pragma once

#include <OGRE/Ogre.h>
#include <unordered_map>
#include <functional>
#include <vector>

namespace BVA {

enum class KeyCode {
    W, A, S, D,
    Up, Down, Left, Right,
    Space, Shift, Ctrl, Alt,
    Enter, Escape, Tab,
    Q, E, R, F, G,
    Num1, Num2, Num3, Num4, Num5,
    Mouse1, Mouse2, Mouse3
};

enum class GamepadButton {
    A, B, X, Y,
    LeftBumper, RightBumper,
    LeftTrigger, RightTrigger,
    Start, Back,
    DPadUp, DPadDown, DPadLeft, DPadRight
};

struct InputAction {
    std::string name;
    std::vector<KeyCode> keys;
    std::vector<GamepadButton> buttons;
};

class InputManager {
public:
    InputManager();
    ~InputManager();

    bool initialize();
    void shutdown();
    void update(float dt);

    // Key/button states
    bool isKeyDown(KeyCode key) const;
    bool isKeyPressed(KeyCode key) const;  // True only on first frame
    bool isKeyReleased(KeyCode key) const;
    bool isButtonDown(GamepadButton button, int playerIndex = 0) const;
    bool isButtonPressed(GamepadButton button, int playerIndex = 0) const;
    bool isButtonReleased(GamepadButton button, int playerIndex = 0) const;

    // Mouse
    Ogre::Vector2 getMousePosition() const { return mousePosition; }
    Ogre::Vector2 getMouseDelta() const { return mouseDelta; }
    float getMouseWheel() const { return mouseWheel; }

    // Gamepad analog
    Ogre::Vector2 getLeftStick(int playerIndex = 0) const;
    Ogre::Vector2 getRightStick(int playerIndex = 0) const;
    float getLeftTrigger(int playerIndex = 0) const;
    float getRightTrigger(int playerIndex = 0) const;

    // Action mapping
    void registerAction(const std::string& name, const std::vector<KeyCode>& keys,
                        const std::vector<GamepadButton>& buttons = {});
    bool isActionDown(const std::string& actionName) const;
    bool isActionPressed(const std::string& actionName) const;
    bool isActionReleased(const std::string& actionName) const;

    // Gamepad detection
    int getConnectedGamepadCount() const { return connectedGamepads; }
    bool isGamepadConnected(int playerIndex) const;

private:
    void updateKeyboardState();
    void updateMouseState();
    void updateGamepadState();

    std::unordered_map<KeyCode, bool> keyStates;
    std::unordered_map<KeyCode, bool> prevKeyStates;
    std::unordered_map<std::string, InputAction> actions;

    Ogre::Vector2 mousePosition;
    Ogre::Vector2 mouseDelta;
    Ogre::Vector2 prevMousePosition;
    float mouseWheel = 0.0f;

    struct GamepadState {
        std::unordered_map<GamepadButton, bool> buttonStates;
        std::unordered_map<GamepadButton, bool> prevButtonStates;
        Ogre::Vector2 leftStick;
        Ogre::Vector2 rightStick;
        float leftTrigger = 0.0f;
        float rightTrigger = 0.0f;
        bool connected = false;
    };

    std::vector<GamepadState> gamepads;
    int connectedGamepads = 0;
};

} // namespace BVA

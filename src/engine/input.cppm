module;

import std;
import fixed_map;
import glm;
import glfw;

// extracted from [Circulli Bellum](https://github.com/platonvin/circuli-Bellum)
// simple arena allocator with faster allocation time

using glm::dvec2;
using glm::ivec2;

export module input;

export namespace Lum {
/*
general structure:
    glfwCallback is called on an input event
    this callback remaps key to action
    this action is remapped to action callback function
    if it exists, it is called

Action's could be created in runtime, but there is no need to
*/

// maybe block one if another chosen?
enum class Device : char {
    KeyboardMouse,
    Gamepad
    // TODO is it all?
};

// input obviously does not know game state, so this is purely key-press sugar
enum class ActionType : char {
    // calls callback every update()
    // used for movement
    Continuous, // DEFAULT
    // calls calback once per press+release
    // used for placing blocks
    OneShot

    // TODO: oneshot with extra OR
    // start-end callbacks? short/long jump
    // start / extra-after-specified-time?
};

// i hate setters but there they make sense
template <typename ActionEnum>
class InputHandler {
  public:
    // InputHandler();

    // required for gamepads. Keyboards are handled via callback
    void pollUpdates() {
        for (int act = 0; act < std::to_underlying(ActionEnum::LAST_ACTION); act++) {
            updateActionState((ActionEnum) act, currentActionStates[act]);
        }
        if (glfw::getGamepadState(glfw::JOYSTICK_1, &gState)) {
            for (int b = 0; b <= glfw::GAMEPAD_BUTTON_LAST; b++) {
                bool isPressed = gState.buttons[b] == glfw::PRESS;
                attemptTriggerActionForGamepadButton(b, isPressed);
            }
            // TODO if i ever find a gamepad...
            // input_speed(gState.axes[glfw::GAMEPAD_AXIS_RIGHT_TRIGGER]);
            // please help me if you have one
        }
    }

    void setup(glfw::GLFWwindow* window) {
        glfw::setWindowUserPointer(window, this);
        glfw::setKeyCallback(window, glfwKeyCallback);
        glfw::setMouseButtonCallback(window, glfwMouseButtonCallback);
        glfw::setCursorPosCallback(window, glfwCursorPosCallback);
        glfw::setWindowSizeCallback(window, glfwWindowSizeCallback);
        glfw::setJoystickCallback(glfwJoystickConnectCallback);

        int w, h;
        glfw::getWindowSize(window, &w, &h);
        screenSizef = dvec2(w, h);
        screenSizei = ivec2(w, h);

        // TODO?
        joystickId = glfw::joystickPresent(glfw::JOYSTICK_1) ? -1 : glfw::JOYSTICK_1;
    }
    void cleanup() {}

    void rebindKey(ActionEnum action, int newKey) {
        keyActionMap[newKey] = action;
    }
    void rebindMouseButton(ActionEnum action, int newKey) {
        mouseActionMap[newKey] = action;
    }
    void rebindGamepadButton(ActionEnum action, int newButton) {
        buttonActionMap[newButton] = action;
    }
    void setActionType(ActionEnum action, ActionType type) {
        actionTypeMap[action] = type;
    }

    void setActionCallback(ActionEnum action, const std::function<void(ActionEnum)>& callback) {
        actionCallbackMap[action] = callback;
    }
    // vec2 getMousePos() {return glfwgemo;}

  private:
    static void glfwKeyCallback(
        glfw::GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto* handler = (InputHandler*) (glfw::getWindowUserPointer(window));
        if (handler && (action == glfw::PRESS || action == glfw::RELEASE)) {
            handler->attemptTriggerActionForKey(key, action == glfw::PRESS);
        }
    }

    static void glfwJoystickConnectCallback(int jid, int event) {
        if (event == glfw::CONNECTED) {
            std::cout << "joystick" << glfw::getJoystickName(jid) << "connected\n";
        }
        if (event == glfw::DISCONNECTED) {
            // legit? TODO
            std::cout << "joystick" /* << glfw::getJoystickName(jid) << */ " disconnected\n";
        }
    }

    static void glfwMouseButtonCallback(
        glfw::GLFWwindow* window, int button, int action, int mods) {
        auto* handler = (InputHandler*) (glfw::getWindowUserPointer(window));
        if (handler && (action == glfw::PRESS || action == glfw::RELEASE)) {
            handler->attemptTriggerActionForMouseButton(button, action == glfw::PRESS);
        }
    }

    static void glfwCursorPosCallback(glfw::GLFWwindow* window, double xpos, double ypos) {
        auto* handler = (InputHandler*) (glfw::getWindowUserPointer(window));
        if (handler) {
            handler->updateMousePosition(xpos, ypos);
        }
    }
    static void glfwWindowSizeCallback(glfw::GLFWwindow* window, int width, int height) {
        auto* handler = (InputHandler*) (glfw::getWindowUserPointer(window));
        if (handler) {
            handler->screenSizef = dvec2((double) width, (double) height);
            handler->screenSizei = ivec2(width, height);
        }
    }

    void attemptTriggerActionForKey(int key, bool isPressed) {
        if (keyActionMap.contains(key)) {
            ActionEnum action = keyActionMap[key];
            updateActionState(action, isPressed);
        }
    }
    void attemptTriggerActionForMouseButton(int button, bool isPressed) {
        if (mouseActionMap.contains(button)) {
            ActionEnum action = mouseActionMap[button];
            updateActionState(action, isPressed);
        }
    }
    void attemptTriggerActionForGamepadButton(int button, bool isPressed) {
        if (buttonActionMap.contains(button)) {
            ActionEnum action = buttonActionMap[button];
            if (actionCallbackMap[action]) {
                actionCallbackMap[action](action);
            }
        }
    }
    void updateMousePosition(double xpos, double ypos) {
        mousePosf = dvec2(double(xpos), double(ypos));
        mousePosi = ivec2(int(xpos), int(ypos));
    }

    void updateActionState(ActionEnum action, bool isPressed) {
        bool wasPressed = previousActionStates[action];
        currentActionStates[action] = isPressed;

        ActionType actionType = actionTypeMap[std::to_underlying(action)];

        // continuous actions
        if (actionType == ActionType::Continuous) {
            if (isPressed) {
                // gets called every frame (update)
                if (actionCallbackMap[std::to_underlying(action)])
                    actionCallbackMap[std::to_underlying(action)](action);
            }
        }

        // one-shot actions
        if (actionType == ActionType::OneShot) {
            if (isPressed && !wasPressed) {
                // button has been just pressed, trigger the callback once
                if (actionCallbackMap[std::to_underlying(action)])
                    actionCallbackMap[std::to_underlying(action)](action);
            }
        }

        // Update the previous state after processing
        previousActionStates[action] = isPressed;
    }

    // TODO?
    bool hasJoystick() {
        return joystickId != -1;
    }

  public:
    // screen coords are from top-left to right-bottom
    // so NOT IN [0~1], but in [0~YourScreenSize] for both
    dvec2 mousePosf;
    ivec2 mousePosi;

    // screen coords are from top-left to right-bottom
    dvec2 screenSizef;
    ivec2 screenSizei;

  private:
    int joystickId = -1;
    glfw::GLFWgamepadstate gState;
    // finite amount of keys / actions. Might change if action become dynamic
    FixedMap<glfw::KEY_LAST + 1, ActionEnum> keyActionMap;
    FixedMap<glfw::MOUSE_BUTTON_LAST + 1, ActionEnum> mouseActionMap;
    FixedMap<glfw::GAMEPAD_BUTTON_LAST + 1, ActionEnum> buttonActionMap;
    FixedMap<ActionEnum::LAST_ACTION, std::function<void(ActionEnum)>> actionCallbackMap;
    FixedMap<ActionEnum::LAST_ACTION, ActionType> actionTypeMap;
    FixedMap<ActionEnum::LAST_ACTION, bool> currentActionStates;
    FixedMap<ActionEnum::LAST_ACTION, bool> previousActionStates;
};
} // namespace Lum

module;

#include <volk/volk.h>
#include <GLFW/glfw3.h>

export module glfw;

export namespace glfw {

// ## Types
using ::GLFWallocator;
using ::GLFWerrorfun;
using ::GLFWframebuffersizefun;
using ::GLFWgamepadstate;
using ::GLFWglproc;
using ::GLFWmonitor;
using ::GLFWvidmode;
using ::GLFWwindow;

// Vulkan types (from volk.h)
using ::VkAllocationCallbacks;
using ::VkInstance;
using ::VkPhysicalDevice;
using ::VkResult;
using ::VkSurfaceKHR;

// ## Constants

// Context client API
constexpr auto NO_API = GLFW_NO_API;
constexpr auto OPENGL_API = GLFW_OPENGL_API;
constexpr auto OPENGL_ES_API = GLFW_OPENGL_ES_API;

// Context robustness
constexpr auto NO_ROBUSTNESS = GLFW_NO_ROBUSTNESS;
constexpr auto NO_RESET_NOTIFICATION = GLFW_NO_RESET_NOTIFICATION;
constexpr auto LOSE_CONTEXT_ON_RESET = GLFW_LOSE_CONTEXT_ON_RESET;

// OpenGL profile
constexpr auto OPENGL_ANY_PROFILE = GLFW_OPENGL_ANY_PROFILE;
constexpr auto OPENGL_CORE_PROFILE = GLFW_OPENGL_CORE_PROFILE;
constexpr auto OPENGL_COMPAT_PROFILE = GLFW_OPENGL_COMPAT_PROFILE;

// Input modes
constexpr auto CURSOR = GLFW_CURSOR;
constexpr auto STICKY_KEYS = GLFW_STICKY_KEYS;
constexpr auto STICKY_MOUSE_BUTTONS = GLFW_STICKY_MOUSE_BUTTONS;
constexpr auto LOCK_KEY_MODS = GLFW_LOCK_KEY_MODS;
constexpr auto RAW_MOUSE_MOTION = GLFW_RAW_MOUSE_MOTION;
constexpr auto UNLIMITED_MOUSE_BUTTONS = GLFW_UNLIMITED_MOUSE_BUTTONS;

// Cursor modes
constexpr auto CURSOR_NORMAL = GLFW_CURSOR_NORMAL;
constexpr auto CURSOR_HIDDEN = GLFW_CURSOR_HIDDEN;
constexpr auto CURSOR_DISABLED = GLFW_CURSOR_DISABLED;
constexpr auto CURSOR_CAPTURED = GLFW_CURSOR_CAPTURED;

// Context release behavior
constexpr auto ANY_RELEASE_BEHAVIOR = GLFW_ANY_RELEASE_BEHAVIOR;
constexpr auto RELEASE_BEHAVIOR_FLUSH = GLFW_RELEASE_BEHAVIOR_FLUSH;
constexpr auto RELEASE_BEHAVIOR_NONE = GLFW_RELEASE_BEHAVIOR_NONE;

// Context creation APIs
constexpr auto NATIVE_CONTEXT_API = GLFW_NATIVE_CONTEXT_API;
constexpr auto EGL_CONTEXT_API = GLFW_EGL_CONTEXT_API;
constexpr auto OSMESA_CONTEXT_API = GLFW_OSMESA_CONTEXT_API;

// Angle platform types
constexpr auto ANGLE_PLATFORM_TYPE_NONE = GLFW_ANGLE_PLATFORM_TYPE_NONE;
constexpr auto ANGLE_PLATFORM_TYPE_OPENGL = GLFW_ANGLE_PLATFORM_TYPE_OPENGL;
constexpr auto ANGLE_PLATFORM_TYPE_OPENGLES = GLFW_ANGLE_PLATFORM_TYPE_OPENGLES;
constexpr auto ANGLE_PLATFORM_TYPE_D3D9 = GLFW_ANGLE_PLATFORM_TYPE_D3D9;
constexpr auto ANGLE_PLATFORM_TYPE_D3D11 = GLFW_ANGLE_PLATFORM_TYPE_D3D11;
constexpr auto ANGLE_PLATFORM_TYPE_VULKAN = GLFW_ANGLE_PLATFORM_TYPE_VULKAN;
constexpr auto ANGLE_PLATFORM_TYPE_METAL = GLFW_ANGLE_PLATFORM_TYPE_METAL;

// Wayland libdecor
constexpr auto WAYLAND_PREFER_LIBDECOR = GLFW_WAYLAND_PREFER_LIBDECOR;
constexpr auto WAYLAND_DISABLE_LIBDECOR = GLFW_WAYLAND_DISABLE_LIBDECOR;

// Position
constexpr auto ANY_POSITION = GLFW_ANY_POSITION;

// Version
constexpr auto VERSION_MAJOR = GLFW_VERSION_MAJOR;
constexpr auto VERSION_MINOR = GLFW_VERSION_MINOR;
constexpr auto VERSION_REVISION = GLFW_VERSION_REVISION;

// Boolean values
constexpr auto TRUE = GLFW_TRUE;
constexpr auto FALSE = GLFW_FALSE;

// Input actions
constexpr auto RELEASE = GLFW_RELEASE;
constexpr auto PRESS = GLFW_PRESS;
constexpr auto REPEAT = GLFW_REPEAT;

// Joystick hats
constexpr auto HAT_CENTERED = GLFW_HAT_CENTERED;
constexpr auto HAT_UP = GLFW_HAT_UP;
constexpr auto HAT_RIGHT = GLFW_HAT_RIGHT;
constexpr auto HAT_DOWN = GLFW_HAT_DOWN;
constexpr auto HAT_LEFT = GLFW_HAT_LEFT;
constexpr auto HAT_RIGHT_UP = GLFW_HAT_RIGHT_UP;
constexpr auto HAT_RIGHT_DOWN = GLFW_HAT_RIGHT_DOWN;
constexpr auto HAT_LEFT_UP = GLFW_HAT_LEFT_UP;
constexpr auto HAT_LEFT_DOWN = GLFW_HAT_LEFT_DOWN;

// Keys
constexpr auto KEY_UNKNOWN = GLFW_KEY_UNKNOWN;
constexpr auto KEY_SPACE = GLFW_KEY_SPACE;
constexpr auto KEY_APOSTROPHE = GLFW_KEY_APOSTROPHE;
constexpr auto KEY_COMMA = GLFW_KEY_COMMA;
constexpr auto KEY_MINUS = GLFW_KEY_MINUS;
constexpr auto KEY_PERIOD = GLFW_KEY_PERIOD;
constexpr auto KEY_SLASH = GLFW_KEY_SLASH;
constexpr auto KEY_0 = GLFW_KEY_0;
constexpr auto KEY_1 = GLFW_KEY_1;
constexpr auto KEY_2 = GLFW_KEY_2;
constexpr auto KEY_3 = GLFW_KEY_3;
constexpr auto KEY_4 = GLFW_KEY_4;
constexpr auto KEY_5 = GLFW_KEY_5;
constexpr auto KEY_6 = GLFW_KEY_6;
constexpr auto KEY_7 = GLFW_KEY_7;
constexpr auto KEY_8 = GLFW_KEY_8;
constexpr auto KEY_9 = GLFW_KEY_9;
constexpr auto KEY_SEMICOLON = GLFW_KEY_SEMICOLON;
constexpr auto KEY_EQUAL = GLFW_KEY_EQUAL;
constexpr auto KEY_A = GLFW_KEY_A;
constexpr auto KEY_B = GLFW_KEY_B;
constexpr auto KEY_C = GLFW_KEY_C;
constexpr auto KEY_D = GLFW_KEY_D;
constexpr auto KEY_E = GLFW_KEY_E;
constexpr auto KEY_F = GLFW_KEY_F;
constexpr auto KEY_G = GLFW_KEY_G;
constexpr auto KEY_H = GLFW_KEY_H;
constexpr auto KEY_I = GLFW_KEY_I;
constexpr auto KEY_J = GLFW_KEY_J;
constexpr auto KEY_K = GLFW_KEY_K;
constexpr auto KEY_L = GLFW_KEY_L;
constexpr auto KEY_M = GLFW_KEY_M;
constexpr auto KEY_N = GLFW_KEY_N;
constexpr auto KEY_O = GLFW_KEY_O;
constexpr auto KEY_P = GLFW_KEY_P;
constexpr auto KEY_Q = GLFW_KEY_Q;
constexpr auto KEY_R = GLFW_KEY_R;
constexpr auto KEY_S = GLFW_KEY_S;
constexpr auto KEY_T = GLFW_KEY_T;
constexpr auto KEY_U = GLFW_KEY_U;
constexpr auto KEY_V = GLFW_KEY_V;
constexpr auto KEY_W = GLFW_KEY_W;
constexpr auto KEY_X = GLFW_KEY_X;
constexpr auto KEY_Y = GLFW_KEY_Y;
constexpr auto KEY_Z = GLFW_KEY_Z;
constexpr auto KEY_LEFT_BRACKET = GLFW_KEY_LEFT_BRACKET;
constexpr auto KEY_BACKSLASH = GLFW_KEY_BACKSLASH;
constexpr auto KEY_RIGHT_BRACKET = GLFW_KEY_RIGHT_BRACKET;
constexpr auto KEY_GRAVE_ACCENT = GLFW_KEY_GRAVE_ACCENT;
constexpr auto KEY_WORLD_1 = GLFW_KEY_WORLD_1;
constexpr auto KEY_WORLD_2 = GLFW_KEY_WORLD_2;
constexpr auto KEY_ESCAPE = GLFW_KEY_ESCAPE;
constexpr auto KEY_ENTER = GLFW_KEY_ENTER;
constexpr auto KEY_TAB = GLFW_KEY_TAB;
constexpr auto KEY_BACKSPACE = GLFW_KEY_BACKSPACE;
constexpr auto KEY_INSERT = GLFW_KEY_INSERT;
constexpr auto KEY_DELETE = GLFW_KEY_DELETE;
constexpr auto KEY_RIGHT = GLFW_KEY_RIGHT;
constexpr auto KEY_LEFT = GLFW_KEY_LEFT;
constexpr auto KEY_DOWN = GLFW_KEY_DOWN;
constexpr auto KEY_UP = GLFW_KEY_UP;
constexpr auto KEY_PAGE_UP = GLFW_KEY_PAGE_UP;
constexpr auto KEY_PAGE_DOWN = GLFW_KEY_PAGE_DOWN;
constexpr auto KEY_HOME = GLFW_KEY_HOME;
constexpr auto KEY_END = GLFW_KEY_END;
constexpr auto KEY_CAPS_LOCK = GLFW_KEY_CAPS_LOCK;
constexpr auto KEY_SCROLL_LOCK = GLFW_KEY_SCROLL_LOCK;
constexpr auto KEY_NUM_LOCK = GLFW_KEY_NUM_LOCK;
constexpr auto KEY_PRINT_SCREEN = GLFW_KEY_PRINT_SCREEN;
constexpr auto KEY_PAUSE = GLFW_KEY_PAUSE;
constexpr auto KEY_F1 = GLFW_KEY_F1;
constexpr auto KEY_F2 = GLFW_KEY_F2;
constexpr auto KEY_F3 = GLFW_KEY_F3;
constexpr auto KEY_F4 = GLFW_KEY_F4;
constexpr auto KEY_F5 = GLFW_KEY_F5;
constexpr auto KEY_F6 = GLFW_KEY_F6;
constexpr auto KEY_F7 = GLFW_KEY_F7;
constexpr auto KEY_F8 = GLFW_KEY_F8;
constexpr auto KEY_F9 = GLFW_KEY_F9;
constexpr auto KEY_F10 = GLFW_KEY_F10;
constexpr auto KEY_F11 = GLFW_KEY_F11;
constexpr auto KEY_F12 = GLFW_KEY_F12;
constexpr auto KEY_F13 = GLFW_KEY_F13;
constexpr auto KEY_F14 = GLFW_KEY_F14;
constexpr auto KEY_F15 = GLFW_KEY_F15;
constexpr auto KEY_F16 = GLFW_KEY_F16;
constexpr auto KEY_F17 = GLFW_KEY_F17;
constexpr auto KEY_F18 = GLFW_KEY_F18;
constexpr auto KEY_F19 = GLFW_KEY_F19;
constexpr auto KEY_F20 = GLFW_KEY_F20;
constexpr auto KEY_F21 = GLFW_KEY_F21;
constexpr auto KEY_F22 = GLFW_KEY_F22;
constexpr auto KEY_F23 = GLFW_KEY_F23;
constexpr auto KEY_F24 = GLFW_KEY_F24;
constexpr auto KEY_F25 = GLFW_KEY_F25;
constexpr auto KEY_KP_0 = GLFW_KEY_KP_0;
constexpr auto KEY_KP_1 = GLFW_KEY_KP_1;
constexpr auto KEY_KP_2 = GLFW_KEY_KP_2;
constexpr auto KEY_KP_3 = GLFW_KEY_KP_3;
constexpr auto KEY_KP_4 = GLFW_KEY_KP_4;
constexpr auto KEY_KP_5 = GLFW_KEY_KP_5;
constexpr auto KEY_KP_6 = GLFW_KEY_KP_6;
constexpr auto KEY_KP_7 = GLFW_KEY_KP_7;
constexpr auto KEY_KP_8 = GLFW_KEY_KP_8;
constexpr auto KEY_KP_9 = GLFW_KEY_KP_9;
constexpr auto KEY_KP_DECIMAL = GLFW_KEY_KP_DECIMAL;
constexpr auto KEY_KP_DIVIDE = GLFW_KEY_KP_DIVIDE;
constexpr auto KEY_KP_MULTIPLY = GLFW_KEY_KP_MULTIPLY;
constexpr auto KEY_KP_SUBTRACT = GLFW_KEY_KP_SUBTRACT;
constexpr auto KEY_KP_ADD = GLFW_KEY_KP_ADD;
constexpr auto KEY_KP_ENTER = GLFW_KEY_KP_ENTER;
constexpr auto KEY_KP_EQUAL = GLFW_KEY_KP_EQUAL;
constexpr auto KEY_LEFT_SHIFT = GLFW_KEY_LEFT_SHIFT;
constexpr auto KEY_LEFT_CONTROL = GLFW_KEY_LEFT_CONTROL;
constexpr auto KEY_LEFT_ALT = GLFW_KEY_LEFT_ALT;
constexpr auto KEY_LEFT_SUPER = GLFW_KEY_LEFT_SUPER;
constexpr auto KEY_RIGHT_SHIFT = GLFW_KEY_RIGHT_SHIFT;
constexpr auto KEY_RIGHT_CONTROL = GLFW_KEY_RIGHT_CONTROL;
constexpr auto KEY_RIGHT_ALT = GLFW_KEY_RIGHT_ALT;
constexpr auto KEY_RIGHT_SUPER = GLFW_KEY_RIGHT_SUPER;
constexpr auto KEY_MENU = GLFW_KEY_MENU;
constexpr auto KEY_LAST = GLFW_KEY_LAST;

// Modifier keys
constexpr auto MOD_SHIFT = GLFW_MOD_SHIFT;
constexpr auto MOD_CONTROL = GLFW_MOD_CONTROL;
constexpr auto MOD_ALT = GLFW_MOD_ALT;
constexpr auto MOD_SUPER = GLFW_MOD_SUPER;
constexpr auto MOD_CAPS_LOCK = GLFW_MOD_CAPS_LOCK;
constexpr auto MOD_NUM_LOCK = GLFW_MOD_NUM_LOCK;

// Mouse buttons
constexpr auto MOUSE_BUTTON_1 = GLFW_MOUSE_BUTTON_1;
constexpr auto MOUSE_BUTTON_2 = GLFW_MOUSE_BUTTON_2;
constexpr auto MOUSE_BUTTON_3 = GLFW_MOUSE_BUTTON_3;
constexpr auto MOUSE_BUTTON_4 = GLFW_MOUSE_BUTTON_4;
constexpr auto MOUSE_BUTTON_5 = GLFW_MOUSE_BUTTON_5;
constexpr auto MOUSE_BUTTON_6 = GLFW_MOUSE_BUTTON_6;
constexpr auto MOUSE_BUTTON_7 = GLFW_MOUSE_BUTTON_7;
constexpr auto MOUSE_BUTTON_8 = GLFW_MOUSE_BUTTON_8;
constexpr auto MOUSE_BUTTON_LAST = GLFW_MOUSE_BUTTON_LAST;
constexpr auto MOUSE_BUTTON_LEFT = GLFW_MOUSE_BUTTON_LEFT;
constexpr auto MOUSE_BUTTON_RIGHT = GLFW_MOUSE_BUTTON_RIGHT;
constexpr auto MOUSE_BUTTON_MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE;

// Joysticks
constexpr auto JOYSTICK_1 = GLFW_JOYSTICK_1;
constexpr auto JOYSTICK_2 = GLFW_JOYSTICK_2;
constexpr auto JOYSTICK_3 = GLFW_JOYSTICK_3;
constexpr auto JOYSTICK_4 = GLFW_JOYSTICK_4;
constexpr auto JOYSTICK_5 = GLFW_JOYSTICK_5;
constexpr auto JOYSTICK_6 = GLFW_JOYSTICK_6;
constexpr auto JOYSTICK_7 = GLFW_JOYSTICK_7;
constexpr auto JOYSTICK_8 = GLFW_JOYSTICK_8;
constexpr auto JOYSTICK_9 = GLFW_JOYSTICK_9;
constexpr auto JOYSTICK_10 = GLFW_JOYSTICK_10;
constexpr auto JOYSTICK_11 = GLFW_JOYSTICK_11;
constexpr auto JOYSTICK_12 = GLFW_JOYSTICK_12;
constexpr auto JOYSTICK_13 = GLFW_JOYSTICK_13;
constexpr auto JOYSTICK_14 = GLFW_JOYSTICK_14;
constexpr auto JOYSTICK_15 = GLFW_JOYSTICK_15;
constexpr auto JOYSTICK_16 = GLFW_JOYSTICK_16;
constexpr auto JOYSTICK_LAST = GLFW_JOYSTICK_LAST;

// Gamepad buttons
constexpr auto GAMEPAD_BUTTON_A = GLFW_GAMEPAD_BUTTON_A;
constexpr auto GAMEPAD_BUTTON_B = GLFW_GAMEPAD_BUTTON_B;
constexpr auto GAMEPAD_BUTTON_X = GLFW_GAMEPAD_BUTTON_X;
constexpr auto GAMEPAD_BUTTON_Y = GLFW_GAMEPAD_BUTTON_Y;
constexpr auto GAMEPAD_BUTTON_LEFT_BUMPER = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER;
constexpr auto GAMEPAD_BUTTON_RIGHT_BUMPER = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER;
constexpr auto GAMEPAD_BUTTON_BACK = GLFW_GAMEPAD_BUTTON_BACK;
constexpr auto GAMEPAD_BUTTON_START = GLFW_GAMEPAD_BUTTON_START;
constexpr auto GAMEPAD_BUTTON_GUIDE = GLFW_GAMEPAD_BUTTON_GUIDE;
constexpr auto GAMEPAD_BUTTON_LEFT_THUMB = GLFW_GAMEPAD_BUTTON_LEFT_THUMB;
constexpr auto GAMEPAD_BUTTON_RIGHT_THUMB = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB;
constexpr auto GAMEPAD_BUTTON_DPAD_UP = GLFW_GAMEPAD_BUTTON_DPAD_UP;
constexpr auto GAMEPAD_BUTTON_DPAD_RIGHT = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT;
constexpr auto GAMEPAD_BUTTON_DPAD_DOWN = GLFW_GAMEPAD_BUTTON_DPAD_DOWN;
constexpr auto GAMEPAD_BUTTON_DPAD_LEFT = GLFW_GAMEPAD_BUTTON_DPAD_LEFT;
constexpr auto GAMEPAD_BUTTON_LAST = GLFW_GAMEPAD_BUTTON_LAST;
constexpr auto GAMEPAD_BUTTON_CROSS = GLFW_GAMEPAD_BUTTON_CROSS;
constexpr auto GAMEPAD_BUTTON_CIRCLE = GLFW_GAMEPAD_BUTTON_CIRCLE;
constexpr auto GAMEPAD_BUTTON_SQUARE = GLFW_GAMEPAD_BUTTON_SQUARE;
constexpr auto GAMEPAD_BUTTON_TRIANGLE = GLFW_GAMEPAD_BUTTON_TRIANGLE;

// Gamepad axes
constexpr auto GAMEPAD_AXIS_LEFT_X = GLFW_GAMEPAD_AXIS_LEFT_X;
constexpr auto GAMEPAD_AXIS_LEFT_Y = GLFW_GAMEPAD_AXIS_LEFT_Y;
constexpr auto GAMEPAD_AXIS_RIGHT_X = GLFW_GAMEPAD_AXIS_RIGHT_X;
constexpr auto GAMEPAD_AXIS_RIGHT_Y = GLFW_GAMEPAD_AXIS_RIGHT_Y;
constexpr auto GAMEPAD_AXIS_LEFT_TRIGGER = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER;
constexpr auto GAMEPAD_AXIS_RIGHT_TRIGGER = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER;
constexpr auto GAMEPAD_AXIS_LAST = GLFW_GAMEPAD_AXIS_LAST;

// Error codes
constexpr auto NO_ERROR = GLFW_NO_ERROR;
constexpr auto NOT_INITIALIZED = GLFW_NOT_INITIALIZED;
constexpr auto NO_CURRENT_CONTEXT = GLFW_NO_CURRENT_CONTEXT;
constexpr auto INVALID_ENUM = GLFW_INVALID_ENUM;
constexpr auto INVALID_VALUE = GLFW_INVALID_VALUE;
constexpr auto OUT_OF_MEMORY = GLFW_OUT_OF_MEMORY;
constexpr auto API_UNAVAILABLE = GLFW_API_UNAVAILABLE;
constexpr auto VERSION_UNAVAILABLE = GLFW_VERSION_UNAVAILABLE;
constexpr auto PLATFORM_ERROR = GLFW_PLATFORM_ERROR;
constexpr auto FORMAT_UNAVAILABLE = GLFW_FORMAT_UNAVAILABLE;
constexpr auto NO_WINDOW_CONTEXT = GLFW_NO_WINDOW_CONTEXT;
constexpr auto CURSOR_UNAVAILABLE = GLFW_CURSOR_UNAVAILABLE;
constexpr auto FEATURE_UNAVAILABLE = GLFW_FEATURE_UNAVAILABLE;
constexpr auto FEATURE_UNIMPLEMENTED = GLFW_FEATURE_UNIMPLEMENTED;
constexpr auto PLATFORM_UNAVAILABLE = GLFW_PLATFORM_UNAVAILABLE;

// Window attributes
constexpr auto FOCUSED = GLFW_FOCUSED;
constexpr auto ICONIFIED = GLFW_ICONIFIED;
constexpr auto RESIZABLE = GLFW_RESIZABLE;
constexpr auto VISIBLE = GLFW_VISIBLE;
constexpr auto DECORATED = GLFW_DECORATED;
constexpr auto AUTO_ICONIFY = GLFW_AUTO_ICONIFY;
constexpr auto FLOATING = GLFW_FLOATING;
constexpr auto MAXIMIZED = GLFW_MAXIMIZED;
constexpr auto CENTER_CURSOR = GLFW_CENTER_CURSOR;
constexpr auto TRANSPARENT_FRAMEBUFFER = GLFW_TRANSPARENT_FRAMEBUFFER;
constexpr auto HOVERED = GLFW_HOVERED;
constexpr auto FOCUS_ON_SHOW = GLFW_FOCUS_ON_SHOW;
constexpr auto MOUSE_PASSTHROUGH = GLFW_MOUSE_PASSTHROUGH;
constexpr auto POSITION_X = GLFW_POSITION_X;
constexpr auto POSITION_Y = GLFW_POSITION_Y;

// Framebuffer attributes
constexpr auto RED_BITS = GLFW_RED_BITS;
constexpr auto GREEN_BITS = GLFW_GREEN_BITS;
constexpr auto BLUE_BITS = GLFW_BLUE_BITS;
constexpr auto ALPHA_BITS = GLFW_ALPHA_BITS;
constexpr auto DEPTH_BITS = GLFW_DEPTH_BITS;
constexpr auto STENCIL_BITS = GLFW_STENCIL_BITS;
constexpr auto ACCUM_RED_BITS = GLFW_ACCUM_RED_BITS;
constexpr auto ACCUM_GREEN_BITS = GLFW_ACCUM_GREEN_BITS;
constexpr auto ACCUM_BLUE_BITS = GLFW_ACCUM_BLUE_BITS;
constexpr auto ACCUM_ALPHA_BITS = GLFW_ACCUM_ALPHA_BITS;
constexpr auto AUX_BUFFERS = GLFW_AUX_BUFFERS;
constexpr auto STEREO = GLFW_STEREO;
constexpr auto SAMPLES = GLFW_SAMPLES;
constexpr auto SRGB_CAPABLE = GLFW_SRGB_CAPABLE;
constexpr auto REFRESH_RATE = GLFW_REFRESH_RATE;
constexpr auto DOUBLEBUFFER = GLFW_DOUBLEBUFFER;

// Window hints
constexpr auto CLIENT_API = GLFW_CLIENT_API;
constexpr auto CONTEXT_VERSION_MAJOR = GLFW_CONTEXT_VERSION_MAJOR;
constexpr auto CONTEXT_VERSION_MINOR = GLFW_CONTEXT_VERSION_MINOR;
constexpr auto CONTEXT_REVISION = GLFW_CONTEXT_REVISION;
constexpr auto CONTEXT_ROBUSTNESS = GLFW_CONTEXT_ROBUSTNESS;
constexpr auto OPENGL_FORWARD_COMPAT = GLFW_OPENGL_FORWARD_COMPAT;
constexpr auto CONTEXT_DEBUG = GLFW_CONTEXT_DEBUG;
constexpr auto OPENGL_DEBUG_CONTEXT = GLFW_OPENGL_DEBUG_CONTEXT;
constexpr auto OPENGL_PROFILE = GLFW_OPENGL_PROFILE;
constexpr auto CONTEXT_RELEASE_BEHAVIOR = GLFW_CONTEXT_RELEASE_BEHAVIOR;
constexpr auto CONTEXT_NO_ERROR = GLFW_CONTEXT_NO_ERROR;
constexpr auto CONTEXT_CREATION_API = GLFW_CONTEXT_CREATION_API;
constexpr auto SCALE_TO_MONITOR = GLFW_SCALE_TO_MONITOR;
constexpr auto SCALE_FRAMEBUFFER = GLFW_SCALE_FRAMEBUFFER;
constexpr auto COCOA_RETINA_FRAMEBUFFER = GLFW_COCOA_RETINA_FRAMEBUFFER;
constexpr auto COCOA_FRAME_NAME = GLFW_COCOA_FRAME_NAME;
constexpr auto COCOA_GRAPHICS_SWITCHING = GLFW_COCOA_GRAPHICS_SWITCHING;
constexpr auto X11_CLASS_NAME = GLFW_X11_CLASS_NAME;
constexpr auto X11_INSTANCE_NAME = GLFW_X11_INSTANCE_NAME;
constexpr auto WIN32_KEYBOARD_MENU = GLFW_WIN32_KEYBOARD_MENU;
constexpr auto WIN32_SHOWDEFAULT = GLFW_WIN32_SHOWDEFAULT;
constexpr auto WAYLAND_APP_ID = GLFW_WAYLAND_APP_ID;

constexpr auto CONNECTED = GLFW_CONNECTED;
constexpr auto DISCONNECTED = GLFW_DISCONNECTED;

// ## Functions

inline int init() {
    return glfwInit();
}
inline void terminate() {
    ::glfwTerminate();
}
inline void initHint(int hint, int value) {
    ::glfwInitHint(hint, value);
}
inline void initAllocator(const GLFWallocator* alloc) {
    ::glfwInitAllocator(alloc);
}

inline GLFWerrorfun setErrorCallback(GLFWerrorfun cb) {
    return glfwSetErrorCallback(cb);
}
inline int getError(const char** desc) {
    return glfwGetError(desc);
}

inline void defaultWindowHints() {
    ::glfwDefaultWindowHints();
}
inline void windowHint(int hint, int value) {
    ::glfwWindowHint(hint, value);
}
inline void windowHintString(int hint, const char* v) {
    ::glfwWindowHintString(hint, v);
}

inline GLFWwindow* createWindow(
    int w, int h, const char* title, GLFWmonitor* m, GLFWwindow* share) {
    return glfwCreateWindow(w, h, title, m, share);
}
inline void destroyWindow(GLFWwindow* window) {
    ::glfwDestroyWindow(window);
}

inline void makeContextCurrent(GLFWwindow* window) {
    ::glfwMakeContextCurrent(window);
}
inline GLFWwindow* getCurrentContext() {
    return glfwGetCurrentContext();
}
inline void swapInterval(int interval) {
    ::glfwSwapInterval(interval);
}

inline int extensionSupported(const char* ext) {
    return glfwExtensionSupported(ext);
}
inline GLFWglproc getProcAddress(const char* procname) {
    return glfwGetProcAddress(procname);
}

inline void getFramebufferSize(GLFWwindow* window, int* w, int* h) {
    ::glfwGetFramebufferSize(window, w, h);
}
inline GLFWframebuffersizefun setFramebufferSizeCallback(
    GLFWwindow* window, GLFWframebuffersizefun callback) {
    return glfwSetFramebufferSizeCallback(window, callback);
}

inline int getInputMode(GLFWwindow* window, int mode) {
    return glfwGetInputMode(window, mode);
}
inline void setInputMode(GLFWwindow* window, int mode, int value) {
    ::glfwSetInputMode(window, mode, value);
}

inline void* getWindowUserPointer(GLFWwindow* window) {
    return glfwGetWindowUserPointer(window);
}
inline void setWindowUserPointer(GLFWwindow* window, void* ptr) {
    ::glfwSetWindowUserPointer(window, ptr);
}
inline void setWindowAttrib(GLFWwindow* window, int attrib, int value) {
    ::glfwSetWindowAttrib(window, attrib, value);
}

inline void pollEvents() {
    ::glfwPollEvents();
}
inline void waitEvents() {
    ::glfwWaitEvents();
}
inline void waitEventsTimeout(double t) {
    ::glfwWaitEventsTimeout(t);
}
inline void postEmptyEvent() {
    ::glfwPostEmptyEvent();
}

inline int windowShouldClose(GLFWwindow* window) {
    return glfwWindowShouldClose(window);
}
inline void setWindowShouldClose(GLFWwindow* window, int value) {
    ::glfwSetWindowShouldClose(window, value);
}
inline void swapBuffers(GLFWwindow* window) {
    ::glfwSwapBuffers(window);
}

inline GLFWmonitor* getPrimaryMonitor() {
    return glfwGetPrimaryMonitor();
}
inline const GLFWvidmode* getVideoMode(GLFWmonitor* monitor) {
    return glfwGetVideoMode(monitor);
}

inline int vulkanSupported() {
    return glfwVulkanSupported();
}
inline const char** getRequiredInstanceExtensions(uint32_t* count) {
    return glfwGetRequiredInstanceExtensions(count);
}
inline VkResult createWindowSurface(VkInstance instance, GLFWwindow* window,
    const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) {
    return glfwCreateWindowSurface(instance, window, allocator, surface);
}
inline int getPhysicalDevicePresentationSupport(
    VkInstance instance, VkPhysicalDevice device, uint32_t queueFamily) {
    return glfwGetPhysicalDevicePresentationSupport(instance, device, queueFamily);
}

inline int getGamepadState(int jid, GLFWgamepadstate* state) {
    return glfwGetGamepadState(jid, state);
};

inline GLFWkeyfun setKeyCallback(GLFWwindow* window, GLFWkeyfun callback) {
    return glfwSetKeyCallback(window, callback);
}
inline GLFWmousebuttonfun setMouseButtonCallback(GLFWwindow* window, GLFWmousebuttonfun callback) {
    return glfwSetMouseButtonCallback(window, callback);
}
inline GLFWcursorposfun setCursorPosCallback(GLFWwindow* window, GLFWcursorposfun callback) {
    return glfwSetCursorPosCallback(window, callback);
}
inline GLFWwindowsizefun setWindowSizeCallback(GLFWwindow* window, GLFWwindowsizefun callback) {
    return glfwSetWindowSizeCallback(window, callback);
}
inline GLFWjoystickfun setJoystickCallback(GLFWjoystickfun callback) {
    return glfwSetJoystickCallback(callback);
}
inline void getWindowSize(GLFWwindow* window, int* width, int* height) {
    return glfwGetWindowSize(window, width, height);
}
inline int joystickPresent(int jid) {
    return glfwJoystickPresent(jid);
}
inline const char* getJoystickName(int jid) {
    return glfwGetJoystickName(jid);
}

inline int getKey(GLFWwindow* window, int key) {
    return glfwGetKey(window, key);
}

} // namespace glfw
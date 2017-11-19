#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <unordered_set>

namespace cfl {
namespace input {
/////////////////////////////
/// Keyboard-specific API ///
/////////////////////////////

#define LENGTH_OF_ENUM(enum_name) NUM_ENUM_ENTRIES_ ## enum_name

enum class Modifier {
    SHIFT = 0,
    CONTROL,
    ALT,
    SUPER,
    LENGTH_OF_ENUM(Modifier) // should equal the number of entries in the enum
};

struct ModifierSet {
    virtual bool Test(Modifier modifier) const = 0;

    virtual std::unordered_set<Modifier> GetModifiers() const = 0;
};

enum class Key {
    SPACE = 0,
    APOSTROPHE,
    COMMA,
    MINUS,
    PERIOD,
    SLASH,
    DIGIT_0, DIGIT_1, DIGIT_2, DIGIT_3, DIGIT_4, DIGIT_5, DIGIT_6, DIGIT_7, DIGIT_8, DIGIT_9,
    SEMICOLON,
    EQUAL,
    A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    LEFT_BRACKET,
    BACKSLASH,
    RIGHT_BRACKET,
    GRAVE_ACCENT,
    WORLD_1,
    WORLD_2,
    ESCAPE,
    ENTER,
    TAB,
    BACKSPACE,
    INSERT,
    DELETE,
    RIGHT,
    LEFT,
    DOWN,
    UP,
    PAGE_UP,
    PAGE_DOWN,
    HOME,
    END,
    CAPS_LOCK,
    SCROLL_LOCK,
    NUM_LOCK,
    PRINT_SCREEN,
    PAUSE,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25,
    KP_0, KP_1, KP_2, KP_3, KP_4, KP_5, KP_6, KP_7, KP_8, KP_9,
    KP_DECIMAL, KP_DIVIDE, KP_MULTIPLY, KP_SUBTRACT, KP_ADD,
    KP_ENTER,
    KP_EQUAL,
    LEFT_SHIFT, LEFT_CONTROL, LEFT_ALT, LEFT_SUPER,
    RIGHT_SHIFT, RIGHT_CONTROL, RIGHT_ALT, RIGHT_SUPER,
    MENU,
    LENGTH_OF_ENUM(Key) // should equal the number of entries in the enum
};

enum class KeyAction {
    PRESS = 0,
    REPEAT,
    RELEASE,
    LENGTH_OF_ENUM(KeyAction) // should equal the number of entries in the enum
};

enum class MouseButton {
    BUTTON_1 = 0,
    BUTTON_2,
    BUTTON_3,
    BUTTON_4,
    BUTTON_5,
    BUTTON_6,
    BUTTON_7,
    BUTTON_8,
    BUTTON_LAST = static_cast<int>(BUTTON_8),
    BUTTON_LEFT = static_cast<int>(BUTTON_1),
    BUTTON_RIGHT = static_cast<int>(BUTTON_2),
    BUTTON_MIDDLE = static_cast<int>(BUTTON_3),
    LENGTH_OF_ENUM(MouseButton) // should equal the number of entries in the enum
};

enum class MouseAction {
    PRESS = 0,
    RELEASE,
    LENGTH_OF_ENUM(MouseAction) // should equal the number of entries in the enum
};

///////////////////////////////
/// Controller-agnostic API ///
///////////////////////////////

enum class Axis {
    HORIZONTAL = 0,
    VERTICAL,
    LENGTH_OF_ENUM(Axis) // should equal the number of entries in the enum
};

#undef LENGTH_OF_ENUM
}
} // namespace cfl

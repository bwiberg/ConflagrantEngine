#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/Window.hh>
#include <conflagrant/input.hh>

namespace cfl {
class Engine;

class InputManager final {
    friend class cfl::Engine;

    enum class State {
        NONE,
        PRESSED,
        HELD_DOWN,
        RELEASED
    };

    std::shared_ptr<Window> window;

    bool ProcessInput();

    std::array<State, static_cast<int>(input::Key::NUM_ENUM_ENTRIES_Key)> keyStates;

    std::array<State, static_cast<int>(input::Key::NUM_ENUM_ENTRIES_Key)> mouseButtonStates;

    dvec2 mousePosition;

public:
    explicit InputManager(std::shared_ptr<Window> &window);

    ~InputManager();

    ///////////////////////////////
    /// Controller-agnostic API ///
    ///////////////////////////////

    vec2 GetAxis(input::Axis axis) const;

    /////////////////////////////
    /// Keyboard-specific API ///
    /////////////////////////////

    bool AnyKey() const;

    bool AnyKeyDown() const;

    dvec2 GetMousePositionPixel() const;

    dvec2 GetMousePositionNormalized() const;

    bool GetMouseButton(input::MouseButton button) const;

    bool GetMouseButtonDown(input::MouseButton button) const;

    bool GetMouseButtonUp(input::MouseButton button) const;

    bool GetKey(input::Key key) const;

    bool GetKeyDown(input::Key key) const;

    bool GetKeyUp(input::Key key) const;
};
} // namespace cfl

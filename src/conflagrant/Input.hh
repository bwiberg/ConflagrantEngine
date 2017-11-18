#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/Window.hh>
#include <conflagrant/input.hh>

namespace cfl {
class Engine;

class Input final {
    friend class cfl::Engine;

    std::shared_ptr<Window> window;

    bool ProcessInput();

public:
    Input(std::shared_ptr<Window> &window);

    ~Input();

    ///////////////////////////////
    /// Controller-agnostic API ///
    ///////////////////////////////

    vec2 GetAxis(input::Axis axis);

    /////////////////////////////
    /// Keyboard-specific API ///
    /////////////////////////////

    bool GetMouseButton(input::MouseButton button);

    bool GetMouseButtonDown(input::MouseButton button);

    bool GetMouseButtonUp(input::MouseButton button);

    bool GetKey(input::Key key);

    bool GetKeyDown(input::Key key);

    bool GetKeyUp(input::Key key);
};
} // namespace cfl

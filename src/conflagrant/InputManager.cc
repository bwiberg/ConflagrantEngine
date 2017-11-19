#include "InputManager.hh"

#include <conflagrant/logging.hh>
#include <algorithm>

namespace cfl {
InputManager::InputManager(std::shared_ptr<Window> &window) : window(window) {
    assert(window != nullptr);

//    window->SetFramebufferSizeCallback([this](uint width,
//                                              uint height) {
//        LOG_ERROR(cfl::InputManager::FramebufferSizeCallback) << "Unimplemented." << std::endl;
//    });

    window->SetKeyCallback([this](input::Key key,
                                  input::KeyAction action,
                                  input::ModifierSet const &modifiers) {
        switch (action) {
            case input::KeyAction::PRESS:
                keyStates[static_cast<int>(key)] = State::PRESSED;
                return;
            case input::KeyAction::REPEAT:
                keyStates[static_cast<int>(key)] = State::HELD_DOWN;
                return;
            case input::KeyAction::RELEASE:
                keyStates[static_cast<int>(key)] = State::RELEASED;
                return;
            default:
                LOG_ERROR(cfl::InputManager::<lambda> SetKeyCallback)
                        << "Received invalid input::KeyAction of value " << static_cast<int>(action) << std::endl;
        }
    });

    window->SetMouseButtonCallback([this](input::MouseButton button,
                                          input::MouseAction action,
                                          input::ModifierSet const &modifiers) {
        switch (action) {
            case input::MouseAction::PRESS:
                mouseButtonStates[static_cast<int>(button)] = State::PRESSED;
                return;
            case input::MouseAction::RELEASE:
                mouseButtonStates[static_cast<int>(button)] = State::HELD_DOWN;
                return;
            default:
                LOG_ERROR(cfl::InputManager::<lambda> SetMouseButtonCallback)
                        << "Received invalid input::MouseAction of value " << static_cast<int>(action) << std::endl;
        }
    });

    window->SetMousePosCallback([this](double x, double y) {
        mousePosition.x = x;
        mousePosition.y = y;
    });
};

InputManager::~InputManager() {
    window->SetKeyCallback(Window::NoopKeyCallback());
    window->SetMouseButtonCallback(Window::NoopMouseButtonCallback());
    window->SetMousePosCallback(Window::NoopMousePosCallback());
    window = nullptr;
}

bool InputManager::ProcessInput() {
    // manually update all "RELEASED" keyStates to "NONE" since there is no KeyAction that is "REPEATED RELEASE"
    for (auto &keyState : keyStates) {
        if (keyState == State::RELEASED) {
            keyState = State::NONE;
        }
    }

    // manually update all "RELEASED" mouseButtonStates to "NONE" since there is no MouseAction for "REPEAT RELEASE"
    // also update all "PRESSED" mouseButtonStates to "HELD_DOWN" since there is no MouseAction for "REPEAT"
    for (auto &mouseButtonState : mouseButtonStates) {
        if (mouseButtonState == State::RELEASED) {
            mouseButtonState = State::NONE;
        }

        if (mouseButtonState == State::PRESSED) {
            mouseButtonState = State::HELD_DOWN;
        }
    }

    return window->PollEvents();
}

vec2 InputManager::GetAxis(input::Axis axis) const {
    LOG_ERROR(cfl::InputManager::GetAxis) << "Unimplemented." << std::endl;
    return cfl::vec2();
}

bool InputManager::AnyKey() const {
    return std::any_of(keyStates.begin(), keyStates.end(), [](InputManager::State state) {
        return state == State::PRESSED || state == State::HELD_DOWN;
    });
}

bool InputManager::AnyKeyDown() const {
    return std::any_of(keyStates.begin(), keyStates.end(), [](InputManager::State state) {
        return state == State::PRESSED;
    });
}

dvec2 InputManager::GetMousePositionPixel() const {
    return mousePosition;
}

dvec2 InputManager::GetMousePositionNormalized() const {
    return mousePosition / dvec2(window->GetSize());
}

bool InputManager::GetMouseButton(input::MouseButton button) const {
    return mouseButtonStates[static_cast<int>(button)] == State::PRESSED ||
           mouseButtonStates[static_cast<int>(button)] == State::HELD_DOWN;
}

bool InputManager::GetMouseButtonDown(input::MouseButton button) const {
    return mouseButtonStates[static_cast<int>(button)] == State::PRESSED;
}

bool InputManager::GetMouseButtonUp(input::MouseButton button) const {
    return mouseButtonStates[static_cast<int>(button)] == State::RELEASED;
}

bool InputManager::GetKey(input::Key key) const {
    return keyStates[static_cast<int>(key)] == State::PRESSED ||
           keyStates[static_cast<int>(key)] == State::HELD_DOWN;
}

bool InputManager::GetKeyDown(input::Key key) const {
    return keyStates[static_cast<int>(key)] == State::PRESSED;
}

bool InputManager::GetKeyUp(input::Key key) const {
    return keyStates[static_cast<int>(key)] == State::RELEASED;
}
} // namespace cfl

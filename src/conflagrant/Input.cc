#include "Input.hh"

#include <conflagrant/logging.hh>

namespace cfl {
Input::Input(std::shared_ptr<Window> &window) : window(window) {
    assert(window != nullptr);

    window->SetFramebufferSizeCallback([this](uint width,
                                              uint height) {
        LOG_ERROR(cfl::Input::FramebufferSizeCallback) << "Unimplemented." << std::endl;
    });

    window->SetKeyCallback([this](input::Key key,
                                  input::KeyAction action,
                                  input::ModifierSet const &modifiers) {
        LOG_ERROR(cfl::Input::KeyCallback) << "Unimplemented." << std::endl;
    });

    window->SetMouseButtonCallback([this](input::MouseButton button,
                                          input::MouseAction action,
                                          input::ModifierSet const &modifiers) {
        LOG_ERROR(cfl::Input::MouseButtonCallback) << "Unimplemented." << std::endl;
    });

    window->SetMousePosCallback([this](double x, double y) {
        LOG_ERROR(cfl::Input::MousePosCallback) << "Unimplemented." << std::endl;
    });
};

Input::~Input() {
    window->SetFramebufferSizeCallback(Window::NoopFramebufferSizeCallback());
    window->SetKeyCallback(Window::NoopKeyCallback());
    window->SetMouseButtonCallback(Window::NoopMouseButtonCallback());
    window->SetMousePosCallback(Window::NoopMousePosCallback());
    window = nullptr;
}

bool Input::ProcessInput() {
    return window->PollEvents();
}

vec2 Input::GetAxis(input::Axis axis) {
    LOG_ERROR(cfl::Input::GetAxis()) << "Unimplemented." << std::endl;
    return cfl::vec2();
}

bool Input::GetMouseButton(input::MouseButton button) {
    LOG_ERROR(cfl::Input::GetMouseButton()) << "Unimplemented." << std::endl;
    return false;
}

bool Input::GetMouseButtonDown(input::MouseButton button) {
    LOG_ERROR(cfl::Input::GetMouseButtonDown()) << "Unimplemented." << std::endl;
    return false;
}

bool Input::GetMouseButtonUp(input::MouseButton button) {
    LOG_ERROR(cfl::Input::GetMouseButtonUp()) << "Unimplemented." << std::endl;
    return false;
}

bool Input::GetKey(input::Key key) {
    LOG_ERROR(cfl::Input::GetKey()) << "Unimplemented." << std::endl;
    return false;
}

bool Input::GetKeyDown(input::Key key) {
    LOG_ERROR(cfl::Input::GetKeyDown()) << "Unimplemented." << std::endl;
    return false;
}

bool Input::GetKeyUp(input::Key key) {
    LOG_ERROR(cfl::Input::GetKeyUp()) << "Unimplemented." << std::endl;
    return false;
}
} // namespace cfl

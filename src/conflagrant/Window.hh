#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <functional>
#include <conflagrant/input.hh>

namespace cfl {
using namespace input;

enum class CursorMode {
    NORMAL,
    HIDDEN,
    HIDDEN_FIXED
};

struct Window {
    typedef std::function<void(Key, KeyAction, ModifierSet const &)> KeyCallback;
    // typedef std::function<void(uint, uint)> FramebufferSizeCallback;
    typedef std::function<void(MouseButton, MouseAction, ModifierSet const &)> MouseButtonCallback;
    typedef std::function<void(double, double)> MousePosCallback;

    inline static KeyCallback const NoopKeyCallback() {
        return [](Key a, KeyAction b, ModifierSet const &c) {};
    };

    // inline static FramebufferSizeCallback const NoopFramebufferSizeCallback() {
    //     return [](uint a, uint b) {};
    // };

    inline static MouseButtonCallback const NoopMouseButtonCallback() {
        return [](MouseButton a, MouseAction b, ModifierSet const &c) {};
    };

    inline static MousePosCallback const NoopMousePosCallback() {
        return [](double a, double b) {};
    };

    virtual ~Window() = default;

    // virtual void SetFramebufferSizeCallback(FramebufferSizeCallback callback) = 0;

    virtual void SetKeyCallback(KeyCallback callback) = 0;

    virtual void SetMouseButtonCallback(MouseButtonCallback callback) = 0;

    virtual void SetMousePosCallback(MousePosCallback callback) = 0;

    virtual bool SetCursorMode(CursorMode mode) const = 0;

    virtual CursorMode GetCursorMode() const = 0;

    virtual bool MakeContextCurrent() = 0;

    virtual bool PollEvents() = 0;

    virtual bool SetSwapInterval(int interval) = 0;

    virtual int GetSwapInterval() const = 0;

    virtual bool BeginFrame() = 0;

    virtual bool FinishFrame(bool renderGui = true) = 0;

    virtual uvec2 GetSize() const = 0;

    virtual bool SizeHasChanged(uvec2 &sizeOut) const = 0;

    virtual double GetTime() const = 0;

    virtual bool SetTime(double time) = 0;

    virtual bool SetTitle(string const &title) = 0;
};
} // namespace cfl

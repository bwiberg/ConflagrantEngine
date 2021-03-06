#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/Window.hh>
#include <unordered_map>
#include <GLFW/glfw3.h>

namespace cfl {
struct GlfwModifierSet : public input::ModifierSet {
    int glfwModifierBits;

    explicit GlfwModifierSet(int glfwModifierBits);

    bool Test(input::Modifier modifier) const override;

    std::unordered_set<input::Modifier> GetModifiers() const override;
};

class GlfwWindow final : public Window {
    GlfwWindow() = default;

    static bool InitHasBeenCalled;

    GLFWwindow *window;

    uint width, height;

    bool sizeChanged{true};

    int swapInterval{0};

    static void GlfwFramebufferSizeCallback(GLFWwindow *w, int width, int height);

    static void GlfwKeyCallback(GLFWwindow *w, int button, int scancode, int action, int modifiers);

    static void GlfwMouseButtonCallback(GLFWwindow *w, int button, int action, int glfwModifierBits);

    static void GlfwCursorPosCallback(GLFWwindow *w, double x, double y);

    static void GlfwScrollCallback(GLFWwindow *w, double xoffset, double yoffset);

    static void GlfwCharCallback(GLFWwindow *w, unsigned int c);

    // FramebufferSizeCallback framebufferSizeCallback;
    KeyCallback keyCallback;
    MouseButtonCallback mouseButtonCallback;
    MousePosCallback mousePosCallback;

public:
    static bool InitGlfw();

    static std::shared_ptr<GlfwWindow> Create(uint width = 640,
                                              uint height = 480,
                                              string title = "Conflagrant");

    ~GlfwWindow() final;

    // void SetFramebufferSizeCallback(FramebufferSizeCallback callback) override;

    void SetKeyCallback(KeyCallback callback) override;

    void SetMouseButtonCallback(MouseButtonCallback callback) override;

    void SetMousePosCallback(MousePosCallback callback) override;

    bool SetCursorMode(CursorMode mode) const override;

    CursorMode GetCursorMode() const override;

    bool MakeContextCurrent() override;

    bool PollEvents() override;

    bool SetSwapInterval(int interval) override;

    int GetSwapInterval() const override;

    bool BeginFrame() override;

    bool FinishFrame(bool renderGui = true) override;

    uvec2 GetSize() const override;

    bool SizeHasChanged(uvec2 &sizeOut) const override;

    double GetTime() const override;

    bool SetTime(double time) override;

    bool SetTitle(string const &title) override;
};
} // namespace cfl

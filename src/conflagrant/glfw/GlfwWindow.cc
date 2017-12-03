#include "GlfwWindow.hh"

#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"

#ifndef GLFW_TRUE
#define GLFW_TRUE 1
#endif

#ifndef GLFW_FALSE
#define GLFW_FALSE 0
#endif

namespace cfl {
using namespace input;

#define MODIFIER(name) {GLFW_MOD_ ## name, Modifier::name}
#define MODIFIER_REVERSE(name) {Modifier::name, GLFW_MOD_ ## name}
std::unordered_map<int, Modifier> ModifiersByGlfwCode = {
        MODIFIER(SHIFT),
        MODIFIER(CONTROL),
        MODIFIER(ALT),
        MODIFIER(SUPER)
};
std::unordered_map<Modifier, int> GlfwModifierBitByCflModifier = {
        MODIFIER_REVERSE(SHIFT),
        MODIFIER_REVERSE(CONTROL),
        MODIFIER_REVERSE(ALT),
        MODIFIER_REVERSE(SUPER)
};

#define KEY(name) {GLFW_KEY_ ## name, Key::name}
#define KEY_CUSTOM(key_name, glfw_name) {GLFW_KEY_ ## glfw_name, Key::key_name}
std::unordered_map<int, Key> KeysByGlfwCode = {
        KEY(SPACE),
        KEY(APOSTROPHE),
        KEY(COMMA),
        KEY(MINUS),
        KEY(PERIOD),
        KEY(SLASH),
        KEY_CUSTOM(DIGIT_0, 0),
        KEY_CUSTOM(DIGIT_1, 1),
        KEY_CUSTOM(DIGIT_2, 2),
        KEY_CUSTOM(DIGIT_3, 3),
        KEY_CUSTOM(DIGIT_4, 4),
        KEY_CUSTOM(DIGIT_5, 5),
        KEY_CUSTOM(DIGIT_6, 6),
        KEY_CUSTOM(DIGIT_7, 7),
        KEY_CUSTOM(DIGIT_8, 8),
        KEY_CUSTOM(DIGIT_9, 9),
        KEY(SEMICOLON),
        KEY(EQUAL),
        KEY(A),
        KEY(B),
        KEY(C),
        KEY(D),
        KEY(E),
        KEY(F),
        KEY(G),
        KEY(H),
        KEY(I),
        KEY(J),
        KEY(K),
        KEY(L),
        KEY(M),
        KEY(N),
        KEY(O),
        KEY(P),
        KEY(Q),
        KEY(R),
        KEY(S),
        KEY(T),
        KEY(U),
        KEY(V),
        KEY(W),
        KEY(X),
        KEY(Y),
        KEY(Z),
        KEY(LEFT_BRACKET),
        KEY(BACKSLASH),
        KEY(RIGHT_BRACKET),
        KEY(GRAVE_ACCENT),
        KEY(WORLD_1),
        KEY(WORLD_2),
        KEY(ESCAPE),
        KEY(ENTER),
        KEY(TAB),
        KEY(BACKSPACE),
        KEY(INSERT),
        KEY(DELETE),
        KEY(RIGHT),
        KEY(LEFT),
        KEY(DOWN),
        KEY(UP),
        KEY(PAGE_UP),
        KEY(PAGE_DOWN),
        KEY(HOME),
        KEY(END),
        KEY(CAPS_LOCK),
        KEY(SCROLL_LOCK),
        KEY(NUM_LOCK),
        KEY(PRINT_SCREEN),
        KEY(PAUSE),
        KEY(F1),
        KEY(F2),
        KEY(F3),
        KEY(F4),
        KEY(F5),
        KEY(F6),
        KEY(F7),
        KEY(F8),
        KEY(F9),
        KEY(F10),
        KEY(F11),
        KEY(F12),
        KEY(F13),
        KEY(F14),
        KEY(F15),
        KEY(F16),
        KEY(F17),
        KEY(F18),
        KEY(F19),
        KEY(F20),
        KEY(F21),
        KEY(F22),
        KEY(F23),
        KEY(F24),
        KEY(F25),
        KEY(KP_0),
        KEY(KP_1),
        KEY(KP_2),
        KEY(KP_3),
        KEY(KP_4),
        KEY(KP_5),
        KEY(KP_6),
        KEY(KP_7),
        KEY(KP_8),
        KEY(KP_9),
        KEY(KP_DECIMAL),
        KEY(KP_DIVIDE),
        KEY(KP_MULTIPLY),
        KEY(KP_SUBTRACT),
        KEY(KP_ADD),
        KEY(KP_ENTER),
        KEY(KP_EQUAL),
        KEY(LEFT_SHIFT),
        KEY(LEFT_CONTROL),
        KEY(LEFT_ALT),
        KEY(LEFT_SUPER),
        KEY(RIGHT_SHIFT),
        KEY(RIGHT_CONTROL),
        KEY(RIGHT_ALT),
        KEY(RIGHT_SUPER),
        KEY(MENU)
};

#define KEY_ACTION(name) {GLFW_ ##name, KeyAction::name}
std::unordered_map<int, KeyAction> KeyActionsByGlfwCode = {
        KEY_ACTION(PRESS),
        KEY_ACTION(REPEAT),
        KEY_ACTION(RELEASE)
};

#define MOUSE_BUTTON(name) {GLFW_MOUSE_ ## name, MouseButton::name}
std::unordered_map<int, MouseButton> MouseButtonsByGlfwCode = {
        MOUSE_BUTTON(BUTTON_1),
        MOUSE_BUTTON(BUTTON_2),
        MOUSE_BUTTON(BUTTON_3),
        MOUSE_BUTTON(BUTTON_4),
        MOUSE_BUTTON(BUTTON_5),
        MOUSE_BUTTON(BUTTON_6),
        MOUSE_BUTTON(BUTTON_7),
        MOUSE_BUTTON(BUTTON_8),
        MOUSE_BUTTON(BUTTON_LAST),
        MOUSE_BUTTON(BUTTON_LEFT),
        MOUSE_BUTTON(BUTTON_RIGHT),
        MOUSE_BUTTON(BUTTON_MIDDLE)
};

#define MOUSE_ACTION(name) {GLFW_ ##name, MouseAction::name}
std::unordered_map<int, MouseAction> MouseActionsByGlfwCode = {
        MOUSE_ACTION(PRESS),
        MOUSE_ACTION(RELEASE)
};

bool GlfwWindow::InitHasBeenCalled = false;

bool encounteredError = false;

void GlfwErrorFunction(int v, const char *str) {
    $
    encounteredError = true;
    LOG_ERROR(glfw) << str << std::endl;
}

#define GLFW_RETURN(x, returnStatement) x; \
if (encounteredError) { \
encounteredError = false; \
returnStatement; \
}
#define GLFW_RETURN_FALSE(x) GLFW_RETURN(x, return false)
#define GLFW_RETURN_NULLPTR(x) GLFW_RETURN(x, return nullptr)
#define GLFW_RETURN_VOID(x) GLFW_RETURN(x, return)

GlfwWindow::~GlfwWindow() {
    $
    glfwDestroyWindow(window);
    ImGui_ImplGlfwGL3_Shutdown();
}

//void GlfwWindow::SetFramebufferSizeCallback(Window::FramebufferSizeCallback callback) {
//    framebufferSizeCallback = callback;
//}

void GlfwWindow::SetKeyCallback(Window::KeyCallback callback) {
    $
    keyCallback = callback;
}

void GlfwWindow::SetMouseButtonCallback(Window::MouseButtonCallback callback) {
    $
    mouseButtonCallback = callback;
}

void GlfwWindow::SetMousePosCallback(Window::MousePosCallback callback) {
    $
    mousePosCallback = callback;
}

bool GlfwWindow::InitGlfw() {
    $
    if (InitHasBeenCalled) {
        LOG_ERROR(cfl::GlfwWindow::InitGlfw()) << "Function has already been called once" << std::endl;
    }
    InitHasBeenCalled = true;

    glfwSetErrorCallback(GlfwErrorFunction);
    if (glfwInit() != GLFW_TRUE) {
        return false;
    }

    GLFW_RETURN_FALSE(glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4));
    GLFW_RETURN_FALSE(glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1));
    GLFW_RETURN_FALSE(glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE));
    GLFW_RETURN_FALSE(glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE));

    return true;
}

std::shared_ptr<GlfwWindow> GlfwWindow::Create(uint width, uint height, string title) {
    $
    if (!InitHasBeenCalled) {
        LOG_ERROR(cfl::GlfwWindow::Create()) << "cfl::GlfwWindow::InitGlfw() must be called before this." << std::endl;
        return nullptr;
    }

    GLFWwindow *window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (window == nullptr) {
        LOG_ERROR(cfl::GlfwWindow::Create) << "glfwCreateWindow returned nullptr" << std::endl;
        GLFW_RETURN_NULLPTR(glfwTerminate());
        return nullptr;
    }

    ImGui_ImplGlfwGL3_Init(window, false);

    auto ret = std::shared_ptr<GlfwWindow>(new GlfwWindow);
    ret->window = window;
    ret->width = width;
    ret->height = height;

    GLFW_RETURN_NULLPTR(glfwSetWindowUserPointer(window, ret.get()));

    GLFW_RETURN_NULLPTR(glfwSetFramebufferSizeCallback(window, GlfwFramebufferSizeCallback));
    GLFW_RETURN_NULLPTR(glfwSetKeyCallback(window, GlfwKeyCallback));
    GLFW_RETURN_NULLPTR(glfwSetMouseButtonCallback(window, GlfwMouseButtonCallback));
    GLFW_RETURN_NULLPTR(glfwSetCursorPosCallback(window, GlfwCursorPosCallback));
    GLFW_RETURN_NULLPTR(glfwSetCharCallback(window, GlfwCharCallback));
    GLFW_RETURN_NULLPTR(glfwSetScrollCallback(window, GlfwScrollCallback));

    return ret;
}

void GlfwWindow::GlfwFramebufferSizeCallback(GLFWwindow *w, int width, int height) {
    $
    auto *win = static_cast<GlfwWindow *>(glfwGetWindowUserPointer(w));

    uint oldWidth = win->width, oldHeight = win->height;
    win->width = static_cast<uint>(width);
    win->height = static_cast<uint>(height);
    win->sizeChanged = (win->width != oldWidth || win->height != oldHeight);

    // win->framebufferSizeCallback(win->width, win->height);
}

void GlfwWindow::GlfwKeyCallback(GLFWwindow *w, int button, int scancode, int action, int glfwModifierBits) {
    $
    static_cast<GlfwWindow *>(glfwGetWindowUserPointer(w))
            ->keyCallback(KeysByGlfwCode[button],
                          KeyActionsByGlfwCode[action],
                          GlfwModifierSet(glfwModifierBits));

    ImGui_ImplGlfwGL3_KeyCallback(button, scancode, action, glfwModifierBits);
}

void GlfwWindow::GlfwMouseButtonCallback(GLFWwindow *w, int button, int action, int glfwModifierBits) {
    $
    static_cast<GlfwWindow *>(glfwGetWindowUserPointer(w))
            ->mouseButtonCallback(MouseButtonsByGlfwCode[button],
                                  MouseActionsByGlfwCode[action],
                                  GlfwModifierSet(glfwModifierBits));

    ImGui_ImplGlfwGL3_MouseButtonCallback(button, action, glfwModifierBits);
}

void GlfwWindow::GlfwCursorPosCallback(GLFWwindow *w, double x, double y) {
    $
    static_cast<GlfwWindow *>(glfwGetWindowUserPointer(w))
            ->mousePosCallback(x, y);
}

void GlfwWindow::GlfwScrollCallback(GLFWwindow *w, double xoffset, double yoffset) {
    $
    ImGui_ImplGlfwGL3_ScrollCallback(xoffset, yoffset);
}

void GlfwWindow::GlfwCharCallback(GLFWwindow *w, unsigned int c) {
    $
    ImGui_ImplGlfwGL3_CharCallback(c);
}

bool GlfwWindow::MakeContextCurrent() {
    $
    GLFW_RETURN_FALSE(glfwMakeContextCurrent(window));

    glewExperimental = GL_TRUE;
    if (glewInit()) {
        LOG_ERROR(glewInit) << "Did not return GL_TRUE" << std::endl;
        return false;
    }

    return true;
}

bool GlfwWindow::PollEvents() {
    $
    sizeChanged = false;
    GLFW_RETURN_FALSE(glfwPollEvents());
    return true;
}

bool GlfwWindow::SetSwapInterval(int interval) {
    $
    GLFW_RETURN_FALSE(glfwSwapInterval(interval));
    swapInterval = interval;
    return true;
}

int GlfwWindow::GetSwapInterval() const {
    $
    return swapInterval;
}

bool GlfwWindow::BeginFrame() {
    $
    ImGui_ImplGlfwGL3_NewFrame();
    return false;
}

bool GlfwWindow::FinishFrame() {
    $
    ImGui::Render();
    GLFW_RETURN_FALSE(glfwSwapBuffers(window));
    return true;
}

uvec2 GlfwWindow::GetSize() const {
    $
    return uvec2(width, height);
}

bool GlfwWindow::SizeHasChanged(uvec2 &sizeOut) const {
    $
    sizeOut.x = width;
    sizeOut.y = height;
    return sizeChanged;
}

double GlfwWindow::GetTime() const {
    $
    return glfwGetTime();
}

bool GlfwWindow::SetTime(double time) {
    $
    GLFW_RETURN_FALSE(glfwSetTime(time));
    return true;
}

bool GlfwWindow::SetCursorMode(CursorMode mode) const {
    $
    int glfwMode = GLFW_CURSOR_NORMAL;
    if (mode == CursorMode::HIDDEN) glfwMode = GLFW_CURSOR_HIDDEN;
    else if (mode == CursorMode::HIDDEN_FIXED) glfwMode = GLFW_CURSOR_DISABLED;

    GLFW_RETURN_FALSE(glfwSetInputMode(window, GLFW_CURSOR, glfwMode));
    return false;
}

CursorMode GlfwWindow::GetCursorMode() const {
    $
    GLFW_RETURN(int glfwMode = glfwGetInputMode(window, GLFW_CURSOR), return CursorMode::NORMAL);

    if (glfwMode == GLFW_CURSOR_NORMAL) return CursorMode::NORMAL;
    if (glfwMode == GLFW_CURSOR_HIDDEN) return CursorMode::HIDDEN;
    return CursorMode::HIDDEN_FIXED;
}

bool GlfwWindow::SetTitle(string const &title) {
    $
    GLFW_RETURN_FALSE(glfwSetWindowTitle(window, title.c_str()));
    return true;
}

GlfwModifierSet::GlfwModifierSet(int glfwModifierBits)
        : glfwModifierBits(glfwModifierBits) { $ }

bool GlfwModifierSet::Test(Modifier modifier) const {
    $
    int glfwModifier = GlfwModifierBitByCflModifier[modifier];
    return (glfwModifierBits & glfwModifier) != 0;
}

std::unordered_set<Modifier> GlfwModifierSet::GetModifiers() const {
    $
    std::unordered_set<Modifier> modifiers;
    if (Test(Modifier::ALT)) modifiers.insert(Modifier::ALT);
    if (Test(Modifier::SHIFT)) modifiers.insert(Modifier::SHIFT);
    if (Test(Modifier::CONTROL)) modifiers.insert(Modifier::CONTROL);
    if (Test(Modifier::SUPER)) modifiers.insert(Modifier::SUPER);
    return modifiers;
}
} // namespace cfl

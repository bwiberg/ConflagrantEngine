#include <conflagrant/conflagrant.hh>
#include <conflagrant/glfw/GlfwWindow.hh>

int RunEditor(std::vector<std::string> const &arguments) {
    if (arguments.empty()) {
        std::cout << "Must provide at least one argument." << std::endl
                  << "The first argument should be a path to a Conflagrant scene file." << std::endl;
        return -1;
    }

#define RETURN_FAIL_ON_FALSE(x) if (!x) { return -1; }

    RETURN_FAIL_ON_FALSE(cfl::GlfwWindow::InitGlfw());

    auto window = cfl::GlfwWindow::Create(320, 240, "Conflagrant Editor");
    if (!window || !window->MakeContextCurrent() || !window->SetSwapInterval(1)) return -1;

    RETURN_FAIL_ON_FALSE(cfl::InitDefaults());

    cfl::Engine engine(std::move(window));
    RETURN_FAIL_ON_FALSE(engine.LoadScene(arguments[0]));

    return engine.Run();
#undef RETURN_FAIL_ON_FALSE
}

int main(int argc, char *argv[]) {
    std::vector<std::string> arguments(argv + 1, argv + argc);
    return RunEditor(arguments);
}

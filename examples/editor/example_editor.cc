#include <conflagrant/conflagrant.hh>
#include <conflagrant/glfw/GlfwWindow.hh>

#include <fstream>
#include <conflagrant/ComponentFactory.hh>
#include <conflagrant/SystemFactory.hh>

struct CompA {
    int a;
    uint count{0};

    inline static cfl::string const &GetName() {
        static const cfl::string name = "CompA";
        return name;
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, CompA &component) {
        SERIALIZE(json["a"], component.a);
        return true;
    }
};

struct CompB {
    std::string a, b;
    uint count{0};

    inline static cfl::string const &GetName() {
        static const cfl::string name = "CompB";
        return name;
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, CompB &component) {
        SERIALIZE(json["a"], component.a);
        SERIALIZE(json["b"], component.b);
        return true;
    }
};

struct SystemA : public cfl::System, public entityx::System<SystemA> {
    void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override {
        auto k = entities.entities_with_components<CompA>();
        for (auto e : k) {
            e.component<CompA>()->count++;
        }
    }

    inline static cfl::string const GetName() {
        return "SystemA";
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, SystemA &name) {
        json["name"] = GetName();
        return true;
    }
};

struct SystemAB : public cfl::System, public entityx::System<SystemAB> {
    void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override {
        auto k = entities.entities_with_components<CompA, CompB>();
        for (auto e : k) {
            e.component<CompA>()->count++;
            e.component<CompB>()->count++;
        }
    }

    inline static cfl::string const GetName() {
        return "SystemAB";
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, SystemAB &name) {
        json["name"] = GetName();
        return true;
    }
};

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

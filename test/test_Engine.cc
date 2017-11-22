#include <gtest/gtest.h>

#include <conflagrant/Engine.hh>
#include <conflagrant/ComponentFactory.hh>
#include <conflagrant/SystemFactory.hh>

struct DummyWindow : public cfl::Window {
    void SetKeyCallback(KeyCallback callback) override {}

    void SetMouseButtonCallback(MouseButtonCallback callback) override {}

    void SetMousePosCallback(MousePosCallback callback) override {}

    bool MakeContextCurrent() override {
        return true;
    }

    bool PollEvents() override {
        return true;
    }

    bool SetSwapInterval(int interval) override {
        return true;
    }

    bool BeginFrame() override {
        return false;
    }

    bool FinishFrame() override {
        return false;
    }

    cfl::uvec2 GetSize() const override {
        return cfl::uvec2();
    }

    double GetTime() const override {
        return 0;
    }

    bool SetTime(double time) override {
        return false;
    }
};

struct CompA {
    CompA() : a(0), count(0) {}

    int a;
    uint count;

    inline static cfl::string const GetName() {
        return "CompA";
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, CompA &component) {
        SERIALIZE(json["a"], component.a);
        return true;
    }
};

struct CompB {
    CompB() : a(""), b(""), count(0) {}

    std::string a, b;
    uint count;

    inline static cfl::string const GetName() {
        return "CompB";
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, CompB &component) {
        SERIALIZE(json["a"], component.a);
        SERIALIZE(json["b"], component.b);
        return true;
    }
};

struct CompUnregistered {
    CompUnregistered() : a(""), b(""), count(0) {}

    std::string a, b;
    uint count;

    inline static cfl::string const GetName() {
        return "CompUnregistered";
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, CompUnregistered &component) {
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

struct SystemUnregistered : public cfl::System, public entityx::System<SystemUnregistered> {
    void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override {
    }

    inline static cfl::string const GetName() {
        return "SystemUnregistered";
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, SystemUnregistered &name) {
        json["name"] = GetName();
        return true;
    }
};

class EngineTest : public ::testing::Test {
public:
    EngineTest() : engine(window = std::static_pointer_cast<cfl::Window>(std::make_shared<DummyWindow>())) {
        REGISTER_SYSTEM(SystemA);
        REGISTER_SYSTEM(SystemAB);

        REGISTER_COMPONENT(CompA);
        REGISTER_COMPONENT(CompB);
    }

    cfl::Engine engine;
    std::shared_ptr<cfl::Window> window;
};

TEST_F(EngineTest, LoadScene_Function_Works) {
    EXPECT_TRUE(engine.LoadScene([](std::shared_ptr<entityx::EntityManager> entities,
                                    std::shared_ptr<entityx::SystemManager> systems) {
        auto entity = entities->create();
        auto compA = entity.assign<CompA>();
        compA->a = 1;

        entity = entities->create();
        compA = entity.assign<CompA>();
        compA->a = 2;

        entity = entities->create();
        compA = entity.assign<CompA>();
        compA->a = 3;
        auto compB = entity.assign<CompB>();
        compB->a = "hello";
        compB->b = "world";

        systems->add<SystemA>();
        systems->add<SystemAB>();

        return true;
    }));

    engine.Run(true);

    auto em = engine.GetEntityManager();
    EXPECT_EQ(3, em->size());

    for (auto e : em->entities_for_debugging()) {
        if (e.has_component<CompB>()) {
            EXPECT_EQ(1, e.component<CompB>()->count);
            EXPECT_EQ(2, e.component<CompA>()->count);
        } else {
            EXPECT_EQ(1, e.component<CompA>()->count);
        }
    }
}

TEST_F(EngineTest, LoadScene_Json_ComponentNotRegistered_Fails) {
    std::istringstream ss(R"(
{
    "entities": [
        {"CompA": {"a": -1}, "CompUnregistered": {"a": "hello", "b": "world"}},
        {"CompA": {"a": 1337}}
    ]
}
)");
    Json::Value json;
    ss >> json;

    EXPECT_FALSE(engine.LoadScene(json));
}

TEST_F(EngineTest, LoadScene_Json_SystemNotRegistered_Fails) {
    std::istringstream ss(R"(
{
    "systems": [
        {"name": "SystemA"}, {"name": "SystemAB"}, {"name": "SystemUnregistered"}
    ]
}
)");
    Json::Value json;
    ss >> json;

    EXPECT_FALSE(engine.LoadScene(json));
}

TEST_F(EngineTest, LoadScene_SaveScene_ReturnsSame) {
    std::istringstream ss(R"(
{
    "entities": [
        {"CompA": {"a": -1}, "CompB": {"a": "hello", "b": "world"}},
        {"CompA": {"a": 1337}}
    ],
    "systems": [
        {"name": "SystemAB"}, {"name": "SystemA"}
    ]
}
)");
    Json::Value jsonOriginal;
    ss >> jsonOriginal;

    EXPECT_TRUE(engine.LoadScene(jsonOriginal));

    Json::Value jsonSerialized;
    EXPECT_TRUE(engine.SaveScene(jsonSerialized));

    EXPECT_EQ(jsonOriginal, jsonSerialized);
}

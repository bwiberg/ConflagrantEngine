#include <gtest/gtest.h>

#include <conflagrant/Engine.hh>
#include <conflagrant/ComponentFactory.hh>
#include <conflagrant/SystemFactory.hh>

class EngineTest : public ::testing::Test {
public:
    cfl::Engine engine;

protected:
    void TearDown() override {
        CLEAR_REGISTERED_COMPONENTS();
        CLEAR_REGISTERED_SYSTEMS();
    }
};

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

struct SystemA : public entityx::System<SystemA> {
    void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override {
        auto k = entities.entities_with_components<CompA>();
        for (auto e : k) {
            e.component<CompA>()->count++;
        }
    }

    inline static cfl::string const &GetName() {
        static const cfl::string name = "SystemA";
        return name;
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, SystemA &name) {
        json["name"] = GetName();
        return true;
    }
};

struct SystemAB : public entityx::System<SystemAB> {
    void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override {
        auto k = entities.entities_with_components<CompA, CompB>();
        for (auto e : k) {
            e.component<CompA>()->count++;
            e.component<CompB>()->count++;
        }
    }

    inline static cfl::string const &GetName() {
        static const cfl::string name = "SystemAB";
        return name;
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, SystemAB &name) {
        json["name"] = GetName();
        return true;
    }
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

    engine.RunOnce();

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

TEST_F(EngineTest, LoadScene_Json_ComponentsAndSystemsRegistered_Works) {
    REGISTER_COMPONENT(CompA);
    REGISTER_COMPONENT(CompB);

    REGISTER_SYSTEM(SystemA);
    REGISTER_SYSTEM(SystemAB);

    std::istringstream ss(R"(
{
    "systems": [
        {"name": "SystemA"}, {"name": "SystemAB"}
    ],
    "entities": [
        {"CompA": {"a": -1}, "CompB": {"a": "hello", "b": "world"}},
        {"CompA": {"a": 1337}}
    ]
}
)");
    Json::Value json;
    ss >> json;

    EXPECT_TRUE(engine.LoadScene(json));

    EXPECT_TRUE(!!engine.GetSystemManager()->system<SystemA>());
    EXPECT_TRUE(!!engine.GetSystemManager()->system<SystemAB>());

    EXPECT_EQ(2, engine.GetEntityManager()->size());
    auto es = engine.GetEntityManager()->entities_for_debugging();
    for (auto e : es) {
        if (e.has_component<CompA>() && e.has_component<CompB>()) {
            auto a = e.component<CompA>();
            EXPECT_EQ(-1, a->a);

            auto b = e.component<CompB>();
            EXPECT_EQ("hello", b->a);
            EXPECT_EQ("world", b->b);
        } else {
            auto a = e.component<CompA>();
            EXPECT_TRUE(a);
            EXPECT_EQ(1337, a->a);
        }
    }
}

TEST_F(EngineTest, LoadScene_Json_ComponentsNotRegistered_Fails) {
    std::istringstream ss(R"(
{
    "entities": [
        {"CompA": {"a": -1}, "CompB": {"a": "hello", "b": "world"}},
        {"CompA": {"a": 1337}}
    ]
}
)");
    Json::Value json;
    ss >> json;

    EXPECT_FALSE(engine.LoadScene(json));
}

TEST_F(EngineTest, LoadScene_Json_SystemsNotRegistered_Fails) {
    std::istringstream ss(R"(
{
    "systems": [
        {"name": "SystemA"}, {"name": "SystemAB"}
    ]
}
)");
    Json::Value json;
    ss >> json;

    EXPECT_FALSE(engine.LoadScene(json));
}

TEST_F(EngineTest, LoadScene_SaveScene_ReturnsSame) {
    REGISTER_COMPONENT(CompA);
    REGISTER_COMPONENT(CompB);

    REGISTER_SYSTEM(SystemA);
    REGISTER_SYSTEM(SystemAB);

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

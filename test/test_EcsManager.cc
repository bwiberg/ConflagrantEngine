/**
 * Created by Benjamin Wiberg on 28/03/2017.
 */

#include <gtest/gtest.h>
#include <gtest/gtest-spi.h>

#include <conflagrant/ecs/EcsManager.hh>
#include "./lib/stl.hh"
#include <conflagrant/ecs/internal/ComponentType.hh>
#include <conflagrant/ecs/internal/SystemType.hh>

#include <string>
#include <array>

using cfl::ecs::EcsManager;
using cfl::ecs::Entity;
using cfl::ecs::Component;
using cfl::ecs::BaseSystem;
using cfl::ecs::internal::ComponentType;
using cfl::ecs::internal::SystemType;
using cfl::ecs::system_id_t;

struct Name : public Component {
    std::string name;
};
struct Color : public Component {
    std::array<uint8_t, 3> rgb;
};

template<typename ...TComponents>
struct TestSystem : public BaseSystem<TComponents...> {
    std::vector<Entity> entities;

    void UpdateEntity(Entity &entity) override {
        entities.push_back(entity);
    }
};

struct System_NameColor : public TestSystem<Name, Color> {
};

struct System_Name : public TestSystem<Name> {
    System_Name(const std::string &name = "") : name(name) {}

    std::string name;
};

struct System_Color : public TestSystem<Color> {
};

class EcsManagerTest : public ::testing::Test {
public:
    std::unique_ptr<EcsManager> manager;
    std::vector<Entity> testEntities_nc;
    std::vector<Entity> testEntities_n;
    std::vector<Entity> testEntities_c;

protected:
    void SetUp() override {
        manager = std::make_unique<EcsManager>();

        ComponentType::RegisterComponentType<Name>("Name");
        ComponentType::RegisterComponentType<Color>("Color");
    }

    void TearDown() override {
        manager = nullptr;
        testEntities_nc.clear();
        testEntities_n.clear();
        testEntities_c.clear();
    }
};

TEST_F(EcsManagerTest, AddingManyEntitiesWorks) {
    manager->CreateEntities<Name, Color>(1000);

}

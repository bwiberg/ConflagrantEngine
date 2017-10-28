/**
 * Created by Benjamin Wiberg on 30/03/2017.
 */

#include <gtest/gtest.h>
#include <gtest/gtest-spi.h>

#include <conflagrant/ecs/EcsManager.hh>
#include <conflagrant/ecs/internal/ComponentType.hh>

using cfl::ecs::EcsManager;
using cfl::ecs::Entity;
using cfl::ecs::Component;
using cfl::ecs::BaseSystem;
using cfl::ecs::internal::ComponentType;
using cfl::ecs::component_id_t;

struct Name : public Component {
    std::string name;

    Name(std::string const &name = "") : name(name) {}
};

struct Color : public Component {
    uint8_t red, green, blue, alpha;

    Color(uint8_t red = 0,
          uint8_t green = 0,
          uint8_t blue = 0,
          uint8_t alpha = 0) : red(red), green(green), blue(blue), alpha(alpha) {}
};

template<typename ...TComponents>
struct TestSystem : public BaseSystem<TComponents...> {
    std::vector<Entity> entities;

    void updateEntity(Entity &entity) override {
        entities.push_back(entity);
    }
};

class ComponentTest : public ::testing::Test {
public:
    std::unique_ptr<EcsManager> manager;

protected:
    void SetUp() override {
        manager = std::make_unique<EcsManager>();

        ComponentType::RegisterComponentType<Name>("Name");
        ComponentType::RegisterComponentType<Color>("Color");
    }

    void TearDown() override {
        manager = nullptr;
    }
};

TEST_F(ComponentTest, ComponentTypesDiffer) {
    component_id_t comp_id_name = ComponentType::TypeId<Name>();
    component_id_t comp_id_color = ComponentType::TypeId<Color>();

    EXPECT_NE(comp_id_name, comp_id_color) << "System IDs of different types should not be equal";
}

TEST_F(ComponentTest, HasComponent_ReturnsFalseBeforeAddingComponent) {
    auto entity1 = manager->createEntity();
    ASSERT_FALSE(entity1.hasComponent<Name>());

    auto entity2 = manager->createEntity<Name, Color>();
    ASSERT_FALSE(entity2.hasComponent<Name>());
    ASSERT_FALSE(entity2.hasComponent<Color>());
}

TEST_F(ComponentTest, AddComponent_WorksForHintedEntity) {
    auto e = manager->createEntity<Name, Color>();
    e.addComponent<Name>();
    EXPECT_TRUE(e.hasComponent<Name>());
    e.addComponent<Color>();
    EXPECT_TRUE(e.hasComponent<Color>());
}

TEST_F(ComponentTest, AddComponent_DoesNotWorkForUnhintedEntity) {
    auto e = manager->createEntity();
    ASSERT_DEATH({e.addComponent<Name>();}, "");
}

TEST_F(ComponentTest, AddComponent_WithParametersWorks) {
    auto e = manager->createEntity<Name>();
    e.addComponent<Name>("__test__");

    EXPECT_EQ("__test__", e.getComponent<Name>().name);
}

TEST_F(ComponentTest, RemoveComponent_Works) {
    auto e = manager->createEntity<Name>();
    e.addComponent<Name>();
    e.removeComponent<Name>();

    EXPECT_FALSE(e.hasComponent<Name>());
}

TEST_F(ComponentTest, AddRemoveComponent_ReaddingRemovedComponentWorks) {
    auto e = manager->createEntity<Name>();
    e.addComponent<Name>();
    e.removeComponent<Name>();
    e.addComponent<Name>();

    EXPECT_TRUE(e.hasComponent<Name>());
}

TEST_F(ComponentTest, AddComponent_AddingComponentTwiceAsserts) {
    auto e = manager->createEntity<Name>();
    e.addComponent<Name>();

    ASSERT_DEATH({e.addComponent<Name>();}, "");
}

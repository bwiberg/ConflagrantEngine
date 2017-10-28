/**
 * Created by Benjamin Wiberg on 28/03/2017.
 */

#include <gtest/gtest.h>
#include <gtest/gtest-spi.h>

#include <conflagrant/ecs/EcsManager.hh>
#include "./lib/stl.hh"
#include <conflagrant/ecs/internal/ComponentType.hh>
#include <conflagrant/ecs/internal/SystemType.hh>

using cfl::ecs::EcsManager;
using cfl::ecs::Entity;
using cfl::ecs::Component;
using cfl::ecs::BaseSystem;
using cfl::ecs::internal::ComponentType;
using cfl::ecs::internal::SystemType;
using cfl::ecs::system_id_t;

struct Name : public Component {
};
struct Color : public Component {
};

template<typename ...TComponents>
struct TestSystem : public BaseSystem<TComponents...> {
    std::vector<Entity> entities;

    void updateEntity(Entity &entity) override {
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

class SystemTest : public ::testing::Test {
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

    void createTestEntities() {
        for (size_t i = 0; i < 3; ++i) {
            Entity e = manager->createEntity<Name, Color>();
            e.addComponent<Name>();
            e.addComponent<Color>();
            testEntities_nc.push_back(e);
            testEntities_n.push_back(e);
            testEntities_c.push_back(e);

            e = manager->createEntity<Name>();
            e.addComponent<Name>();
            testEntities_n.push_back(e);

            e = manager->createEntity<Color>();
            e.addComponent<Color>();
            testEntities_c.push_back(e);
        }
    }
};

TEST_F(SystemTest, SystemTypesDiffer) {
    system_id_t sysid_name = SystemType::TypeId<System_Name>();
    system_id_t sysid_color = SystemType::TypeId<System_Color>();

    EXPECT_NE(sysid_name, sysid_color) << "System IDs of different types should not be equal";
}

TEST_F(SystemTest, HasSystem_ReturnsFalse_BeforeAddingSystem) {
    EXPECT_FALSE(manager->hasSystem<System_NameColor>());
    EXPECT_FALSE(manager->hasSystem<System_Name>());
    EXPECT_FALSE(manager->hasSystem<System_Color>());
}

TEST_F(SystemTest, AddSystem_Works) {
    manager->addSystem<System_NameColor>();
    manager->addSystem<System_Name>();
    manager->addSystem<System_Color>();

    EXPECT_TRUE(manager->hasSystem<System_NameColor>());
    EXPECT_TRUE(manager->hasSystem<System_Name>());
    EXPECT_TRUE(manager->hasSystem<System_Color>());
}

TEST_F(SystemTest, AddSystem_WithParametersWorks) {
    auto system = manager->addSystem<System_Name>("test");

    EXPECT_EQ("test", system->name);
}

TEST_F(SystemTest, AddAndGet_HasSamePointers) {
    auto ptr1 = manager->addSystem<System_Color>();

    EXPECT_EQ(ptr1, manager->getSystem<System_Color>());
}

TEST_F(SystemTest, RemoveSelf_Works) {
    auto system = manager->addSystem<System_NameColor>();
    system->removeSelf();

    EXPECT_FALSE(manager->hasSystem<System_NameColor>());
}

TEST_F(SystemTest, AddSystem_Twice_Asserts) {
    manager->addSystem<System_NameColor>();
    ASSERT_DEATH({ manager->addSystem<System_NameColor>(); }, "Can't add a duplicate system.");
}

TEST_F(SystemTest, UpdateSystems_UpdatesAllAddedEntities) {
    createTestEntities();

    auto sys_nc = manager->addSystem<System_NameColor>();
    auto sys_n = manager->addSystem<System_Name>();
    auto sys_c = manager->addSystem<System_Color>();

    manager->updateSystems();

    ASSERT_TRUE(std::is_permutation(testEntities_nc.cbegin(), testEntities_nc.cend(),
                                    sys_nc->entities.cbegin())) << "System did not update the correct entities.";

    ASSERT_TRUE(std::is_permutation(testEntities_n.cbegin(), testEntities_n.cend(),
                                    sys_n->entities.cbegin())) << "System did not update the correct entities.";

    ASSERT_TRUE(std::is_permutation(testEntities_c.cbegin(), testEntities_c.cend(),
                                    sys_c->entities.cbegin())) << "System did not update the correct entities.";
}

TEST_F(SystemTest, UpdateSystems_DoesNotUpdateEntitiesWithoutRequirements) {
    for (auto& entity : manager->createEntities<Name>(3)) {
        entity.addComponent<Name>();
        testEntities_n.push_back(entity);
    }

    for (auto& entity : manager->createEntities<Color>(3)) {
        entity.addComponent<Color>();
        testEntities_c.push_back(entity);
    }

    auto sys_n = manager->addSystem<System_Name>();
    auto sys_c = manager->addSystem<System_Color>();

    manager->updateSystems();

    EXPECT_FALSE(utility::containsAtLeastOne(testEntities_c, sys_n->entities));
    EXPECT_FALSE(utility::containsAtLeastOne(testEntities_n, sys_c->entities));
}

TEST_F(SystemTest, UpdateSystems_UpdatedEntitiesHaveRequiredComponents) {
    createTestEntities();

    auto sys_nc = manager->addSystem<System_NameColor>();
    auto sys_n = manager->addSystem<System_Name>();
    auto sys_c = manager->addSystem<System_Color>();

    manager->updateSystems();

    for (Entity e : sys_nc->entities) {
        EXPECT_TRUE(e.hasComponent<Name>()) << "Updated entity did not have required component";
        EXPECT_TRUE(e.hasComponent<Color>()) << "Updated entity did not have required component";
    }

    for (Entity e : sys_n->entities) {
        EXPECT_TRUE(e.hasComponent<Name>()) << "Updated entity did not have required component";
    }

    for (Entity e : sys_c->entities) {
        EXPECT_TRUE(e.hasComponent<Color>()) << "Updated entity did not have required component";
    }
}

TEST_F(SystemTest, UpdateSystems_EntityWithRemovedRequiredComponentIsNotUpdated) {
    auto sys_n = manager->addSystem<System_Name>();
    auto e = manager->createEntity<Name>();
    e.addComponent<Name>();
    e.removeComponent<Name>();

    manager->updateSystems();

    EXPECT_TRUE(sys_n->entities.empty());
}

TEST_F(SystemTest, UpdateSystems_EntityWithReaddedRequiredComponentIsUpdated) {
    auto sys_n = manager->addSystem<System_Name>();
    auto e = manager->createEntity<Name>();
    e.addComponent<Name>();
    e.removeComponent<Name>();
    e.addComponent<Name>();

    manager->updateSystems();

    EXPECT_EQ(size_t(1), sys_n->entities.size());
    EXPECT_EQ(e, sys_n->entities[0]);
}

TEST_F(SystemTest, UpdateSystems_DisabledEntitiesAreNotUpdated) {
    auto sys_n = manager->addSystem<System_Name>();
    auto e_disabled = manager->createEntity<Name>();
    e_disabled.addComponent<Name>();
    e_disabled.disable();

    auto e_enabled = manager->createEntity<Name>();
    e_enabled.addComponent<Name>();
    manager->updateSystems();

    EXPECT_TRUE(utility::contains(sys_n->entities, e_enabled));
    EXPECT_FALSE(utility::contains(sys_n->entities, e_disabled));
}

TEST_F(SystemTest, UpdateSystems_DisabledSystemDoesNotUpdate) {
    createTestEntities();

    auto sys = manager->addSystem<System_NameColor>();
    sys->disable();

    manager->updateSystems();

    EXPECT_TRUE(sys->entities.empty()) << "System should not receive updateEntity calls when disabled.";
}

TEST_F(SystemTest, UpdateSystems_DisabledReenabledSystemDoesUpdate) {
    createTestEntities();

    auto sys = manager->addSystem<System_NameColor>();
    sys->disable();
    sys->enable();

    manager->updateSystems();

    EXPECT_FALSE(sys->entities.empty()) << "System should receive updateEntity calls when disabled.";
}

TEST_F(SystemTest, UpdateSystems_ReaddingRemovedSystemWorks) {
    auto sys = manager->addSystem<System_NameColor>();
    sys->removeSelf();
    sys = manager->addSystem<System_NameColor>();

    EXPECT_TRUE(manager->hasSystem<System_NameColor>());
}

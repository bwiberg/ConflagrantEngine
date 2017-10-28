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
            Entity e = manager->CreateEntity<Name, Color>();
            e.AddComponent<Name>();
            e.AddComponent<Color>();
            testEntities_nc.push_back(e);
            testEntities_n.push_back(e);
            testEntities_c.push_back(e);

            e = manager->CreateEntity<Name>();
            e.AddComponent<Name>();
            testEntities_n.push_back(e);

            e = manager->CreateEntity<Color>();
            e.AddComponent<Color>();
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
    EXPECT_FALSE(manager->HasSystem<System_NameColor>());
    EXPECT_FALSE(manager->HasSystem<System_Name>());
    EXPECT_FALSE(manager->HasSystem<System_Color>());
}

TEST_F(SystemTest, AddSystem_Works) {
    manager->AddSystem<System_NameColor>();
    manager->AddSystem<System_Name>();
    manager->AddSystem<System_Color>();

    EXPECT_TRUE(manager->HasSystem<System_NameColor>());
    EXPECT_TRUE(manager->HasSystem<System_Name>());
    EXPECT_TRUE(manager->HasSystem<System_Color>());
}

TEST_F(SystemTest, AddSystem_WithParametersWorks) {
    auto system = manager->AddSystem<System_Name>("test");

    EXPECT_EQ("test", system->name);
}

TEST_F(SystemTest, AddAndGet_HasSamePointers) {
    auto ptr1 = manager->AddSystem<System_Color>();

    EXPECT_EQ(ptr1, manager->GetSystem<System_Color>());
}

TEST_F(SystemTest, RemoveSelf_Works) {
    auto system = manager->AddSystem<System_NameColor>();
    system->RemoveSelf();

    EXPECT_FALSE(manager->HasSystem<System_NameColor>());
}

TEST_F(SystemTest, AddSystem_Twice_Asserts) {
    manager->AddSystem<System_NameColor>();
    ASSERT_DEATH({ manager->AddSystem<System_NameColor>(); }, "Can't add a duplicate system.");
}

TEST_F(SystemTest, UpdateSystems_UpdatesAllAddedEntities) {
    createTestEntities();

    auto sys_nc = manager->AddSystem<System_NameColor>();
    auto sys_n = manager->AddSystem<System_Name>();
    auto sys_c = manager->AddSystem<System_Color>();

    manager->UpdateSystems();

    ASSERT_TRUE(std::is_permutation(testEntities_nc.cbegin(), testEntities_nc.cend(),
                                    sys_nc->entities.cbegin())) << "System did not update the correct entities.";

    ASSERT_TRUE(std::is_permutation(testEntities_n.cbegin(), testEntities_n.cend(),
                                    sys_n->entities.cbegin())) << "System did not update the correct entities.";

    ASSERT_TRUE(std::is_permutation(testEntities_c.cbegin(), testEntities_c.cend(),
                                    sys_c->entities.cbegin())) << "System did not update the correct entities.";
}

TEST_F(SystemTest, UpdateSystems_DoesNotUpdateEntitiesWithoutRequirements) {
    for (auto& entity : manager->CreateEntities<Name>(3)) {
        entity.AddComponent<Name>();
        testEntities_n.push_back(entity);
    }

    for (auto& entity : manager->CreateEntities<Color>(3)) {
        entity.AddComponent<Color>();
        testEntities_c.push_back(entity);
    }

    auto sys_n = manager->AddSystem<System_Name>();
    auto sys_c = manager->AddSystem<System_Color>();

    manager->UpdateSystems();

    EXPECT_FALSE(utility::containsAtLeastOne(testEntities_c, sys_n->entities));
    EXPECT_FALSE(utility::containsAtLeastOne(testEntities_n, sys_c->entities));
}

TEST_F(SystemTest, UpdateSystems_UpdatedEntitiesHaveRequiredComponents) {
    createTestEntities();

    auto sys_nc = manager->AddSystem<System_NameColor>();
    auto sys_n = manager->AddSystem<System_Name>();
    auto sys_c = manager->AddSystem<System_Color>();

    manager->UpdateSystems();

    for (Entity e : sys_nc->entities) {
        EXPECT_TRUE(e.HasComponent<Name>()) << "Updated entity did not have required component";
        EXPECT_TRUE(e.HasComponent<Color>()) << "Updated entity did not have required component";
    }

    for (Entity e : sys_n->entities) {
        EXPECT_TRUE(e.HasComponent<Name>()) << "Updated entity did not have required component";
    }

    for (Entity e : sys_c->entities) {
        EXPECT_TRUE(e.HasComponent<Color>()) << "Updated entity did not have required component";
    }
}

TEST_F(SystemTest, UpdateSystems_EntityWithRemovedRequiredComponentIsNotUpdated) {
    auto sys_n = manager->AddSystem<System_Name>();
    auto e = manager->CreateEntity<Name>();
    e.AddComponent<Name>();
    e.RemoveComponent<Name>();

    manager->UpdateSystems();

    EXPECT_TRUE(sys_n->entities.empty());
}

TEST_F(SystemTest, UpdateSystems_EntityWithReaddedRequiredComponentIsUpdated) {
    auto sys_n = manager->AddSystem<System_Name>();
    auto e = manager->CreateEntity<Name>();
    e.AddComponent<Name>();
    e.RemoveComponent<Name>();
    e.AddComponent<Name>();

    manager->UpdateSystems();

    EXPECT_EQ(size_t(1), sys_n->entities.size());
    EXPECT_EQ(e, sys_n->entities[0]);
}

TEST_F(SystemTest, UpdateSystems_DisabledEntitiesAreNotUpdated) {
    auto sys_n = manager->AddSystem<System_Name>();
    auto e_disabled = manager->CreateEntity<Name>();
    e_disabled.AddComponent<Name>();
    e_disabled.Disable();

    auto e_enabled = manager->CreateEntity<Name>();
    e_enabled.AddComponent<Name>();
    manager->UpdateSystems();

    EXPECT_TRUE(utility::contains(sys_n->entities, e_enabled));
    EXPECT_FALSE(utility::contains(sys_n->entities, e_disabled));
}

TEST_F(SystemTest, UpdateSystems_DisabledSystemDoesNotUpdate) {
    createTestEntities();

    auto sys = manager->AddSystem<System_NameColor>();
    sys->Disable();

    manager->UpdateSystems();

    EXPECT_TRUE(sys->entities.empty()) << "System should not receive updateEntity calls when disabled.";
}

TEST_F(SystemTest, UpdateSystems_DisabledReenabledSystemDoesUpdate) {
    createTestEntities();

    auto sys = manager->AddSystem<System_NameColor>();
    sys->Disable();
    sys->Enable();

    manager->UpdateSystems();

    EXPECT_FALSE(sys->entities.empty()) << "System should receive updateEntity calls when disabled.";
}

TEST_F(SystemTest, UpdateSystems_ReaddingRemovedSystemWorks) {
    auto sys = manager->AddSystem<System_NameColor>();
    sys->RemoveSelf();
    sys = manager->AddSystem<System_NameColor>();

    EXPECT_TRUE(manager->HasSystem<System_NameColor>());
}

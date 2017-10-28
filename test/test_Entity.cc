/**
 * Created by Benjamin Wiberg on 28/03/2017.
 */

#include <gtest/gtest.h>

#include <memory>
#include <conflagrant/ecs/EcsManager.hh>
#include <conflagrant/ecs/Component.hh>
#include <conflagrant/ecs/ComponentFilter.hh>

using cfl::ecs::EcsManager;
using cfl::ecs::Entity;
using cfl::ecs::Component;

struct Name : public Component {};
struct Color : public Component {};

class EntityTest : public ::testing::Test {
public:
    std::unique_ptr<EcsManager> manager;

protected:
    void SetUp() override {
        manager = std::make_unique<EcsManager>();
    }
};

TEST_F(EntityTest, IsValidInitially) {
    auto entity = manager->CreateEntity<Name, Color>();
    EXPECT_TRUE(entity.IsValid());

    auto entities = manager->CreateEntities<Name, Color>(10);
    for (auto entity : entities) {
        EXPECT_TRUE(entity.IsValid());
    }
}

TEST_F(EntityTest, IsInvalidAfterDestruction) {
    auto entity = manager->CreateEntity<Name, Color>();
    entity.Destroy();
    EXPECT_FALSE(entity.IsValid());
    
    auto entities = manager->CreateEntities<Name, Color>(10);
    for (auto entity : entities) {
        entity.Destroy();
        EXPECT_FALSE(entity.IsValid());
    }
}

TEST_F(EntityTest, ClonedEntitiesAreEqual) {
    auto entity = manager->CreateEntity();
    Entity entity_copy(entity);

    EXPECT_EQ(entity, entity_copy);

    Entity entity2 = manager->CreateEntity();
    entity2 = entity;
    EXPECT_EQ(entity, entity2);
}

TEST_F(EntityTest, DifferentEntitiesAreNotEqual) {
    auto entity1 = manager->CreateEntity();
    auto entity2 = manager->CreateEntity();

    EXPECT_FALSE(entity1 == entity2);
}

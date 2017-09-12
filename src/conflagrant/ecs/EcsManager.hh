/**
 * Created by bwiberg on 9/11/17.
 */

#pragma once

#include <memory>
#include <vector>
#include <array>
#include <ostream>
#include <assert.h>

#include "EcsTypes.hh"
#include "Component.hh"
#include "System.hh"

#include "internal/EcsAssert.hh"
#include "internal/EntityStorage.hh"
#include "internal/ComponentType.hh"
#include "internal/SystemType.hh"

class cfl::ecs::Entity final {
    friend class cfl::ecs::EcsManager;

    friend class cfl::ecs::internal::EntityStorage;

    Entity(EcsManager *manager,
           entity_id_t entity_id,
           entity_index_t entity_index);

    /**
     * The lifetime id of this entity. Used to validate entities.
     */
    entity_id_t id;

    /**
     * The index of this entity.
     */
    entity_index_t index;

    /**
     * Reference to the manager of the ECS.
     */
    EcsManager *manager;

public:
    /**
     *
     */
    Entity(Entity const &source);

    /**
     * Assignment operator that makes this handle equal to the other.
     */
    Entity const &operator=(Entity const &rhs);

    bool operator==(Entity const &other) const;

    inline entity_id_t getId() const { return id; }

    inline entity_index_t getIndex() const { return index; }

    /**
     * Adds a component of the given type to this entity. TODO what to do if already has?
     * @return the added component
     */
    template<typename T, typename ...Args>
    T &addComponent(Args &&...args);

    /**
     * Gets the component of the given type attached to this entity.
     */
    template<typename T>
    T &getComponent();

    /**
     * Checks if the entity has a component of the given type.
     */
    template<typename T>
    bool hasComponent();

    /**
     * Removes the component of the given type if it has it.
     */
    template<typename T>
    void removeComponent();

    /**
     * Activates this entity to be processed by systems.
     */
    void enable();

    /**
     * Deactivates this entity to not be processed by systems.
     */
    void disable();

    void destroy();

    bool isValid() const;

    bool isEnabled() const;

    friend std::ostream &operator<<(std::ostream &os, const Entity &entity);
};

class cfl::ecs::EcsManager final {
    friend class cfl::ecs::Entity;

    friend class System;

    enum class SystemState {
        Enabled,
        Disabled,
        NoSystem
    };

    Entity const NullEntity;

    /**
     * Storage of all components.
     */
    internal::EntityStorage entityStorage;

    /**
     * Array of all enabled and disabled entities, by their index.
     */
    std::vector<Entity> entitiesByIndex;

    /**
     * Vector of all systems, ordered by their SystemType ID.
     */
    std::vector<std::shared_ptr<System>> systemsByTypeId;

    std::vector<SystemState> systemStatesByTypeId;

    entity_index_t maxEntityIndex;

    template<typename T>
    std::shared_ptr<T> internalGetSystem(system_id_t system_id);

    void resizeToFitEntityIndex(entity_index_t entity_index);

public:
    EcsManager(entity_index_t initial_entities_capacity = DEFAULT_INITIAL_ENTITIES_CAPACITY);

    ~EcsManager();

    void updateSystems();

    void log(std::ostream &os) const;

    template<typename ...TComponents>
    Entity createEntity();

    Entity createEntity(component_filter_t component_filter);

    template<typename ...TComponents>
    std::vector<Entity> createEntities(entity_index_t num_entities);

    std::vector<Entity> createEntities(component_filter_t component_filter, entity_index_t num_entities);

    template<typename T, typename ...Args>
    std::shared_ptr<T> addSystem(Args &&...args);

    template<typename T>
    std::shared_ptr<T> getSystem();

    template<typename T>
    bool hasSystem();

    system_id_t numSystems() const;

    entity_index_t numEntities() const;
};

namespace cfl {
namespace ecs {
///////////////////////////////////////////////////////////////////
/// Out-of-line definitions of Entity template member functions ///
///////////////////////////////////////////////////////////////////

template<typename T, typename ...Args>
T &Entity::addComponent(Args &&...args) {
    AssertDerivesComponent<T>();
    Assert(isValid(), "Entity invalid.");
    Assert(!hasComponent<T>(), "Entity already has component.");

    return manager->entityStorage.initializeComponent<T>(index, std::forward<Args>(args)...);
};

template<typename T>
T &Entity::getComponent() {
    AssertDerivesComponent<T>();
    Assert(isValid(), "Entity invalid.");
    Assert(hasComponent<T>(), "Entity does not have component.");

    return manager->entityStorage.getComponent<T>(index);
};

template<typename T>
bool Entity::hasComponent() {
    AssertDerivesComponent<T>();
    Assert(isValid(), "Entity invalid.");

    component_id_t component_id = ComponentType::TypeId<T>();
    return manager->entityStorage.hasComponent(index, component_id);
};

template<typename T>
void Entity::removeComponent() {
    AssertDerivesComponent<T>();
    Assert(isValid(), "Entity invalid.");
    Assert(hasComponent<T>(), "Entity does not have component.");

    component_id_t component_id = ComponentType::TypeId<T>();
    manager->entityStorage.removeComponent(index, component_id);
}


///////////////////////////////////////////////////////////////////////
/// Out-of-line definitions of EcsManager template member functions ///
///////////////////////////////////////////////////////////////////////

template<typename ...TComponents>
Entity EcsManager::createEntity() {
    return createEntity(ComponentFilter<TComponents...>::get());
}

template<typename ...TComponents>
std::vector<Entity> EcsManager::createEntities(entity_index_t num_entities) {
    return createEntities(ComponentFilter<TComponents...>::get(), num_entities);
}

template<typename TSystem, typename ...Args>
std::shared_ptr<TSystem> EcsManager::addSystem(Args &&...args) {
    AssertDerivesSystem<TSystem>();

    auto system_id = SystemType::TypeId<TSystem>();
    Assert(!systemsByTypeId[system_id], "Can't add a duplicate system.");

    systemsByTypeId[system_id] = std::make_shared<TSystem>(std::forward<Args>(args)...);
    systemsByTypeId[system_id]->manager = this;
    systemStatesByTypeId[system_id] = SystemState::Enabled;
    return internalGetSystem<TSystem>(system_id);
}

template<typename TSystem>
std::shared_ptr<TSystem> EcsManager::getSystem() {
    AssertDerivesSystem<TSystem>();

    auto system_id = SystemType::TypeId<TSystem>();
    assert(systemsByTypeId[system_id]);

    return internalGetSystem<TSystem>(system_id);
}

template<typename TSystem>
bool EcsManager::hasSystem() {
    AssertDerivesSystem<TSystem>();

    auto system_id = SystemType::TypeId<TSystem>();
    return system_id < static_cast<system_id_t>(systemsByTypeId.size()) &&
           systemsByTypeId[system_id];
}

template<typename TSystem>
std::shared_ptr<TSystem> EcsManager::internalGetSystem(system_id_t system_id) {
    return std::static_pointer_cast<TSystem>(systemsByTypeId[system_id]);
}
} // namespace ecs
} // namespace cfl

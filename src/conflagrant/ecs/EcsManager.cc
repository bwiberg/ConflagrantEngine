/**
 * Created by bwiberg on 9/11/17.
 */

#include <iostream>
#include <limits>
#include "EcsManager.hh"

namespace cfl {
namespace ecs {
EcsManager::EcsManager(entity_index_t initial_entities_capacity)
        : NullEntity(this, std::numeric_limits<entity_id_t>::max(), std::numeric_limits<entity_index_t>::max()),
          entityStorage(),
          systemsByTypeId(MAX_SYSTEM_TYPES),
          systemStatesByTypeId(MAX_SYSTEM_TYPES, SystemState::NoSystem),
          maxEntityIndex(0) {
    resizeToFitEntityIndex(initial_entities_capacity);
}

EcsManager::~EcsManager() {

}

void EcsManager::updateSystems() {
    auto const num_systems = numSystems();
    auto const num_components = ComponentType::GetNumTypes();

    std::shared_ptr<System> system;
    bool should_process_entity;
    for (system_id_t system_id = 0; system_id < num_systems; ++system_id) {

        /// Handle only enabled systems
        if (systemStatesByTypeId[system_id] == SystemState::Enabled) {
            system = systemsByTypeId[system_id];

            for (entity_index_t entity_index = 0; entity_index <= maxEntityIndex; ++entity_index) {
                should_process_entity = true;

                /// Handle only enabled entities
                if (entityStorage.entityData[entity_index].state != EntityState::Enabled) {
                    continue;
                }

                for (component_id_t comp_id = 0; comp_id < num_components; ++comp_id) {
                    if (system->componentBitmask.test(comp_id) &&
                        !entityStorage.entityData[entity_index].componentBitmask.test(comp_id)) {
                        should_process_entity = false;
                        break;
                    }
                }

                if (should_process_entity) {
                    system->updateEntity(entitiesByIndex[entity_index]);
                }
            }
        }
    }
}

void EcsManager::log(std::ostream &os) const {
    size_t num_enabled_systems = 0, num_disabled_systems = 0;
    for (auto &system_state : systemStatesByTypeId) {
        if (system_state == SystemState::Enabled) ++num_enabled_systems;
        if (system_state == SystemState::Disabled) ++num_disabled_systems;
    }

    size_t num_enabled_entities = 0, num_disabled_entities = 0;
    for (EntityData const &entity_data : entityStorage.entityData) {
        if (entity_data.state == EntityState::Enabled) ++num_enabled_entities;
        if (entity_data.state == EntityState::Disabled) ++num_disabled_entities;
    }

    os << "SYSTEMS: "
       << "enabled(" << num_enabled_systems
       << ") disabled(" << num_disabled_systems << ")" << std::endl
       << "ENTITIES: "
       << "enabled(" << num_enabled_entities << ")"
       << " disabled(" << num_disabled_entities << ")" << std::endl;
}

void EcsManager::resizeToFitEntityIndex(entity_index_t entity_index) {
    if (entity_index < entitiesByIndex.size()) {
        return;
    }

    size_t new_size = static_cast<size_t>(1.5 * entity_index);

    entitiesByIndex.resize(new_size, NullEntity);
}

Entity EcsManager::createEntity(component_filter_t component_filter) {
    Entity entity(NullEntity);

    entityStorage.reserveComponentMemory(entity, component_filter);
    entityStorage.entityData[entity.index].state = EntityState::Enabled;
    resizeToFitEntityIndex(entity.index);
    entitiesByIndex[entity.index] = entity;

    maxEntityIndex = std::max(maxEntityIndex, entity.index);

    return entity;
}

std::vector<Entity> EcsManager::createEntities(component_filter_t component_filter, entity_index_t num_entities) {
    std::vector<Entity> entities(num_entities, NullEntity);

    entityStorage.reserveComponentMemory(entities, component_filter);
    resizeToFitEntityIndex(entities[entities.size() - 1].index);

    for (Entity &entity : entities) {
        entitiesByIndex[entity.index] = entity;
        entityStorage.entityData[entity.index].state = EntityState::Enabled;
    }

    maxEntityIndex = std::max(maxEntityIndex, entities[entities.size() - 1].index);

    return entities;
}

system_id_t EcsManager::numSystems() const {
    return static_cast<system_id_t>(systemsByTypeId.size());
}

entity_index_t EcsManager::numEntities() const {
    assert(false && "numEntities not implemented");
}
} // namespace ecs
} // namespace cfl

/**
 * Created by bwiberg on 9/11/17.
 */

#include <assert.h>
#include <limits>
#include <iostream>

#include "EntityStorage.hh"
#include "../EcsManager.hh"

namespace cfl {
namespace ecs {
namespace internal {
const EntityData EntityData::Null = EntityData{std::numeric_limits<entity_id_t>::max(),
                                               component_bitmask_t{0},
                                               EntityState::Destroyed};

EntityStorage::EntityStorage(size_t initial_memory, entity_index_t initial_entities_capacity)
        : megaArray(initial_memory),
          componentOffsetsByType(ComponentType::GetNumTypes()),
          currentMaxIndex(0),
          nextEntityOffset(0),
          nextEntityIndex(0) {
    ReserveTablesForEntityIndex(initial_entities_capacity);
}

void EntityStorage::ReserveComponentMemory(Entity &entity,
                                           component_bitmask_t const &component_bitmask) {
    auto &freeEntityIndices = freeEntityIndicesByComponentBitmask[component_bitmask];
    if (freeEntityIndices.empty()) {
        /// reserve new (unused) memory for the entity's components
        entity.index = nextEntityIndex++;
        ReserveTablesForEntityIndex(entity.index);
        entity.id = nextEntityIds[entity.index]++;

        component_offset_t offset_sum = 0;
        for (component_id_t ic = 0; ic < ComponentType::GetNumTypes(); ++ic) {
            if (component_bitmask.test(ic)) {
                componentOffsetsByType[ic][entity.index] = storage::StoreActualComponentOffset(
                        offset_sum);
                offset_sum += ComponentType::GetComponentSize(ic);
            } else {
                componentOffsetsByType[ic][entity.index] = storage::InvalidComponentOffset;
            }
        }
        // offset_sum is now the cumulative size of all components

        entityOffsets[entity.index] = nextEntityOffset;
        nextEntityOffset += offset_sum;

        /// 4) Sanity-check that our MegaArray is still not full
        Assert(nextEntityOffset <= megaArray.capacity(), "MegaArray of components ran out of memory.");
    } else {
        /// reuse an already destroyed entity's memory for its components
        ReuseReservedComponentMemory(entity, freeEntityIndices);
    }

    StoreEntityData(entity);
}

void EntityStorage::ReserveComponentMemory(std::vector<Entity> &entities,
                                           component_bitmask_t const &component_bitmask) {
    entity_index_t i_entity = 0;

    auto &freeEntityIndices = freeEntityIndicesByComponentBitmask[component_bitmask];

    ///
    /// Re-use already allocated memory for as many entities as possible
    ///

    while (!freeEntityIndices.empty() && i_entity < entities.size()) {
        ReuseReservedComponentMemory(entities[i_entity], freeEntityIndices);
        StoreEntityData(entities[i_entity]);
        ++i_entity;
    }

    entity_index_t n_entities_left = static_cast<entity_index_t>(entities.size()) - i_entity;

    ///
    /// Reserve new memory for the rest of the entities
    ///

    if (n_entities_left > 0) {
        /// 1) Make sure lookup tables has enough reserved entries
        ReserveTablesForEntityIndex(nextEntityIndex + n_entities_left - 1);

        /// 2) Reserve new component memory for one "template" entity
        Entity &template_entity = entities[i_entity];
        template_entity.index = nextEntityIndex++;
        template_entity.id = nextEntityIds[template_entity.index]++;

        StoreEntityData(template_entity);

        component_offset_t offset_sum = 0;
        for (component_id_t ic = 0; ic < ComponentType::GetNumTypes(); ++ic) {
            if (component_bitmask.test(ic)) {
                componentOffsetsByType[ic][template_entity.index] = storage::StoreActualComponentOffset(
                        offset_sum + ComponentType::GetComponentSize(ic));
                offset_sum += ComponentType::GetComponentSize(ic);
            } else {
                componentOffsetsByType[ic][template_entity.index] = storage::InvalidComponentOffset;
            }
        }
        // offset_sum is now the cumulative size of all components

        entityOffsets[template_entity.index] = nextEntityOffset;
        nextEntityOffset += offset_sum;

        ++i_entity;

        /// 3) reuse template entity offsets for subsequent entities
        for (; i_entity < entities.size(); ++i_entity) {
            entities[i_entity].index = nextEntityIndex++;
            entities[i_entity].id = nextEntityIds[entities[i_entity].index]++;
            StoreEntityData(entities[i_entity]);

            for (component_id_t comp_id = 0; comp_id < ComponentType::GetNumTypes(); ++comp_id) {
                componentOffsetsByType[comp_id][entities[i_entity].index] = componentOffsetsByType[comp_id][template_entity.index];
            }

            entityOffsets[entities[i_entity].index] = nextEntityOffset;
            nextEntityOffset += offset_sum;
        }
    }

    /// 4) Sanity-check that our MegaArray is still not full
    Assert(nextEntityOffset <= megaArray.size(), "Component MegaArray ran out of memory.");
}

void EntityStorage::FreeEntityMemory(Entity &e) {
    freeEntityIndicesByComponentBitmask[entityData[e.index].componentBitmask].push(e.index);
    entityData[e.index].componentBitmask.reset();
}

void EntityStorage::ReserveTablesForEntityIndex(entity_index_t index) {
    if (index < currentMaxIndex) {
        return;
    }

    auto new_max_index = static_cast<entity_index_t>(1.5 * index);
    currentMaxIndex = new_max_index;

    entityData.resize(new_max_index, EntityData::Null);
    entityOffsets.resize(new_max_index, 0);

    for (auto &componentOffsetArray : componentOffsetsByType) {
        componentOffsetArray.resize(new_max_index, storage::InvalidComponentOffset);
    }

    nextEntityIds.resize(new_max_index, 0);
}

void EntityStorage::ReuseReservedComponentMemory(Entity &e, std::queue<entity_index_t> &freeEntityIndices) {
    e.index = freeEntityIndices.front();
    e.id = nextEntityIds[e.index]++;
    freeEntityIndices.pop();
}

void EntityStorage::StoreEntityData(Entity const &entity) {
    entityData[entity.index].id = entity.id;
}
} // namespace internal
} // namespace ecs
} // namespace cfl

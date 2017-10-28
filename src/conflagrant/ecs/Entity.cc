/**
 * Created by bwiberg on 9/11/17.
 */

#include "EcsManager.hh"

namespace cfl {
namespace ecs {
Entity::Entity(EcsManager *manager,
               entity_id_t entity_id,
               entity_index_t entity_index)
        : id(entity_id),
          index(entity_index),
          manager(manager) {}

Entity::Entity(Entity const &source)
        : id(source.id),
          index(source.index),
          manager(source.manager) {}

Entity const &Entity::operator=(Entity const &rhs) {
    id = rhs.id;
    index = rhs.index;
    manager = rhs.manager;

    return *this;
}

bool Entity::operator==(Entity const &other) const {
    return index == other.index && id == other.id;
}

void Entity::Enable() {
    manager->entityStorage.entityData[index].state = EntityState::Enabled;
}

void Entity::Disable() {
    manager->entityStorage.entityData[index].state = EntityState::Disabled;
}

void Entity::Destroy() {
    manager->entityStorage.entityData[index].state = EntityState::Destroyed;
    manager->entityStorage.FreeEntityMemory(*this);
}

bool Entity::IsValid() const {
    return manager->entityStorage.entityData[index].state != EntityState::Destroyed &&
           id == manager->entityStorage.entityData[index].id;
}

bool Entity::IsEnabled() const {
    Assert(IsValid(), "isEnabled called on invalid Entity handle");
    return manager->entityStorage.entityData[index].state == EntityState::Enabled;
}

std::ostream &operator<<(std::ostream &os, const Entity &entity) {
    os << "index: " << entity.index << " id: " << entity.id;
    return os;
};
} // namespace ecs
} // namespace cfl


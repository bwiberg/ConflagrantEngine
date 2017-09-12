/**
 * Created by bwiberg on 9/11/17.
 */

#include <algorithm>

#include "System.hh"
#include "EcsManager.hh"

namespace cfl {
namespace ecs {
auto System::findSystemIndex() const {
    assert(manager);
    auto &systems = manager->systemsByTypeId;

    return std::find_if(systems.cbegin(), systems.cend(), [&](auto const &system_ptr) {
        return system_ptr.get() == this;
    }) - systems.cbegin();
}

bool System::isValid() const {
    auto index = findSystemIndex();
    return static_cast<size_t>(index) < manager->systemsByTypeId.size() &&
           manager->systemStatesByTypeId[index] != EcsManager::SystemState::NoSystem;
}

void System::enable() {
    assert(isValid());
    auto index = findSystemIndex();
    manager->systemStatesByTypeId[index] = EcsManager::SystemState::Enabled;
}

void System::disable() {
    assert(isValid());
    auto index = findSystemIndex();
    manager->systemStatesByTypeId[index] = EcsManager::SystemState::Disabled;
}

void System::removeSelf() {
    assert(isValid());
    auto index = findSystemIndex();
    manager->systemsByTypeId[index] = nullptr;
    manager->systemStatesByTypeId[index] = EcsManager::SystemState::NoSystem;
}
} // namespace ecs
} // namespace cfl

/**
 * Created by bwiberg on 9/11/17.
 */

#include <algorithm>

#include "System.hh"
#include "EcsManager.hh"

namespace cfl {
namespace ecs {
auto System::FindSystemIndex() const {
    assert(manager);
    auto &systems = manager->systemsByTypeId;

    return std::find_if(systems.cbegin(), systems.cend(), [&](auto const &system_ptr) {
        return system_ptr.get() == this;
    }) - systems.cbegin();
}

bool System::IsValid() const {
    auto index = FindSystemIndex();
    return static_cast<size_t>(index) < manager->systemsByTypeId.size() &&
           manager->systemStatesByTypeId[index] != EcsManager::SystemState::NoSystem;
}

void System::Enable() {
    assert(IsValid());
    auto index = FindSystemIndex();
    manager->systemStatesByTypeId[index] = EcsManager::SystemState::Enabled;
}

void System::Disable() {
    assert(IsValid());
    auto index = FindSystemIndex();
    manager->systemStatesByTypeId[index] = EcsManager::SystemState::Disabled;
}

void System::RemoveSelf() {
    assert(IsValid());
    auto index = FindSystemIndex();
    manager->systemsByTypeId[index] = nullptr;
    manager->systemStatesByTypeId[index] = EcsManager::SystemState::NoSystem;
}
} // namespace ecs
} // namespace cfl

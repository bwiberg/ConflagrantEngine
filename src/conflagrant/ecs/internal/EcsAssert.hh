/**
 * Created by bwiberg on 9/11/17.
 */

#pragma once

#include <assert.h>

#include "../EcsTypes.hh"

namespace cfl {
namespace ecs {
namespace internal {
#define Assert(condition, message) assert(condition && message)

template<typename T>
void AssertDerivesComponent() {
    static_assert(std::is_base_of<Component, T>::value, "Specified class must derive from Component.");
}

template<typename T>
void AssertDerivesSystem() {
    static_assert(std::is_base_of<System, T>::value, "Specified class must derive from System.");
}
} // namespace internal
} // namespace ecs
} // namespace cfl

/**
 * Created by bwiberg on 9/11/17.
 */

#pragma once

#include "ClassType.hh"
#include "../EcsTypes.hh"
#include "InternalEcsTypes.hh"

namespace cfl {
namespace ecs {
namespace internal {
/**
 * Convenience typedef for enumerating System derived classes
 * from 0 and upwards.
 */
typedef ::cfl::ecs::internal::ClassType<system_id_t, System> SystemType;
} // namespace internal
} // namespace ecs
} // namespace cfl

/**
 * Created by bwiberg on 9/11/17.
 */

#include "ComponentType.hh"

std::array<cfl::ecs::internal::ComponentType::Data, MAX_COMPONENT_TYPES> cfl::ecs::internal::ComponentType::ComponentData;

cfl::ecs::internal::component_id_t cfl::ecs::internal::ComponentType::NumRegisteredTypes = 0;

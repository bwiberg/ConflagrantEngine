/**
 * Created by bwiberg on 9/11/17.
 */

#pragma once

#include <ostream>
#include <array>
#include <iostream>
#include <memory>

#include "internal/InternalEcsTypes.hh"
#include "internal/EcsAssert.hh"
#include "internal/ComponentType.hh"

namespace cfl {
namespace ecs {
typedef internal::component_bitmask_t component_filter_t;

template<typename ...TComponents>
class ComponentFilter {
    ComponentFilter() = delete;

public:
    /**
     * Create a filter with the given components.
     */
    static component_filter_t const Get() {
        auto constexpr num_components = sizeof...(TComponents);
        internal::component_id_t component_ids[] = {internal::ComponentType::TypeId<TComponents>()...};

        internal::component_bitmask_t bitmask{0};
        for (size_t i = 0; i < num_components; ++i) {
            bitmask.set(component_ids[i]);
        }

        return bitmask;
    }
};
} // namespace ecs
} // namespace cfl

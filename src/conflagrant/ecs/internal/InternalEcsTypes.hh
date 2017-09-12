/**
 * Created by bwiberg on 9/11/17.
 */

#pragma once

#include <bitset>
#include <vector>

namespace cfl {
namespace ecs {
namespace internal {
typedef uint32_t entity_id_t;

typedef std::bitset<MAX_COMPONENT_TYPES> component_bitmask_t;

typedef uint32_t component_id_t;

typedef char system_id_t;

typedef size_t entity_offset_t;
typedef size_t component_offset_t;

typedef std::vector<uint8_t> MegaArray;

namespace storage {
const component_offset_t InvalidComponentOffset = 0;

inline bool IsValidComponentOffset(component_offset_t component_offset) {
    return component_offset != InvalidComponentOffset;
}

inline component_offset_t GetActualComponentOffset(component_offset_t stored_component_offset) {
    return stored_component_offset - 1;
}

inline component_offset_t StoreActualComponentOffset(component_offset_t actual_component_offset) {
    return actual_component_offset + 1;
}
}
} // namespace internal
} // namespace ecs
} // namespace cfl

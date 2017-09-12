/**
 * Created by bwiberg on 9/11/17.
 *
 * Inspired by the open source entity system "anax", by Miguel Martin.
 * https://github.com/miguelmartin75/anax/blob/448e6c3f545e6bddcb0dd95daed52464c1f78bc0/include/anax/detail/ClassTypeId.hpp
 */

#pragma once

#include <memory>

namespace cfl {
namespace ecs {
namespace internal {
/**
 * Static helper class to get a consistent enumeration of all derived classes
 * of a specific base class.
 */
template<typename TId, typename TBase>
class ClassType {
    /**
     * Internal counter that holds the type id of the next derived class.
     */
    static TId NextTypeId;

    /**
     * Private constructor to disable instantiation.
     */
    ClassType() {}

public:
    /**
     * Get the type id of a derived class of the given base class.
     * Guaranteed to be unique and increasing starting from 0.
     */
    template<typename TDerived>
    static TId TypeId() {
        static_assert(std::is_base_of<TBase, TDerived>::value, "Specified class must derive from the base class.");

        static const TId typeId = NextTypeId++;
        return typeId;
    };
};

template<typename TId, typename TBase>
TId ClassType<TId, TBase>::NextTypeId = 0;
} // namespace internal
} // namespace ecs
} // namespace cfl

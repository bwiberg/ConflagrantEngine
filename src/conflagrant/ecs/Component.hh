/**
 * Created by bwiberg on 9/11/17.
 */

#pragma once

namespace cfl {
namespace ecs {
/**
 * Base class for components in the ECS architecture.
 *
 * For each constructor, must provide a public member function "reset"
 * that takes the same parameters.
 */
struct Component {
    /**
     * Pure virtual destructor to make class abstract.
     */
    virtual ~Component() = 0;
};

inline Component::~Component() {}

} // namespace ecs
} // namespace cfl

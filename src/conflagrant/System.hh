#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>

#include <conflagrant/InputManager.hh>

#include <entityx/System.h>

namespace cfl {
class Engine;

struct System {
    /**
     * Assigned by cfl::Engine.
     */
    std::shared_ptr<InputManager const> input{nullptr};

    /**
     * Assigned by cfl::Engine.
     */
    std::shared_ptr<Window> window{nullptr};

    /**
     * Assigned by cfl::Engine.
     */
    Engine const *engine{nullptr};
};
} // namespace cfl

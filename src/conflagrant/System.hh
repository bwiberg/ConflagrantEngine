#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>

#include "InputManager.hh"

namespace cfl {
struct System {
    /**
     * Assigned by cfl::Engine.
     */
    std::shared_ptr<InputManager const> input{nullptr};

    /**
     * assigned by cfl::Engine.
     */
    std::shared_ptr<Window const> window{nullptr};
};
} // namespace cfl

#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>

#include "Input.hh"

namespace cfl {
struct System {
    /**
     * Assigned by cfl::Engine.
     */
    std::shared_ptr<Input const> input{nullptr};

    /**
     * assigned by cfl::Engine.
     */
    std::shared_ptr<Window const> window{nullptr};
};
} // namespace cfl
